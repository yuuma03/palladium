/* SPDX-FileCopyrightText: (C) 2023 ilmmatias
 * SPDX-License-Identifier: BSD-3-Clause */

#include <boot.h>
#include <memory.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <x86/bios.h>
#include <x86/cpuid.h>

#define LOADER_MAGIC "BMGR"
#define LOADER_CURRENT_VERSION 0x0000

typedef struct __attribute__((packed)) {
    char Magic[4];
    uint16_t Version;
    struct {
        uint64_t BaseAdress;
        int IsXsdt;
    } Acpi;
    struct {
        uint64_t MemorySize;
        uint64_t PageAllocatorBase;
    } MemoryManager;
    struct {
        uint64_t BaseAddress;
        uint32_t Count;
    } MemoryMap;
    struct {
        uint64_t BaseAddress;
        uint32_t Count;
    } Images;
} LoaderBootData;

extern uint64_t BiosRsdtLocation;
extern int BiosIsXsdt;

extern BiosMemoryRegion *BiosMemoryMap;
extern uint32_t BiosMemoryMapEntries;
extern uint64_t BiosMemorySize;
extern uint64_t BiosMaxAddressableMemory;

[[noreturn]] void BiTransferExecution(uint64_t *Pml4, uint64_t BootData, uint64_t EntryPoint);

/*-------------------------------------------------------------------------------------------------
 * PURPOSE:
 *     This function sets up the paging structures, and prepares to put the processor in the
 *     correct state to jump into the kernel.
 *
 * PARAMETERS:
 *     Images - List containing all memory regions we need to map.
 *     ImageCount - How many entries the image list has.
 *
 * RETURN VALUE:
 *     Does not return.
 *-----------------------------------------------------------------------------------------------*/
[[noreturn]] void BmTransferExecution(LoadedImage *Images, size_t ImageCount) {
    /* TODO: Add support for PML5 (under compatible processors). */

    /* Currently, we load the kernel expecting at most 16TiB of addressable physical memory; Any
       more than this, and we'll assume we're running on an incompatible platform. */
    if (BiosMaxAddressableMemory > 0x100000000000) {
        BmPanic("An error occoured while trying to load the selected operating system.\n"
                "The maximum addressable RAM address in the memory map is too big.");
    }

    /* Check for 1GiB page support; If we do support it, it reduces the amount of work to map all
       of the physical memory. */
    uint32_t Eax, Ebx, Ecx, Edx;
    __cpuid(0x80000001, Eax, Ebx, Ecx, Edx);

    /* Pre-allocate the space required for the kernel's physical memory manager; This way
       the kernel doesn't need to worry about anything but filling the info.
       MmSize should always be `PagesOfMemory * sizeof(MmPageEntry)`, if MmPageEntry changes
       on the kernel, the size here needs to change too! */
    uint64_t PagesOfMemory = (BiosMemorySize + PAGE_SIZE - 1) >> PAGE_SHIFT;
    uint64_t MmSize = PagesOfMemory * 29;
    void *MmBase = BmAllocatePages((MmSize + PAGE_SIZE - 1) >> PAGE_SHIFT, MEMORY_KERNEL);
    if (!MmBase) {
        BmPanic("An error occoured while trying to load the selected operating system.\n"
                "There is not enough RAM for the memory manager.");
    }

    uint64_t Slices512GiB = (BiosMaxAddressableMemory + 0x7FFFFFFFFF) >> 39;
    uint64_t Slices1GiB = (BiosMaxAddressableMemory + 0x3FFFFFFF) >> 30;
    uint64_t Slices2MiB = (BiosMaxAddressableMemory + 0x1FFFFF) >> 21;

    do {
        int Fail = 0;
        uint64_t *Pml4 = BmAllocatePages(1, MEMORY_KERNEL);
        uint64_t *EarlyIdentPdpt = BmAllocatePages(1, MEMORY_KERNEL);
        uint64_t *LateIdentPdpt = BmAllocatePages(Slices512GiB, MEMORY_KERNEL);
        uint64_t *KernelPdpt = BmAllocatePages(1, MEMORY_KERNEL);
        uint64_t *EarlyIdentPdt = BmAllocatePages(1, MEMORY_KERNEL);
        LoaderBootData *BootData = malloc(sizeof(LoaderBootData));

        if (!Pml4 || !EarlyIdentPdpt || !LateIdentPdpt || !EarlyIdentPdt || !KernelPdpt ||
            !BootData) {
            break;
        }

        /* First 2MiB are the early identity mapping (which contains ourselves; Required for no
           crash upon entering long mode).
           Right after the higher half barrier, we have the actual identity mapping (supporting up
           to 16TiB, mapped according to the max adressable memory).
           The kernel and the kernel drivers are mapped at last, in whichever locations the ASLR
           layer chose. */
        memset(Pml4, 0, PAGE_SIZE);
        Pml4[0] = (uint64_t)EarlyIdentPdpt | 0x03;
        Pml4[(ARENA_BASE >> 39) & 0x1FF] = (uint64_t)KernelPdpt | 0x03;

        for (uint64_t i = 0; i < Slices512GiB; i++) {
            Pml4[256 + i] = (uint64_t)(LateIdentPdpt + (i << 9)) | 0x03;
        }

        if (Edx & bit_PDPE1GB) {
            printf("mapping %llu 1GiB slices of adressable physical memory\n", Slices1GiB);
            memset(LateIdentPdpt, 0, Slices512GiB * PAGE_SIZE);
            for (uint64_t i = 0; i < Slices1GiB; i++) {
                LateIdentPdpt[i] = (i << 30) | 0x83;
            }
        } else {
            uint64_t *LateIdentPdt = BmAllocatePages(Slices1GiB, MEMORY_KERNEL);
            if (!LateIdentPdt) {
                break;
            }

            memset(LateIdentPdpt, 0, Slices512GiB * PAGE_SIZE);
            for (uint64_t i = 0; i < Slices1GiB; i++) {
                LateIdentPdpt[i] = (uint64_t)(LateIdentPdt + (i << 9)) | 0x03;
            }

            printf("mapping %llu 2MiB slices of adressable physical memory\n", Slices2MiB);
            memset(LateIdentPdt, 0, Slices1GiB * PAGE_SIZE);
            for (uint64_t i = 0; i < Slices2MiB; i++) {
                LateIdentPdt[i] = (i << 21) | 0x83;
            }
        }

        memset(EarlyIdentPdpt, 0, PAGE_SIZE);
        EarlyIdentPdpt[0] = (uint64_t)EarlyIdentPdt | 0x03;

        memset(EarlyIdentPdt, 0, PAGE_SIZE);
        EarlyIdentPdt[0] = 0x83;

        memset(KernelPdpt, 0, PAGE_SIZE);
        for (size_t i = 0; i < ImageCount; i++) {
            /* 1GiB should be a sane limit for a single image/driver. */
            uint64_t ImageSlices2MiB = (Images[i].ImageSize + 0x1FFFFF) >> 21;
            uint64_t ImageSlices4KiB = Images[i].ImageSize >> 12;

            uint64_t ImagePdptBase = (Images[i].VirtualAddress >> 30) & 0x1FF;
            uint64_t ImagePdtBase = (Images[i].VirtualAddress >> 21) & 0x1FF;
            uint64_t ImagePtBase = (Images[i].VirtualAddress >> 12) & 0x1FF;

            uint64_t *ImagePdt = BmAllocatePages(1, MEMORY_KERNEL);
            uint64_t *ImagePt = BmAllocatePages(ImageSlices2MiB, MEMORY_KERNEL);

            if (!ImagePdt || !ImagePt) {
                break;
            }

            KernelPdpt[ImagePdptBase] = (uint64_t)ImagePdt | 0x03;

            memset(ImagePdt, 0, PAGE_SIZE);
            for (uint64_t j = 0; j < ImageSlices2MiB; j++) {
                ImagePdt[ImagePdtBase + j] = (uint64_t)(ImagePt + (j << 9)) | 0x03;
            }

            printf("mapping %llu slices of 4KiB of image %zu\n", ImageSlices4KiB, i);
            memset(ImagePt, 0, ImageSlices2MiB * PAGE_SIZE);
            for (uint64_t j = 0; j < ImageSlices4KiB; j++) {
                int Flags = Images[i].PageFlags[j];
                ImagePt[ImagePtBase + j] = (uint64_t)(Images[i].PhysicalAddress + (j << 12)) |
                                           (Flags & PAGE_WRITE     ? 0x8000000000000003
                                            : !(Flags & PAGE_EXEC) ? 0x8000000000000001
                                                                   : 0x01);
            }
        }

        if (Fail) {
            break;
        }

        /* Mount the OS-specific boot data, and enter assembly land to get into long mode.  */

        memcpy(BootData->Magic, LOADER_MAGIC, 4);
        BootData->Version = LOADER_CURRENT_VERSION;
        BootData->Acpi.BaseAdress = BiosRsdtLocation;
        BootData->Acpi.IsXsdt = BiosIsXsdt;
        BootData->MemoryManager.MemorySize = BiosMemorySize;
        BootData->MemoryManager.PageAllocatorBase = (uint64_t)MmBase + 0xFFFF800000000000;
        BootData->MemoryMap.BaseAddress = (uint64_t)BiosMemoryMap + 0xFFFF800000000000;
        BootData->MemoryMap.Count = BiosMemoryMapEntries;
        BootData->Images.BaseAddress = (uint64_t)Images + 0xFFFF800000000000;
        BootData->Images.Count = ImageCount;

        BiTransferExecution(Pml4, (uint64_t)BootData + 0xFFFF800000000000, Images[0].EntryPoint);
    } while (0);

    BmPanic("An error occoured while trying to load the selected operating system.\n"
            "Please, reboot your device and try again.\n");
}
