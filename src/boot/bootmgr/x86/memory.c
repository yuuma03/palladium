/* SPDX-FileCopyrightText: (C) 2023 ilmmatias
 * SPDX-License-Identifier: BSD-3-Clause */

#include <memory.h>
#include <string.h>
#include <x86/bios.h>

BiosMemoryRegion *BiosMemoryMap = NULL;
uint32_t BiosMemoryMapEntries = 0;
uint64_t BiosMemorySize = 0;

/*-------------------------------------------------------------------------------------------------
 * PURPOSE:
 *     This function sets up the memory allocator for the boot manager.
 *
 * PARAMETERS:
 *     BootBlock - Information obtained while setting up the boot environment.
 *
 * RETURN VALUE:
 *     None.
 *-----------------------------------------------------------------------------------------------*/
void BmInitMemory(void *BootBlock) {
    BiosBootBlock *Data = (BiosBootBlock *)BootBlock;

    /* We should have booted from the startup module, so it should be safe to assume that
     * that memory map is already sane. */
    BiosMemoryMap = (BiosMemoryRegion *)(uintptr_t)Data->MemoryRegions;
    BiosMemoryMapEntries = Data->MemoryCount;

    /* The startup module doesn't reserve the first MiB, we need to do that manually. */
    if (BiosMemoryMap->Length > 0x100000) {
        memmove(
            BiosMemoryMap + 1,
            BiosMemoryMap + 2,
            (BiosMemoryMapEntries - 2) * sizeof(BiosMemoryMapEntries));

        BiosMemoryRegion *Region = BiosMemoryMap + 1;

        Region->BaseAddress = BiosMemoryMap->BaseAddress + 0x100000;
        Region->Length = BiosMemoryMap->Length - 0x100000;
        Region->Type = BIOS_MEMORY_REGION_TYPE_AVAILABLE;
        BiosMemoryMapEntries++;

        BiosMemoryMap->Length = 0x100000;
        BiosMemoryMap->Type = BIOS_MEMORY_REGION_TYPE_USED;
    } else {
        BiosMemoryMap->Type = BIOS_MEMORY_REGION_TYPE_USED;
    }

    /* In the current state, free (and boot used) entries should pretty much dictate how much
       physical memory we have.
       Anything beyond them, we don't really care (device IO probably?). */
    BiosMemoryRegion *Region = BiosMemoryMap;
    for (uint32_t i = 0; i < BiosMemoryMapEntries; i++) {
        if ((Region->Type == BIOS_MEMORY_REGION_TYPE_AVAILABLE ||
             Region->Type >= BIOS_MEMORY_REGION_TYPE_USED) &&
            Region->BaseAddress + Region->Length > BiosMemorySize) {
            BiosMemorySize = Region->BaseAddress + Region->Length;
        }

        Region++;
    }
}

/*-------------------------------------------------------------------------------------------------
 * PURPOSE:
 *     This function allocates the specified number of pages for use by the boot manager.
 *
 * PARAMETERS:
 *     Pages - How many pages to allocate; The total amount of memory will be Pages * 4096.
 *     Type - Type of the allocation.
 *
 * RETURN VALUE:
 *     Base address of the allocation, or NULL if there is no memory left.
 *-----------------------------------------------------------------------------------------------*/
void *BmAllocatePages(size_t Pages, int Type) {
    size_t Size = Pages << PAGE_SHIFT;

    if (Type == MEMORY_BOOT) {
        Type = BIOS_MEMORY_REGION_TYPE_USED;
    } else {
        Type = BIOS_MEMORY_REGION_TYPE_KERNEL;
    }

    for (uint32_t i = 0; i < BiosMemoryMapEntries; i++) {
        BiosMemoryRegion *Region = &BiosMemoryMap[i];

        if (Region->Type != BIOS_MEMORY_REGION_TYPE_AVAILABLE || Region->Length < Size) {
            continue;
        }

        /* TODO: This is not the right way of doing this, we should use a linked list
         * instead. We have three cases to take into consideration: Exact match (consuming
         * the rest of this entry); Just flip the type to USED. End of the list; This entry
         * becomes USED, and takes the exact size of the allocated region; A new region is
         * added to the end, containing the remaining AVAILABLE memory. Middle of the list;
         * We need to memmove the entire list first (this is the TODO/to optimize part),
         * before executing the same procedure as above. */

        if (Region->Length == Size) {
            Region->Type = Type;
            return (void *)Region->BaseAddress;
        } else if (i != BiosMemoryMapEntries - 1) {
            memmove(
                Region + 2, Region + 1, (BiosMemoryMapEntries - i - 1) * sizeof(BiosMemoryRegion));
        }

        BiosMemoryRegion *NewRegion = Region + 1;

        NewRegion->BaseAddress = Region->BaseAddress + Size;
        NewRegion->Length = Region->Length - Size;
        NewRegion->Type = BIOS_MEMORY_REGION_TYPE_AVAILABLE;

        Region->Length = Size;
        Region->Type = Type;

        BiosMemoryMapEntries += 1;

        return (void *)Region->BaseAddress;
    }

    return NULL;
}

/*-------------------------------------------------------------------------------------------------
 * PURPOSE:
 *     This function returns the specified range of pages for use by the allocator.
 *     The pages should have been allocated as MEMORY_BOOT instead of MEMORY_KERNEL.
 *
 * PARAMETERS:
 *     Base - Base address returned by BmAllocatePages.
 *     Pages - How many pages were originally allocated by BmAllocatePages.
 *
 * RETURN VALUE:
 *     None.
 *-----------------------------------------------------------------------------------------------*/
void BmFreePages(void *Base, size_t Pages) {
    size_t Size = Pages << PAGE_SHIFT;

    for (uint32_t i = 0; i < BiosMemoryMapEntries; i++) {
        BiosMemoryRegion *Region = &BiosMemoryMap[i];

        /* We want an exact match; You do need to pass the exact same amount of pages as you
           did for BmAllocatePages! */
        if (Region->Type != BIOS_MEMORY_REGION_TYPE_USED || Region->BaseAddress != (size_t)Base ||
            Region->Length != Size) {
            continue;
        }

        /* Merge with neighbours to decrease fragmentation; First merge forward, then backwards.
         */
        Region->Type = BIOS_MEMORY_REGION_TYPE_AVAILABLE;

        if (i + 1 < BiosMemoryMapEntries &&
            BiosMemoryMap[i + 1].Type == BIOS_MEMORY_REGION_TYPE_AVAILABLE &&
            BiosMemoryMap[i + 1].BaseAddress == Region->BaseAddress + Region->Length) {
            Region->Length += BiosMemoryMap[i + 1].Length;
            memmove(
                Region + 1, Region + 2, (BiosMemoryMapEntries - i - 2) * sizeof(BiosMemoryRegion));
            BiosMemoryMapEntries -= 1;
        }

        if (i > 0 && BiosMemoryMap[i - 1].Type == BIOS_MEMORY_REGION_TYPE_AVAILABLE &&
            BiosMemoryMap[i - 1].BaseAddress + BiosMemoryMap[i - 1].Length == Region->BaseAddress) {
            BiosMemoryMap[i - 1].Length += Region->Length;
            memmove(Region, Region + 1, (BiosMemoryMapEntries - i - 1) * sizeof(BiosMemoryRegion));
            BiosMemoryMapEntries -= 1;
        }

        break;
    }
}
