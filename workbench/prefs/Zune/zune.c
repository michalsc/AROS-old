/*
    Copyright � 2002, The AROS Development Team. 
    All rights reserved.
    
    $Id$
*/

#include <libraries/asl.h>
#include <libraries/mui.h>

#include <clib/alib_protos.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/utility.h>

#ifdef _AROS
#include <proto/muimaster.h>
#endif

#include "zunestuff.h"

struct Library *MUIMasterBase;

#ifndef _AROS

/* On AmigaOS we build a fake library base, because it's not compiled as sharedlibrary yet */
#include "muimaster_intern.h"

int open_muimaster(void)
{
    static struct MUIMasterBase_intern MUIMasterBase_instance;
    MUIMasterBase = (struct Library*)&MUIMasterBase_instance;

    MUIMasterBase_instance.sysbase = *((struct ExecBase **)4);
    MUIMasterBase_instance.dosbase = (void*)OpenLibrary("dos.library",37);
    MUIMasterBase_instance.utilitybase = (void*)OpenLibrary("utility.library",37);
    MUIMasterBase_instance.aslbase = OpenLibrary("asl.library",37);
    MUIMasterBase_instance.gfxbase = (void*)OpenLibrary("graphics.library",37);
    MUIMasterBase_instance.layersbase = OpenLibrary("layers.library",37);
    MUIMasterBase_instance.intuibase = (void*)OpenLibrary("intuition.library",37);
    MUIMasterBase_instance.cxbase = OpenLibrary("commodities.library",37);
    MUIMasterBase_instance.keymapbase = OpenLibrary("keymap.library",37);
    MUIMasterBase_instance.gadtoolsbase = OpenLibrary("gadtools.library",37);
    __zune_prefs_init(&__zprefs);
    InitSemaphore(&MUIMB(MUIMasterBase)->ZuneSemaphore);
    return 1;
}

void close_muimaster(void)
{
}

#else

int open_muimaster(void)
{
    if ((MUIMasterBase = OpenLibrary("muimaster.library", 0))) return 1;
    return 0;
}

void close_muimaster(void)
{
    if (MUIMasterBase) CloseLibrary(MUIMasterBase);
}

#endif



/****************************************************************
 Open needed libraries
*****************************************************************/
int open_libs(void)
{
    if (open_muimaster())
    {
	return 1;
    }
    
    return 0;
}

/****************************************************************
 Close opened libraries
*****************************************************************/
void close_libs(void)
{
    close_muimaster();
}

struct Hook hook_standard;

static Object *app;
static Object *open_menuitem;
static Object *append_menuitem;
static Object *saveas_menuitem;
static Object *aboutzune_menuitem;
static Object *quit_menuitem;

static Object *main_wnd;
static Object *main_page_list;
static Object *main_page_group; /* contains the selelected group */
static Object *main_page_group_displayed; /* The current displayed group */
static Object *main_page_space; /* a space object */


struct page_entry
{
    char *name;
    Object *group;  /* The group which should be is displayed, maybe NULL */

    int (*init)(struct page_entry *); /* this function initializes the group, maybe NULL */
};

int init_window_page(struct page_entry *page)
{
    page->group = HGroup,
    	Child, ColGroup(2),
	    GroupFrameT("Fonts"),
	    Child, MakeLabel("Normal"),
	    Child, PopaslObject,
		MUIA_Popasl_Type, ASL_FontRequest,
		MUIA_Popstring_String, StringObject, StringFrame, End,
		MUIA_Popstring_Button, PopButton(MUII_PopUp),
		End,

	    Child, MakeLabel("Small"),
	    Child, PopaslObject,
		MUIA_Popasl_Type, ASL_FontRequest,
		MUIA_Popstring_String, StringObject, StringFrame, End,
		MUIA_Popstring_Button, PopButton(MUII_PopUp),
		End,

	    Child, MakeLabel("Big"),
	    Child, PopaslObject,
		MUIA_Popasl_Type, ASL_FontRequest,
		MUIA_Popstring_String, StringObject, StringFrame, End,
		MUIA_Popstring_Button, PopButton(MUII_PopUp),
		End,

	    End,
	End;


    return !!page->group;
}

struct page_entry main_page_entries[] =
{
    {"Info",NULL,NULL},
    {"System",NULL,NULL},
    {"Windows",NULL,init_window_page},
};

/****************************************************************
 Our standard hook function, for easy call backs
*****************************************************************/
#ifndef __AROS__
__saveds static __asm void hook_func_standard(register __a0 struct Hook *h, register __a1 ULONG * funcptr)
#else
AROS_UFH2( void, hook_func_standard,
    AROS_UFHA( struct Hook *, h,       A0 ),
    AROS_UFHA( ULONG *,       funcptr, A1 ))
#endif
{
    void (*func) (ULONG *) = (void (*)(ULONG *)) (*funcptr);
    if (func) func(funcptr + 1);
}

/****************************************************************
 The display function for the page listview
*****************************************************************/
#ifndef __AROS__
__saveds __asm void main_page_list_display(register __a0 struct Hook *h, register __a2 char **strings, register __a1 struct page_entry *entry)
#else
AROS_UFH3( void, main_page_list_display,
    AROS_UFHA( struct Hook *,       h,       A0 ),
    AROS_UFHA( char **,             strings, A2 ),
    AROS_UFHA( struct page_entry *, entry,   A1 )) 
#endif
{
    if (entry)
    {
        strings[0] = entry->name;
    }
}


/****************************************************************
 A new entry has been selected
*****************************************************************/
void main_page_active(void)
{
    int new_active = xget(main_page_list,MUIA_List_Active);
    Object *new_group;

    if (new_active == -1) new_group = main_page_space;
    else
    {
	new_group = main_page_entries[new_active].group;
	if (!new_group) new_group = main_page_space;
    }

    if (new_group == main_page_group_displayed) return;

    DoMethod(main_page_group,MUIM_Group_InitChange);
    DoMethod(main_page_group,OM_REMMEMBER,main_page_group_displayed);
    DoMethod(main_page_group,OM_ADDMEMBER,new_group);
    DoMethod(main_page_group,MUIM_Group_ExitChange);
    main_page_group_displayed = new_group;
}

/****************************************************************
 Allocalte resources for gui
*****************************************************************/
int init_gui(void)
{
    Object *save_button;
    Object *use_button;
    Object *test_button;
    Object *cancel_button;

    static struct Hook page_display_hook;

    hook_standard.h_Entry = (HOOKFUNC)hook_func_standard;
    page_display_hook.h_Entry = (HOOKFUNC)main_page_list_display;

    app = ApplicationObject,
	MUIA_Application_Menustrip, MenuitemObject,
	    MUIA_Family_Child, MenuitemObject,
	    	MUIA_Menuitem_Title, "Project",
	    	MUIA_Family_Child, open_menuitem = MenuitemObject, MUIA_Menuitem_Title, "Open...", MUIA_Menuitem_Shortcut, "O", End,
	    	MUIA_Family_Child, append_menuitem = MenuitemObject, MUIA_Menuitem_Title, "Append...", End,
	    	MUIA_Family_Child, saveas_menuitem = MenuitemObject, MUIA_Menuitem_Title, "Save As...", MUIA_Menuitem_Shortcut, "A", End,
	    	MUIA_Family_Child, MenuitemObject, MUIA_Menuitem_Title, ~0, End,
	    	MUIA_Family_Child, aboutzune_menuitem = MenuitemObject, MUIA_Menuitem_Title, "About Zune...", MUIA_Menuitem_Shortcut, "?", End,
	    	MUIA_Family_Child, MenuitemObject, MUIA_Menuitem_Title, ~0, End,
	    	MUIA_Family_Child, quit_menuitem = MenuitemObject, MUIA_Menuitem_Title, "Quit", MUIA_Menuitem_Shortcut, "Q", End,
	    	End,
	    End,
    	SubWindow, main_wnd = WindowObject,
    	    MUIA_Window_Title, "Zune - Preferences",
	    MUIA_Window_Activate, TRUE,

	    WindowContents, VGroup,
    	    	Child, HGroup,
		    Child, ListviewObject,
			MUIA_Listview_List, main_page_list = ListObject,
			    InputListFrame,
			    MUIA_List_DisplayHook, &page_display_hook,
			    End,
			End,
		    Child, main_page_group = VGroup,
			Child, main_page_group_displayed = main_page_space = HVSpace,
		        End,
		    End,
    	    	Child, HGroup,
		    Child, save_button = MakeButton("Save"),
		    Child, use_button = MakeButton("Use"),
    	    	    Child, test_button = MakeButton("Test"),
    	    	    Child, cancel_button = MakeButton("Cancel"),
    	    	    End,
		End,
    	    End,
    	End;

    if (app)
    {
    	int i;

	DoMethod(main_wnd, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
	DoMethod(cancel_button, MUIM_Notify, MUIA_Pressed, FALSE, app, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
	DoMethod(quit_menuitem, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, app, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

	for (i=0;i<(sizeof(main_page_entries)/sizeof(main_page_entries[0]));i++)
	{
	    struct page_entry *p = &main_page_entries[i];
	    if (p->init) p->init(p);
	    DoMethod(main_page_list,MUIM_List_InsertSingle,p,MUIV_List_Insert_Bottom);
	}

	DoMethod(main_page_list, MUIM_Notify, MUIA_List_Active, MUIV_EveryTime, app, 3, MUIM_CallHook, &hook_standard, main_page_active);

	/* Activate first entry */
	set(main_page_list,MUIA_List_Active,0);

	return 1;
    }
    return 0;
}

/****************************************************************
 Deallocates all gui resources
*****************************************************************/
void deinit_gui(void)
{
    if (app) MUI_DisposeObject(app);
}

/****************************************************************
 The message loop
*****************************************************************/
void loop(void)
{
    ULONG sigs = 0;

    while((LONG) DoMethod(app, MUIM_Application_NewInput, &sigs) != MUIV_Application_ReturnID_Quit)
    {
	if (sigs)
	{
	    sigs = Wait(sigs | SIGBREAKF_CTRL_C | SIGBREAKF_CTRL_D);
	    if (sigs & SIGBREAKF_CTRL_C) break;
	    if (sigs & SIGBREAKF_CTRL_D) break;
	}
    }
}

/****************************************************************
 The main entry point
*****************************************************************/
void main(void)
{
    if (open_libs())
    {
    	if (init_gui())
    	{
    	    set(main_wnd, MUIA_Window_Open, TRUE);
    	    if (xget(main_wnd,MUIA_Window_Open))
	    {
		loop();
	    }
	    deinit_gui();
    	}
	close_libs();
    }
}
