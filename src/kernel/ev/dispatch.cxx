/* SPDX-FileCopyrightText: (C) 2023-2025 ilmmatias
 * SPDX-License-Identifier: GPL-3.0-or-later */

#include <halp.h>
#include <psp.h>

#include <cxx/critical_section.hxx>
#include <cxx/lock.hxx>

/*-------------------------------------------------------------------------------------------------
 * PURPOSE:
 *     This function dispatches the given object into the event queue.
 *     Do not use this function unless you're implementing a custom event, use EvWaitObject(s)
 *     instead.
 *
 * PARAMETERS:
 *     Object - Pointer to the event object.
 *     Timeout - How many nanoseconds we should wait until we drop the event; Set this to 0 to
 *               wait indefinitely.
 *     Yield - Set this to 1 if we should yield the thread (and only reinsert it after the event
 *             finishes).
 *
 * RETURN VALUE:
 *     None.
 *-----------------------------------------------------------------------------------------------*/
extern "C" void EvpDispatchObject(void *Object, uint64_t Timeout, int Yield) {
    KeProcessor *Processor = HalGetCurrentProcessor();
    uint64_t CurrentTicks = HalGetTimerTicks();
    EvHeader *Header = (EvHeader *)Object;

    /* Enter critical section (can't let any scheduling happen here), and update the event
       queue. */
    CriticalSection Guard;
    if (Header->Finished) {
        return;
    }

    /* Ignore the target timeout for already dispatched objects (we probably just want to yield
       this thread out). */
    if (!Header->Dispatched) {
        Header->DeadlineTicks = 0;
        if (Timeout) {
            Header->DeadlineReference = CurrentTicks;
            Header->DeadlineTicks = Timeout / HalGetTimerPeriod();
        }
    }

    if (!Header->Dispatched) {
        RtAppendDList(&HalGetCurrentProcessor()->EventQueue, &Header->ListHeader);
        Header->Dispatched = 1;
    }

    if (Yield) {
        Processor->ForceYield = PSP_YIELD_EVENT;
        Header->Source = Processor->CurrentThread;
    }

    if (Yield) {
        HalpNotifyProcessor(Processor, 1);
    }
}

/*-------------------------------------------------------------------------------------------------
 * PURPOSE:
 *     This function schedules out the current thread while it awaits an event to complete.
 *
 * PARAMETERS:
 *     Object - Pointer to the event object.
 *     Timeout - How many nanoseconds we should wait until we drop the event; Set this to 0 to
 *               wait indefinitely.
 *
 * RETURN VALUE:
 *     None.
 *-----------------------------------------------------------------------------------------------*/
extern "C" void EvWaitObject(void *Object, uint64_t Timeout) {
    EvpDispatchObject(Object, Timeout, 1);
}

/*-------------------------------------------------------------------------------------------------
 * PURPOSE:
 *     This function cancels a previously dispatched object. This will PsReadyThread() if
 *     required.
 *
 * PARAMETERS:
 *     Object - Pointer to the event object.
 *
 * RETURN VALUE:
 *     None.
 *-----------------------------------------------------------------------------------------------*/
extern "C" void EvCancelObject(void *Object) {
    CriticalSection Guard;
    EvHeader *Header = (EvHeader *)Object;

    if (!Header->Dispatched) {
        return;
    }

    /* No DPC dispatch happens on cancel. */
    RtUnlinkDList(&Header->ListHeader);

    if (Header->Source) {
        PsReadyThread(Header->Source);
    }
}

/*-------------------------------------------------------------------------------------------------
 * PURPOSE:
 *     This function handles any pending kernel events.
 *
 * PARAMETERS:
 *     Context - Current processor state.
 *
 * RETURN VALUE:
 *     None.
 *-----------------------------------------------------------------------------------------------*/
extern "C" void EvpHandleEvents(HalRegisterState *Context) {
    KeProcessor *Processor = HalGetCurrentProcessor();
    uint64_t CurrentTicks = HalGetTimerTicks();

    /* Process any pending events (they might enqueue DPCs, so we need to process them
     * first). */
    RtDList *ListHeader = Processor->EventQueue.Next;
    while (ListHeader != &Processor->EventQueue) {
        EvHeader *Header = CONTAINING_RECORD(ListHeader, EvHeader, ListHeader);
        ListHeader = ListHeader->Next;

        /* Out of the deadline, for anything but timers, this will make WaitObject return an
           error. */
        if (Header->DeadlineTicks &&
            HalCheckTimerExpiration(
                CurrentTicks, Header->DeadlineReference, Header->DeadlineTicks)) {
            Header->Finished = 1;
        }

        if (Header->Finished) {
            Header->Dispatched = 0;
            RtUnlinkDList(&Header->ListHeader);

            if (Header->Source) {
                /* Boost the priority of the waiting task, and insert it back. */
                SpinLockGuard Guard(&Processor->ThreadQueueLock);
                __atomic_add_fetch(&Processor->ThreadQueueSize, 1, __ATOMIC_SEQ_CST);
                RtPushDList(&Processor->ThreadQueue, &Header->Source->ListHeader);
            }

            if (Header->Dpc) {
                RtAppendDList(&Processor->DpcQueue, &Header->Dpc->ListHeader);
            }
        }
    }

    /* Process any pending DPCs. */
    while (Processor->DpcQueue.Next != &Processor->DpcQueue) {
        EvDpc *Dpc = CONTAINING_RECORD(RtPopDList(&Processor->DpcQueue), EvDpc, ListHeader);
        Dpc->Routine(Dpc->Context);
    }

    /* Wrap up by calling the scheduler; It should handle initialization+retriggering the event
       handler if required. */
    PspScheduleNext(Context);
}
