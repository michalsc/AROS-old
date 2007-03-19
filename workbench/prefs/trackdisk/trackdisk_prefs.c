#ifdef __AROS__
#include <aros/asmcall.h>
#endif
#include <prefs/trackdisk.h>
#include <clib/alib_protos.h>
#include <devices/trackdisk.h>
#include <dos/dosextens.h>
#include <intuition/imageclass.h>
#include <libraries/iffparse.h>
#include <libraries/mui.h>
#include <utility/tagitem.h>
#include <workbench/startup.h>

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>

#include <stdio.h>

#ifndef __AROS__
#include "prefs_common.h"
#endif
#include "trackdisk_prefs.h"

#ifdef __MORPHOS__
ULONG __abox__ = 1;
#endif

#ifdef __AROS__
struct Library *aroscbase;
#endif
struct ExecBase *SysBase;
struct DosLibrary *DOSBase;
struct IntuitionBase *IntuitionBase;
struct Library *MUIMasterBase;
Object *App, *MainWin, *SaveButton, *UseButton, *CancelButton;
struct DriveControls Drives[TD_NUMUNITS];
struct WindowGroup MainGrp;
struct TrackdiskPrefs TDPrefs;
struct IORequest TDIO;

#ifdef __AROS__
AROS_UFH3(ULONG, Start,
	  AROS_UFHA(char *, argstr, A0),
	  AROS_UFHA(LONG, arglen, D0),
	  AROS_UFHA(struct ExecBase *, sBase, A6))
{
	AROS_USERFUNC_INIT
#else
#define sBase *(struct ExecBase **)4L

ULONG Start()
{
#endif
	struct Process *me;
	struct Message *wbmsg;
	ULONG ret;

	SysBase = sBase;
	me = (struct Process *)FindTask(NULL);
	if (me->pr_CLI)
		return Main();
	else {
		WaitPort(&me->pr_MsgPort);
		wbmsg = GetMsg(&me->pr_MsgPort);
		ret = Main();
		Forbid();
		ReplyMsg(wbmsg);
		return ret;
	}
#ifdef __AROS__
	AROS_USERFUNC_EXIT
#endif
}

ULONG Main(void)
{
    ULONG signals;
    int i;
    ULONG retval = 0;

#ifdef __AROS__
    aroscbase = OpenLibrary("arosc.library", 39);
    if (aroscbase) {
#endif
	MUIMasterBase = OpenLibrary("muimaster.library", 4);
	if (MUIMasterBase) {
		IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 36);
		if (IntuitionBase) {
		    DOSBase = (struct DosLibrary *)OpenLibrary("dos.library", 36);
		    if (DOSBase) {
			for (i = 0; i < TD_NUMUNITS; i++)
				InitUnitPrefs(&TDPrefs.UnitPrefs[i], i);
			LoadPrefs();
			for (i = 0; i < TD_NUMUNITS; i++) {
				MainGrp.DriveGroup[i].ti_Tag = MUIA_Group_Child;
				MainGrp.DriveGroup[i].ti_Data = (ULONG)CreateDriveControls(&Drives[i], i);
			}
			MainGrp.TagChild = MUIA_Group_Child;
			MainGrp.ButtonsGroup = MUI_NewObject("Group.mui", MUIA_Group_Horiz, TRUE,
				MUIA_Group_Child,
				SaveButton = MUI_NewObject("Text.mui", MUIA_InputMode, MUIV_InputMode_RelVerify,
					MUIA_CycleChain, TRUE,
					MUIA_Text_Contents, "Save",
					MUIA_Text_PreParse, "\33c",
					MUIA_Background, MUII_ButtonBack,
					MUIA_Frame, MUIV_Frame_Button,
				TAG_DONE),
				MUIA_Group_Child,
				UseButton = MUI_NewObject("Text.mui", MUIA_InputMode, MUIV_InputMode_RelVerify,
					MUIA_CycleChain, TRUE,
					MUIA_Text_Contents, "Use",
					MUIA_Text_PreParse, "\33c",
					MUIA_Background, MUII_ButtonBack,
					MUIA_Frame, MUIV_Frame_Button,
				TAG_DONE),
				MUIA_Group_Child,
				CancelButton = MUI_NewObject("Text.mui", MUIA_InputMode, MUIV_InputMode_RelVerify,
					MUIA_CycleChain, TRUE,
					MUIA_Text_Contents, "Cancel",
					MUIA_Text_PreParse, "\33c",
					MUIA_Background, MUII_ButtonBack,
					MUIA_Frame, MUIV_Frame_Button,
				TAG_DONE),
			TAG_DONE);
			MainGrp.TagDone = TAG_DONE;
			App = MUI_NewObject("Application.mui", MUIA_Application_Author, "Pavel Fedin",
				MUIA_Application_Base, (ULONG)"TRACKDISKPREFS",
				MUIA_Application_Copyright, (ULONG)"(c) 2006 Pavel Fedin",
				MUIA_Application_Description, (ULONG)"trackdisk.device preferences editor",
				MUIA_Application_SingleTask, TRUE,
				MUIA_Application_Title, (ULONG)"Trackdisk prefs",
				MUIA_Application_Version, (ULONG)"$VER: trackdisk prefs 1.0 (15.07.2006)",
				MUIA_Application_Window,
				MainWin = MUI_NewObject("Window.mui", MUIA_Window_ID, MAKE_ID('M', 'A', 'I', 'N'),
					MUIA_Window_Title, (ULONG)"trackdisk.device preferences",
					MUIA_Window_RootObject,	MUI_NewObjectA("Group.mui", (struct TagItem *)&MainGrp),
				TAG_DONE),
			TAG_DONE);
			if (App) {
				DoMethod(SaveButton, MUIM_Notify, MUIA_Pressed, FALSE, App, 2, MUIM_Application_ReturnID, 1);
				DoMethod(UseButton, MUIM_Notify, MUIA_Pressed, FALSE, App, 2, MUIM_Application_ReturnID, 2);
				DoMethod(CancelButton, MUIM_Notify, MUIA_Pressed, FALSE, App, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
				SetAttrs(MainWin, MUIA_Window_Open, TRUE);
				while (retval != MUIV_Application_ReturnID_Quit) {
					retval = DoMethod(App, MUIM_Application_NewInput, &signals);
					if (retval && (retval != MUIV_Application_ReturnID_Quit)) {
						for (i = 0; i < TD_NUMUNITS; i++)
							ControlsToPrefs(&Drives[i], &TDPrefs.UnitPrefs[i]);
						if (retval == 1)
							SavePrefs();
						UsePrefs();
						retval = MUIV_Application_ReturnID_Quit;
					}
					if (signals)
						signals = Wait(signals);
				}
				MUI_DisposeObject(App);
			}
			CloseLibrary((struct Library *)DOSBase);
		    }
		    CloseLibrary((struct Library *)IntuitionBase);
		}
		CloseLibrary(MUIMasterBase);
	}
#ifdef __AROS__
	CloseLibrary(aroscbase);
    }
#endif
    return 0;
}

void InitUnitPrefs(struct TDU_Prefs *UnitPrefs, int nunit)
{
	UnitPrefs->TagUnitNum	= TDPR_UnitNum;
	UnitPrefs->Unit		= nunit;
	UnitPrefs->TagPubFlags	= TDPR_PubFlags;
	UnitPrefs->PubFlags	= 0;
	UnitPrefs->TagRetryCnt	= TDPR_RetryCnt;
	UnitPrefs->RetryCnt	= 3;
}

Object *CreateDriveControls(struct DriveControls *dc, int ndrive)
{

	sprintf(dc->DriveLabel, "Drive %u", ndrive);
	return MUI_NewObject("Group.mui", MUIA_Group_Horiz, TRUE,
		MUIA_Disabled, dc->Disabled,
		MUIA_FrameTitle, dc->DriveLabel,
		MUIA_Background, MUII_GroupBack,
		MUIA_Frame, MUIV_Frame_Group,
		MUIA_Group_Child, MUI_MakeObject(MUIO_Label, "No click:", 0),
		MUIA_Group_Child,
		dc->NoClickSwitch = MUI_NewObject("Image.mui", MUIA_Image_Spec, MUII_CheckMark,
			MUIA_InputMode, MUIV_InputMode_Toggle,
			MUIA_Selected, TDPrefs.UnitPrefs[ndrive].PubFlags & TDPF_NOCLICK,
			MUIA_CycleChain, TRUE,
/* FIXME: in original MUI checkmarks are frameless. They include a frame themselves if they need to */
#ifdef __AROS__
			MUIA_Frame, MUIV_Frame_ImageButton,
			MUIA_ShowSelState, FALSE,
#endif
		TAG_DONE),
		MUIA_Group_Child, MUI_MakeObject(MUIO_Label, "Retries:", 0),
		MUIA_Group_Child,
		dc->RetriesSlider = MUI_NewObject("Slider.mui", MUIA_CycleChain, TRUE,
			MUIA_Numeric_Min, 1, MUIA_Numeric_Max, 10,
			MUIA_Numeric_Value, TDPrefs.UnitPrefs[ndrive].RetryCnt,
		TAG_DONE),
	TAG_DONE);
}

void LoadPrefs(void)
{
	ULONG i;
	struct TDU_PublicUnit *tdu;

	for (i = 0; i < TD_NUMUNITS; i++) {
		if (OpenDevice("trackdisk.device", i, &TDIO, 0))
			Drives[i].Disabled = TRUE;
		else {
			tdu = (struct TDU_PublicUnit *)TDIO.io_Unit;
			TDPrefs.UnitPrefs[i].PubFlags = tdu->tdu_PubFlags;
			TDPrefs.UnitPrefs[i].RetryCnt = tdu->tdu_RetryCnt;
			CloseDevice(&TDIO);
			Drives[i].Disabled = FALSE;
		}
	}
}

void ControlsToPrefs(struct DriveControls *dc, struct TDU_Prefs *pr)
{
	ULONG NoClick;

	GetAttr(MUIA_Selected, dc->NoClickSwitch, &NoClick);
	pr->PubFlags = NoClick ? TDPF_NOCLICK : 0 ;
	GetAttr(MUIA_Numeric_Value, dc->RetriesSlider, &pr->RetryCnt);

}

void SavePrefs(void)
{
	BPTR cf;

	cf = Open(TRACKDISK_PREFS_NAME, MODE_NEWFILE);
	if (cf) {
		TDPrefs.TagDone = TAG_DONE;
		Write(cf, &TDPrefs, sizeof(TDPrefs));
		Close(cf);
	}
}

void UsePrefs(void)
{
	ULONG i;
	struct TDU_PublicUnit *tdu;

	for (i = 0; i < TD_NUMUNITS; i++) {
		if ((!Drives[i].Disabled) && (!OpenDevice("trackdisk.device", i, &TDIO, 0))) {
			tdu = (struct TDU_PublicUnit *)TDIO.io_Unit;
			tdu->tdu_PubFlags = TDPrefs.UnitPrefs[i].PubFlags;
			tdu->tdu_RetryCnt = TDPrefs.UnitPrefs[i].RetryCnt;
			CloseDevice(&TDIO);
		}
	}
}
