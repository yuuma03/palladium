/* SPDX-FileCopyrightText: (C) 2023-2025 ilmmatias
 * SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _HALP_H_
#define _HALP_H_

#include <generic/context.h>
#include <hal.h>
#include <ki.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern uint32_t HalpProcessorCount;
extern KeProcessor **HalpProcessorList;

void HalpInitializeBootStack(KiLoaderBlock *LoaderBlock);
void HalpInitializeBootProcessor(void);
void HalpInitializeApplicationProcessor(KeProcessor *Processor);
void HalpStopProcessor(void);
void HalpPauseProcessor(void);

uint64_t HalpGetPhysicalAddress(void *VirtualAddress);
int HalpMapPage(void *VirtualAddress, uint64_t PhysicalAddress, int Flags);
void HalpUnmapPage(void *VirtualAddress);

void HalpNotifyProcessor(KeProcessor *Processor, int WaitDelivery);
void HalpFreezeProcessor(KeProcessor *Processor);

void *HalpEnterCriticalSection(void);
void HalpLeaveCriticalSection(void *Context);

KeIrql HalpGetIrql(void);
void HalpSetIrql(KeIrql NewIrql);

void HalpInitializeContext(
    HalContextFrame *Context,
    char *Stack,
    uint64_t StackSize,
    void (*EntryPoint)(void *),
    void *Parameter);
void HalpSwitchContext(HalContextFrame *CurrentThread, HalContextFrame *TargetThread);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _HALP_H_ */
