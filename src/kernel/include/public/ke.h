/* SPDX-FileCopyrightText: (C) 2023-2025 ilmmatias
 * SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _KE_H_
#define _KE_H_

#ifdef ARCH_amd64
#include <amd64/processor.h>
#else
#error "Undefined ARCH for the kernel module!"
#endif /* ARCH_amd64 */

#include <rt/list.h>

#define KE_EVENT_NONE 0
#define KE_EVENT_FREEZE 1

#define KE_PANIC_MANUALLY_INITIATED_CRASH 0
#define KE_PANIC_IRQL_NOT_LESS_OR_EQUAL 1
#define KE_PANIC_IRQL_NOT_GREATER_OR_EQUAL 2
#define KE_PANIC_IRQL_NOT_DISPATCH 3
#define KE_PANIC_SPIN_LOCK_ALREADY_OWNED 4
#define KE_PANIC_SPIN_LOCK_NOT_OWNED 5
#define KE_PANIC_TRAP_NOT_HANDLED 6
#define KE_PANIC_EXCEPTION_NOT_HANDLED 7
#define KE_PANIC_PAGE_FAULT_NOT_HANDLED 8
#define KE_PANIC_NMI_HARDWARE_FAILURE 9
#define KE_PANIC_KERNEL_INITIALIZATION_FAILURE 10
#define KE_PANIC_DRIVER_INITIALIZATION_FAILURE 11
#define KE_PANIC_BAD_PFN_HEADER 12
#define KE_PANIC_BAD_POOL_HEADER 13
#define KE_PANIC_COUNT 14

#define KE_PANIC_PARAMETER_OUT_OF_RESOURCES 0x0000000000000000

#define KE_PANIC_PARAMETER_PFN_INITIALIZATION_FAILURE 0x0000000000000000
#define KE_PANIC_PARAMETER_POOL_INITIALIZATION_FAILURE 0x0000000000000001
#define KE_PANIC_PARAMETER_SCHEDULER_INITIALIZATION_FAILURE 0x0000000000000002
#define KE_PANIC_PARAMETER_ACPI_INITIALIZATION_FAILURE 0x0000000000000003
#define KE_PANIC_PARAMETER_DRIVER_INITIALIZATION_FAILURE 0x0000000000000004
#ifdef ARCH_amd64
#define KE_PANIC_PARAMETER_APIC_INITIALIZATION_FAILURE 0x0000000000000005
#define KE_PANIC_PARAMETER_IOAPIC_INITIALIZATION_FAILURE 0x0000000000000006
#define KE_PANIC_PARAMETER_HPET_INITIALIZATION_FAILURE 0x0000000000000007
#define KE_PANIC_PARAMETER_SMP_INITIALIZATION_FAILURE 0x0000000000000008
#endif

#define KE_PANIC_PARAMETER_BAD_RSDT_TABLE 0x0000000000000000
#define KE_PANIC_PARAMETER_BAD_APIC_TABLE 0x0000000000000001
#define KE_PANIC_PARAMETER_BAD_HPET_TABLE 0x0000000000000002
#define KE_PANIC_PARAMETER_BAD_DSDT_TABLE 0x0000000000000003
#define KE_PANIC_PARAMETER_TABLE_NOT_FOUND 0x0000000000000001
#define KE_PANIC_PARAMETER_INVALID_TABLE_CHECKSUM 0x0000000000000002

#if defined(ARCH_amd64)
#define KE_IRQL_PASSIVE 0
#define KE_IRQL_DISPATCH 2
#define KE_IRQL_DEVICE 3
#define KE_IRQL_MASK 15

#define KE_STACK_SIZE 0x2000
#else
#error "Undefined ARCH for the kernel module!"
#endif /* ARCH */

typedef uint64_t KeSpinLock;
typedef uint64_t KeIrql;

typedef struct {
    RtDList ListHeader;
    void *ImageBase;
    void *EntryPoint;
    uint32_t SizeOfImage;
    const char *ImageName;
} KeModule;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern RtDList KeModuleListHead;

void *KiFindAcpiTable(const char Signature[4], int Index);

KeIrql KeGetIrql(void);
KeIrql KeRaiseIrql(KeIrql NewIrql);
void KeLowerIrql(KeIrql NewIrql);

int KeTryAcquireSpinLockHighIrql(KeSpinLock *Lock);
KeIrql KeAcquireSpinLock(KeSpinLock *Lock);
void KeAcquireSpinLockHighIrql(KeSpinLock *Lock);
void KeReleaseSpinLock(KeSpinLock *Lock, KeIrql NewIrql);
void KeReleaseSpinLockHighIrql(KeSpinLock *Lock);
int KeTestSpinLock(KeSpinLock *Lock);

[[noreturn]] void KeFatalError(
    uint32_t Message,
    uint64_t Parameter1,
    uint64_t Parameter2,
    uint64_t Parameter3,
    uint64_t Parameter4);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _KE_H_ */
