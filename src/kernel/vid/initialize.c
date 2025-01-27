/* SPDX-FileCopyrightText: (C) 2023-2025 ilmmatias
 * SPDX-License-Identifier: GPL-3.0-or-later */

#include <mi.h>
#include <vidp.h>

extern char *VidpBackBuffer;
extern char *VidpFrontBuffer;
extern uint16_t VidpWidth;
extern uint16_t VidpHeight;
extern uint16_t VidpPitch;
extern int VidpUseLock;

/*-------------------------------------------------------------------------------------------------
 * PURPOSE:
 *     This function saves all display related data from the boot block, and resets the display (to
 *     remove any data still visible from osloader).
 *
 * PARAMETERS:
 *     LoaderBlock - Data prepared by the boot loader for us.
 *
 * RETURN VALUE:
 *     None.
 *-----------------------------------------------------------------------------------------------*/
void VidpInitialize(KiLoaderBlock *LoaderBlock) {
    uint64_t FrameBufferSize = VidpHeight * VidpPitch * 4;
    VidpBackBuffer = MiEnsureEarlySpace((uint64_t)LoaderBlock->BackBuffer, FrameBufferSize);
    VidpFrontBuffer = MiEnsureEarlySpace((uint64_t)LoaderBlock->FrontBuffer, FrameBufferSize);
    VidpWidth = LoaderBlock->FramebufferWidth;
    VidpHeight = LoaderBlock->FramebufferHeight;
    VidpPitch = LoaderBlock->FramebufferPitch;
    VidResetDisplay();
}

/*-------------------------------------------------------------------------------------------------
 * PURPOSE:
 *     This function disables the KeAcquireSpinLock()s inside the Vid* functions.
 *
 * PARAMETERS:
 *     None.
 *
 * RETURN VALUE:
 *     None.
 *-----------------------------------------------------------------------------------------------*/
void VidpAcquireOwnership(void) {
    VidpUseLock = 0;
}
