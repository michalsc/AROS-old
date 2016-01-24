/*
    Copyright � 2015-2016, The AROS Development	Team. All rights reserved.
    $Id$
*/

#ifndef DEBUG
#   define DEBUG 0
#endif
#include <aros/debug.h>

#include <clib/alib_protos.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/realtime.h>
#include <proto/layers.h>
#include <proto/datatypes.h>

#include <intuition/gadgetclass.h>
#include <libraries/realtime.h>
#include <gadgets/tapedeck.h>

#include "animationclass.h"

AROS_UFH3(ULONG, playerHookFunc, 
    AROS_UFHA(struct    Hook *,     hook, A0), 
    AROS_UFHA(struct Player *, obj, A2), 
    AROS_UFHA(struct pmTime *, msg, A1))
{ 
    AROS_USERFUNC_INIT
 
    struct Animation_Data *animd = (struct Animation_Data *)hook->h_Data;
    BOOL doTick = FALSE;
    IPTR buffSigs = 0;

#if (0)
    D(bug("[animation.datatype]: %s(%08x)\n", __PRETTY_FUNCTION__, msg->pmt_Method));
#endif

    switch (msg->pmt_Method)
    {
	case PM_TICK:
            animd->ad_TimerData.atd_Tick++;

            if (animd->ad_TimerData.atd_Tick >= animd->ad_TimerData.atd_TicksPerFrame)
            {
                animd->ad_TimerData.atd_Tick = 0;
                animd->ad_FrameData.afd_FrameCurrent++;
                if (animd->ad_FrameData.afd_FrameCurrent >= animd->ad_FrameData.afd_Frames)
                    animd->ad_FrameData.afd_FrameCurrent = 0;
                doTick = TRUE;
            }
            if (animd->ad_TimerData.atd_Tick == 0)
            {
                if (animd->ad_ProcessData->pp_BufferPurge != -1)
                    buffSigs |= (1 << animd->ad_ProcessData->pp_BufferPurge);
            }

	    break;

	case PM_SHUTTLE:
            bug("[animation.datatype] %s: PM_SHUTTLE\n", __PRETTY_FUNCTION__);
            doTick = TRUE;
	    break;

	case PM_STATE:
            doTick = TRUE;
	    break;
    }

    if (buffSigs && (animd->ad_BufferProc))
        Signal((struct Task *)animd->ad_BufferProc, buffSigs);
    if (doTick && (animd->ad_PlayerProc) && (animd->ad_ProcessData->pp_PlaybackTick != -1))
        Signal((struct Task *)animd->ad_PlayerProc, (1 << animd->ad_ProcessData->pp_PlaybackTick));

    return 0;

    AROS_USERFUNC_EXIT
}
    
void FreePlaybackSignals(struct ProcessPrivate *priv)
{
    D(bug("[animation.datatype/PLAY]: %s()\n", __PRETTY_FUNCTION__));

    if (priv->pp_PlaybackTick != -1)
        FreeSignal(priv->pp_PlaybackTick);
    if (priv->pp_PlaybackEnable != -1)
        FreeSignal(priv->pp_PlaybackEnable);
    if (priv->pp_PlaybackDisable != -1)
        FreeSignal(priv->pp_PlaybackDisable);
}

BOOL AllocPlaybackSignals(struct ProcessPrivate *priv)
{
    if ((priv->pp_PlaybackEnable = AllocSignal(-1)) != -1)
    {
        D(bug("[animation.datatype/PLAY]: %s: allocated enable signal (%x)\n", __PRETTY_FUNCTION__, priv->pp_PlaybackEnable));
        if ((priv->pp_PlaybackDisable = AllocSignal(-1)) != -1)
        {
            D(bug("[animation.datatype/PLAY]: %s: allocated disable signal (%x)\n", __PRETTY_FUNCTION__, priv->pp_PlaybackDisable));
            if ((priv->pp_PlaybackTick = AllocSignal(-1)) != -1)
            {
                D(bug("[animation.datatype/PLAY]: %s: allocated tick signal (%x)\n", __PRETTY_FUNCTION__, priv->pp_PlaybackTick));

                priv->pp_PlaybackSigMask = (1 << priv->pp_PlaybackEnable) | (1 << priv->pp_PlaybackDisable) | (1 << priv->pp_PlaybackTick);

                D(bug("[animation.datatype/PLAY]: %s: signal mask (%x)\n", __PRETTY_FUNCTION__, priv->pp_PlaybackSigMask));

                return TRUE;
            }
        }
    }
    return FALSE;
}

struct AnimFrame *NextFrame(struct ProcessPrivate *priv, struct AnimFrame *frameCurrent, UWORD *frame)
{
    struct AnimFrame *frameFound = NULL;

    DFRAMES("[animation.datatype/PLAY]: %s(0x%p, %d)\n", __PRETTY_FUNCTION__, frameCurrent, *frame)

    ObtainSemaphoreShared(&priv->pp_Data->ad_FrameData.afd_AnimFramesLock);

    if ((!frameCurrent) ||
        (*frame < NODEID(frameCurrent)))
        frameFound = (struct AnimFrame *)&priv->pp_Data->ad_FrameData.afd_AnimFrames;
    else
        frameFound = frameCurrent;

    while ((frameFound = (struct AnimFrame *)GetSucc(&frameFound->af_Node)) != NULL)
    {
        DFRAMES("[animation.datatype/PLAY] %s:   frame #%d @ 0x%p\n", __PRETTY_FUNCTION__, NODEID(frameFound), frameFound)
        if (NODEID(frameFound) >= *frame)
        {
            break;
        }
    }

    if ((frameFound) && (frameCurrent != frameFound) &&
        (NODEID(frameFound) <= *frame))
    {
        priv->pp_Data->ad_FrameData.afd_FrameCurrent = NODEID(frameFound);
    }
    else
    {
        if ((frameCurrent) && (NODEID(frameCurrent) < (priv->pp_Data->ad_FrameData.afd_Frames - 1)))
        {
            frameFound = frameCurrent;
            priv->pp_Data->ad_FrameData.afd_FrameCurrent = NODEID(frameCurrent);
        }
        else
        {
            frameFound = NULL;
            priv->pp_Data->ad_FrameData.afd_FrameCurrent = 0;
        }
    }

    *frame = priv->pp_Data->ad_FrameData.afd_FrameCurrent;

    DFRAMES("[animation.datatype/PLAY] %s: found 0x%p\n", __PRETTY_FUNCTION__, frameFound)

    ReleaseSemaphore(&priv->pp_Data->ad_FrameData.afd_AnimFramesLock);

    return frameFound;
}

AROS_UFH3(void, playerProc,
        AROS_UFHA(STRPTR,              argPtr, A0),
        AROS_UFHA(ULONG,               argSize, D0),
        AROS_UFHA(struct ExecBase *,   SysBase, A6))
{
    AROS_USERFUNC_INIT

    struct ProcessPrivate *priv = FindTask(NULL)->tc_UserData;
    struct AnimFrame *curFrame = NULL, *prevFrame = NULL;
    struct gpRender gprMsg;
    struct TagItem attrtags[] =
    {
        { TAG_IGNORE,   0},
        { TAG_IGNORE,   0},
        { TAG_DONE,     0}
    };
    UWORD frame = 0;
    ULONG signal;

    D(bug("[animation.datatype/PLAY]: %s()\n", __PRETTY_FUNCTION__));

    if (priv)
    {
        D(bug("[animation.datatype/PLAY] %s: private data @ 0x%p\n", __PRETTY_FUNCTION__, priv));
        D(bug("[animation.datatype/PLAY] %s: dt obj @ 0x%p, instance data @ 0x%p\n", __PRETTY_FUNCTION__, priv->pp_Object, priv->pp_Data));

        priv->pp_PlaybackFrame = NULL;
        priv->pp_PlayerFlags |= PRIVPROCF_RUNNING;

        if (AllocPlaybackSignals(priv))
        {
            D(bug("[animation.datatype/PLAY]: %s: entering main loop ...\n", __PRETTY_FUNCTION__));
            while (TRUE)
            {
                priv->pp_PlayerFlags &= ~PRIVPROCF_ACTIVE;

                signal = priv->pp_PlaybackSigMask | SIGBREAKF_CTRL_C;
                signal = Wait(signal);

                D(bug("[animation.datatype/PLAY]: %s: signalled (%08x)\n", __PRETTY_FUNCTION__, signal));

                if (signal & SIGBREAKF_CTRL_C)
                    break;

                if (signal & (1 << priv->pp_PlaybackEnable))
                    priv->pp_PlayerFlags |= PRIVPROCF_ENABLED;
                else if (signal & (1 << priv->pp_PlaybackDisable))
                    priv->pp_PlayerFlags &= ~PRIVPROCF_ENABLED;

                if ((priv->pp_PlayerFlags & PRIVPROCF_ENABLED) && (signal & (1 << priv->pp_PlaybackTick)))
                {
                    struct privRenderBuffer *rendFrame = (struct privRenderBuffer *)&gprMsg;
                    rendFrame->MethodID = PRIVATE_RENDERBUFFER;

                    frame = priv->pp_Data->ad_FrameData.afd_FrameCurrent;
                    D(bug("[animation.datatype/PLAY]: %s: TICK (frame %d)\n", __PRETTY_FUNCTION__, frame));

                    priv->pp_PlayerFlags |= PRIVPROCF_ACTIVE;

                    curFrame = NextFrame(priv, priv->pp_PlaybackFrame, &frame);

                    if (!(curFrame) || (priv->pp_PlaybackFrame == curFrame))
                    {
                        if ((priv->pp_Data->ad_BufferProc) && (priv->pp_BufferFill != -1))
                        {
                            ObtainSemaphore(&priv->pp_Data->ad_FrameData.afd_AnimFramesLock);
                            if ((curFrame) && (NODEID(curFrame) < (priv->pp_Data->ad_FrameData.afd_Frames - 1)))
                                priv->pp_BufferFirst = curFrame;
                            else
                                priv->pp_BufferFirst = NULL;
                            ReleaseSemaphore(&priv->pp_Data->ad_FrameData.afd_AnimFramesLock);

                            Signal((struct Task *)priv->pp_Data->ad_BufferProc, (1 << priv->pp_BufferFill));
                            SetTaskPri((struct Task *)priv->pp_Data->ad_PlayerProc, -2);
                        }
                        continue;
                    }

                    // frame has changed ... render it ..
                    D(bug("[animation.datatype/PLAY]: %s: Rendering Frame #%d\n", __PRETTY_FUNCTION__, frame));

                    rendFrame->Source = curFrame->af_Frame.alf_BitMap;
                    D(bug("[animation.datatype/PLAY]: %s: #%d BitMap @ 0x%p\n", __PRETTY_FUNCTION__, NODEID(curFrame), curFrame->af_Frame.alf_BitMap));
                    if (curFrame->af_Frame.alf_CMap)
                    {
                        D(bug("[animation.datatype/PLAY]: %s: Frame CMap @ 0x%p\n", __PRETTY_FUNCTION__, curFrame, curFrame->af_Frame.alf_CMap));
                        DoMethod(priv->pp_Object, PRIVATE_FREEPENS);
                        GetRGB32(curFrame->af_Frame.alf_CMap, 0UL,
                            (curFrame->af_Frame.alf_CMap->Count < priv->pp_Data->ad_ColorData.acd_NumColors) ? curFrame->af_Frame.alf_CMap->Count : priv->pp_Data->ad_ColorData.acd_NumColors,
                            priv->pp_Data->ad_ColorData.acd_CRegs);
                    }

                    DoMethodA(priv->pp_Object, (Msg)&gprMsg);

                    if ((priv->pp_Data->ad_Window) && !(priv->pp_Data->ad_Flags & ANIMDF_LAYOUT))
                    {
                        if (priv->pp_Data->ad_Tapedeck)
                        {
                            // update the tapedeck gadget..
                            attrtags[0].ti_Tag = TDECK_CurrentFrame;
                            attrtags[0].ti_Data = frame;
                            attrtags[1].ti_Tag = TAG_IGNORE;

                            SetAttrsA((Object *)priv->pp_Data->ad_Tapedeck, attrtags);
                        }
                        D(bug("[animation.datatype/PLAY]: %s: Asking DTObj to render..\n", __PRETTY_FUNCTION__));
                        // tell the top level gadget to redraw...
                        gprMsg.MethodID   = GM_RENDER;
                        gprMsg.gpr_RPort  = priv->pp_Data->ad_Window->RPort;
                        gprMsg.gpr_GInfo  = NULL;
                        gprMsg.gpr_Redraw = GREDRAW_UPDATE;
                        DoGadgetMethodA((struct Gadget *)priv->pp_Object, priv->pp_Data->ad_Window, NULL, (Msg)&gprMsg);
                    }
                    priv->pp_PlaybackFrame = curFrame;
                }
            }
            FreePlaybackSignals(priv);
        }
        priv->pp_PlayerFlags &= ~PRIVPROCF_RUNNING;
        priv->pp_Data->ad_PlayerProc = NULL;
    }

    D(bug("[animation.datatype/PLAY]: %s: exiting ...\n", __PRETTY_FUNCTION__));

    return;

    AROS_USERFUNC_EXIT
}
