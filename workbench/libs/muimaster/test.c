#include <stdio.h>

#include <clib/alib_protos.h>
#include <intuition/gadgetclass.h>
#include <intuition/icclass.h>
#include <gadgets/colorwheel.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/dos.h>

/* define this if this should be compiled with mui */
#ifdef COMPILE_WITH_MUI

#include <libraries/mui.h>
#include <proto/muimaster.h>

typedef unsigned long IPTR;

#else

/* the following should go in a single include file which then only
** constits of the public constants and members. Actually this is easiey
*/

#include "mui.h"

/* muimaster.library is not yet a library */
#include "muimaster_intern.h"

struct MUIMasterBase_intern MUIMasterBase_instance;

#endif

struct Library *MUIMasterBase;
struct Library *ColorWheelBase;


__saveds void repeat_function(void)
{
    printf("MUI_Timer\n");
}

/* The custom class */

struct DropText_Data
{
    ULONG times;
};

#ifndef _AROS
__saveds __asm IPTR dispatcher(register __a0 struct IClass *cl, register __a2 Object *obj, register __a1 Msg msg)
#else
AROS_UFH3S(IPTR, dispatcher,
	AROS_UFHA(Class  *, cl,  A0),
	AROS_UFHA(Object *, obj, A2),
	AROS_UFHA(Msg     , msg, A1))
#endif
{
    switch (msg->MethodID)
    {
    	case MUIM_DragQuery: return MUIV_DragQuery_Accept;
    	case MUIM_DragDrop:
    	{
	    struct DropText_Data *data = (struct DropText_Data*)INST_DATA(cl,obj);
	    char buf[100];
	    data->times++;
	    sprintf(buf,"%ld times",data->times); /* no MUIM_SetAsString yet */
	    set(obj,MUIA_Text_Contents,buf);
    	}
    }
    return DoSuperMethodA(cl,obj,(Msg)msg);
}

struct MUI_CustomClass *CL_DropText;

#define DropTextObject (Object*)NewObject(CL_DropText->mcc_Class, NULL


/* Main prog */

void main(void)
{
    Object *app;
    Object *wnd,*second_wnd;
    Object *open_button;
    Object *quit_button;
    Object *repeat_button;
    Object *wheel;

    struct Hook hook;

#ifndef COMPILE_WITH_MUI
    MUIMasterBase = (struct Library*)&MUIMasterBase_instance;

    MUIMasterBase_instance.sysbase = *((struct ExecBase **)4);
    MUIMasterBase_instance.dosbase = OpenLibrary("dos.library",37);
    MUIMasterBase_instance.utilitybase = OpenLibrary("utility.library",37);
    MUIMasterBase_instance.aslbase = OpenLibrary("asl.library",37);
    MUIMasterBase_instance.gfxbase = OpenLibrary("graphics.library",37);
    MUIMasterBase_instance.layersbase = OpenLibrary("layers.library",37);
    MUIMasterBase_instance.intuibase = OpenLibrary("intuition.library",37);
    MUIMasterBase_instance.cxbase = OpenLibrary("commodities.library",37);
    MUIMasterBase_instance.keymapbase = OpenLibrary("keymap.library",37);
    __zune_prefs_init(&__zprefs);
#else
    MUIMasterBase = (struct Library*)OpenLibrary("muimaster.library",0);
#endif

    hook.h_Entry = (HOOKFUNC)repeat_function;

    /* should check the result in a real program! */
    CL_DropText = MUI_CreateCustomClass(NULL,MUIC_Text,NULL,sizeof(struct DropText_Data), dispatcher);
    ColorWheelBase = OpenLibrary("gadgets/colorwheel.gadget",0);

    app = ApplicationObject,
    	SubWindow, wnd = WindowObject,
    	    MUIA_Window_Title, "test",
	    MUIA_Window_Activate, TRUE,

    	    WindowContents, VGroup,
    	    	Child, TextObject, TextFrame, MUIA_Text_Contents, "\33cHello World!!\nThis is a text object\n\33lLeft \33bbold\33n\n\33rRight",End,
    	    	Child, HGroup,
		    GroupFrameT("A horizontal group"),
		    Child, ColGroup(2),
			GroupFrameT("A column group"),
			Child, repeat_button = TextObject, MUIA_CycleChain, 1, ButtonFrame, MUIA_Background, MUII_ButtonBack, MUIA_Text_PreParse, "\33c", MUIA_Text_Contents, "Repeat", MUIA_InputMode, MUIV_InputMode_RelVerify, End,
			Child, TextObject, MUIA_CycleChain, 1, ButtonFrame, MUIA_Background, MUII_ButtonBack, MUIA_Text_PreParse, "\33c", MUIA_Text_Contents, "Drag Me", MUIA_Draggable, TRUE, MUIA_InputMode, MUIV_InputMode_RelVerify, End,
			Child, open_button = TextObject, MUIA_CycleChain, 1, ButtonFrame, MUIA_Background, MUII_ButtonBack, MUIA_Text_PreParse, "\33c", MUIA_Text_Contents, "Open Window", MUIA_InputMode, MUIV_InputMode_RelVerify, End,
			Child, TextObject, MUIA_CycleChain, 1, ButtonFrame, MUIA_Background, MUII_ButtonBack, MUIA_Text_PreParse, "\33c", MUIA_Text_Contents, "Button4", MUIA_InputMode, MUIV_InputMode_RelVerify, End,
			Child, TextObject, MUIA_CycleChain, 1, ButtonFrame, MUIA_Background, MUII_ButtonBack, MUIA_Text_PreParse, "\33c", MUIA_Text_Contents, "Button5", MUIA_InputMode, MUIV_InputMode_RelVerify, End,
			Child, HVSpace, //TextObject, MUIA_CycleChain, 1, ButtonFrame, MUIA_Background, MUII_ButtonBack, MUIA_Text_PreParse, "\33c", MUIA_Text_Contents, "Button6", MUIA_InputMode, MUIV_InputMode_RelVerify, End,
			End,
		    Child, VGroup,
			GroupFrameT("A vertical group"),
			Child, DropTextObject, MUIA_CycleChain, 1, ButtonFrame, MUIA_Background, MUII_ButtonBack, MUIA_Text_PreParse, "\33c", MUIA_Text_Contents, "Drop Here", MUIA_Dropable, TRUE, MUIA_InputMode, MUIV_InputMode_RelVerify, End,
			Child, TextObject, MUIA_CycleChain, 1, ButtonFrame, MUIA_Background, MUII_ButtonBack, MUIA_Text_PreParse, "\33c", MUIA_Text_Contents, "Button8", MUIA_InputMode, MUIV_InputMode_RelVerify, End,
			End,
		    End,

		Child, HGroup,
		    Child, VGroup,
			Child, RectangleObject,
			    MUIA_VertWeight,0, /* Seems to be not supported properly as orginal MUI doesn't allow to alter the height of the window */
			    MUIA_Rectangle_HBar, TRUE,
			    MUIA_Rectangle_BarTitle,"Enter a string",
			    End,

			Child, StringObject,
			    StringFrame,
			    MUIA_CycleChain,1,
			    MUIA_String_AdvanceOnCR, TRUE,
			    End,

	    	    	Child, SliderObject,
    		    	    MUIA_Group_Horiz, TRUE,
    	    		    MUIA_CycleChain,1,
			    End,
			End,

		    Child, wheel = BoopsiObject,  /* MUI and Boopsi tags mixed */
		        GroupFrame,
		        MUIA_Boopsi_ClassID  , "colorwheel.gadget",
		        MUIA_Boopsi_MinWidth , 30, /* boopsi objects don't know */
		        MUIA_Boopsi_MinHeight, 30, /* their sizes, so we help   */
		        MUIA_Boopsi_Remember , WHEEL_Saturation, /* keep important values */
		        MUIA_Boopsi_Remember , WHEEL_Hue,        /* during window resize  */
		        MUIA_Boopsi_TagScreen, WHEEL_Screen, /* this magic fills in */
		        WHEEL_Screen         , NULL,         /* the screen pointer  */
		        GA_Left     , 0,
		        GA_Top      , 0, /* MUI will automatically     */
		        GA_Width    , 0, /* fill in the correct values */
		        GA_Height   , 0,
		        ICA_TARGET  , ICTARGET_IDCMP, /* needed for notification */
		        WHEEL_Saturation, 0, /* start in the center */
		        MUIA_FillArea, TRUE, /* use this because it defaults to FALSE 
					     for boopsi gadgets but the colorwheel
					     doesnt bother about redrawing its backgorund */
		        End,
		    End,

    	    	Child, HGroup,
    	    	    Child, quit_button = TextObject,
			ButtonFrame,
			MUIA_InputMode, MUIV_InputMode_RelVerify,
			MUIA_CycleChain, 1,
			MUIA_Background, MUII_ButtonBack,
			MUIA_ControlChar, 'q',
			MUIA_Text_HiChar, 'q',
			MUIA_Text_PreParse, "\33c",
			MUIA_Text_Contents, "Quit",
			End,
    	    	    End,
    	        End,
    	    End,
    	SubWindow, second_wnd = WindowObject,
    	    MUIA_Window_Title, "Second window",

    	    WindowContents, VGroup,
		Child, DropTextObject, MUIA_Text_Contents, "Drop here", MUIA_Dropable, TRUE, End,
		End,
    	    End,
    	End;

    if (app)
    {
	ULONG sigs = 0;

    	DoMethod(wnd, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
    	DoMethod(second_wnd, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, second_wnd, 3, MUIM_Set, MUIA_Window_Open, FALSE);
    	DoMethod(open_button, MUIM_Notify, MUIA_Pressed, FALSE, second_wnd, 3,  MUIM_Set, MUIA_Window_Open, TRUE);
    	DoMethod(quit_button, MUIM_Notify, MUIA_Pressed, FALSE, app, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
    	DoMethod(repeat_button, MUIM_Notify, MUIA_Timer, MUIV_EveryTime, app, 2, MUIM_CallHook, &hook);

	set(wnd,MUIA_Window_Open,TRUE);

	while((LONG) DoMethod(app, MUIM_Application_NewInput, &sigs) != MUIV_Application_ReturnID_Quit)
	{
	    if (sigs)
	    {
		sigs = Wait(sigs | SIGBREAKF_CTRL_C | SIGBREAKF_CTRL_D);
		if (sigs & SIGBREAKF_CTRL_C) break;
		if (sigs & SIGBREAKF_CTRL_D) break;
	    }
	}

	MUI_DisposeObject(app);
    }
    CloseLibrary(ColorWheelBase);
    MUI_DeleteCustomClass(CL_DropText);

#ifdef COMPILE_WITH_MUI
    CloseLibrary(MUIMasterBase);
#endif
}

// ---- old test -------

#if 0
__asm __saveds void hook_function(register __a1 int *pval)
{
    printf("get notification of the userdata: %ld\n",*pval);
}

void main(void)
{
    struct Hook hook;
    Object *notify;

    MUIMasterBase = (struct Library*)&MUIMasterBase_instance;

    MUIMasterBase_instance.sysbase = *((struct ExecBase **)4);
    MUIMasterBase_instance.utilitybase = OpenLibrary("utility.library",37);

    printf("Creating object\n");
    notify = MUI_NewObjectA(MUIC_Notify, NULL);

    if (notify)
    {
	LONG ud;
	printf("Object created at 0x%lx. Now setting MUIA_UserData to 10\n",notify);
	set(notify, MUIA_UserData, 10);
	get(notify, MUIA_UserData, &ud);
	printf("UserData is now %ld\n",ud);

	printf("Create a notify for the MUIA_UserData tag\n");
	hook.h_Entry = (HOOKFUNC)hook_function;
	DoMethod(notify, MUIM_Notify, MUIA_UserData, MUIV_EveryTime, notify, 3, MUIM_CallHook, &hook, MUIV_TriggerValue);
	printf("Setting User Data to 20\n");
	set(notify, MUIA_UserData, 20);
	printf("Done. Now disposing object\n");

	MUI_DisposeObject(notify);
    }
}

#endif
