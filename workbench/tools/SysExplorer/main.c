/*
    Copyright (C) 2013, The AROS Development Team.
    $Id: main.c 41537 2011-09-22 08:33:28Z sonic $
*/

#include <exec/memory.h>
#include <hidd/hidd.h>
#include <libraries/asl.h>
#include <mui/NListtree_mcc.h>
#include <mui/NListview_mcc.h>
#include <utility/tagitem.h>
#include <utility/hooks.h>

#include <proto/alib.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/muimaster.h>
#include <proto/oop.h>
#include <proto/utility.h>
#include <proto/intuition.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "locale.h"

#define DEBUG 1
#include <aros/debug.h>

#define APPNAME "SysExplorer"
#define VERSION "SysExplorer 0.1"

const char version[] = "$VER: " VERSION " (" ADATE ")\n";

OOP_AttrBase HiddAttrBase;
OOP_AttrBase HWAttrBase;

const struct OOP_ABDescr abd[] =
{
    {IID_Hidd, &HiddAttrBase},
    {IID_HW  , &HWAttrBase  },
    {NULL    , NULL         }
};

Object *app;
Object *MainWindow;
Object *objTree;
Object *menu_quit;
Object *instance_name, *hardware_description, *vendor_info;

AROS_UFP3(static void, enumFunc,
    AROS_UFPA(struct Hook *, h,  A0),
    AROS_UFPA(OOP_Object*, obj, A2),
    AROS_UFPA(void *, parent, A1));

AROS_UFP3(static void, selectedFunc,
    AROS_UFPA(struct Hook *, h,  A0),
    AROS_UFPA(Object*, obj, A2),
    AROS_UFPA(struct MUI_NListtree_TreeNode **, tn, A1));

static struct Hook enum_hook =
{
    .h_Entry = enumFunc,
};

static struct Hook selected_hook =
{
    .h_Entry = selectedFunc,
};

AROS_UFH3(static void, enumFunc,
    AROS_UFHA(struct Hook *, h,  A0),
    AROS_UFHA(OOP_Object*, obj, A2),
    AROS_UFHA(void *, parent, A1))
{
    AROS_USERFUNC_INIT

    CONST_STRPTR name = NULL;
    
    OOP_GetAttr(obj, aHW_ClassName, (IPTR *)&name);

    if (name)
    {
        // node
        struct MUI_NListtree_TreeNode *tn;

        tn = (APTR)DoMethod(objTree, MUIM_NListtree_Insert, name, obj,
                            parent, MUIV_NListtree_Insert_PrevNode_Tail,
                            TNF_LIST|TNF_OPEN);
        if (tn)
        {
            HW_EnumDrivers(obj, &enum_hook, tn);
        }
    }
    else
    {
        // leave
        OOP_GetAttr(obj, aHidd_HardwareName, (IPTR *)&name);
        DoMethod(objTree, MUIM_NListtree_Insert, name, obj,
                 parent, MUIV_NListtree_Insert_PrevNode_Tail, 0);
    }

    AROS_USERFUNC_EXIT
}

AROS_UFH3(static void, selectedFunc,
    AROS_UFHA(struct Hook *, h,  A0),
    AROS_UFHA(Object*, obj, A2),
    AROS_UFHA(struct MUI_NListtree_TreeNode **, tn, A1))
{
    AROS_USERFUNC_INIT

    D(bug("selectedFunc called tn: %p name %s\n", *tn, (*tn)->tn_Name));

    if ((*tn)->tn_Flags & TNF_LIST)
    {
        // node
        SET(instance_name, MUIA_Text_Contents, "");
        SET(hardware_description, MUIA_Text_Contents, "");
        SET(vendor_info, MUIA_Text_Contents, "");
    }
    else
    {
        // leave
        SET(instance_name, MUIA_Text_Contents, (*tn)->tn_Name);
        SET(hardware_description, MUIA_Text_Contents, "FOO");
        SET(vendor_info, MUIA_Text_Contents, "BAR");
    }

    AROS_USERFUNC_EXIT
}

static BOOL GUIinit()
{
    BOOL retval = FALSE;

    app = ApplicationObject,
        MUIA_Application_Title,         (IPTR)APPNAME,
        MUIA_Application_Version,       (IPTR)VERSION,
        MUIA_Application_Copyright,     (IPTR)"(C) 2013, The AROS Development Team",
        MUIA_Application_Author,        (IPTR)"Pavel Fedin",
        MUIA_Application_Base,          (IPTR)APPNAME,
        MUIA_Application_Description,   __(MSG_DESCRIPTION),

        MUIA_Application_Menustrip, MenuitemObject,
            MUIA_Family_Child, MenuitemObject,
                MUIA_Menuitem_Title, __(MSG_MENU_PROJECT),
                MUIA_Family_Child,
                    menu_quit = MenuitemObject, MUIA_Menuitem_Title, __(MSG_MENU_QUIT),
                End,
            End,
        End,

        SubWindow, MainWindow = WindowObject,
            MUIA_Window_Title,	__(MSG_WINTITLE),
            MUIA_Window_ID, MAKE_ID('S', 'Y', 'E', 'X'),
            WindowContents, HGroup,
                Child, HGroup,
                    GroupFrame,
                    MUIA_FrameTitle, "HIDD Tree",
                    Child, NListviewObject,
                        MUIA_NListview_NList, objTree = NListtreeObject,
                            ReadListFrame,
                            MUIA_CycleChain, TRUE,
                        End,
                    End,
                End,
                Child, VGroup,
                    Child, ColGroup(2),
                        MUIA_FrameTitle, "Properties",
                        GroupFrame,
                        Child, Label("Driver instance name"),
                        Child, instance_name = TextObject,
                            MUIA_Text_Contents, "          ",
                            TextFrame,
                        End,
                        Child, Label("Hardware description"),
                        Child, hardware_description = TextObject,
                            MUIA_Text_Contents, "          ",
                            TextFrame,
                        End,
                        Child, Label("Vendor information"),
                        Child, vendor_info = TextObject,
                            MUIA_Text_Contents, "          ",
                            TextFrame,
                        End,
                    End,
                    Child, HVSpace,
                End,
            End, // WindowContents
        End, // MainWindow
    End; // ApplicationObject

    if (app)
    {
        /* Quit application if the windowclosegadget or the esc key is pressed. */
        DoMethod(MainWindow, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, 
                 app, 2, 
                 MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

        DoMethod(menu_quit, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
                 app, 2,
                 MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

        DoMethod(objTree, MUIM_Notify, MUIA_NListtree_Active, MUIV_EveryTime,
                 app, 3,
                 MUIM_CallHook, &selected_hook, MUIV_TriggerValue);

        retval = TRUE;
    }

    return retval;
}

int __nocommandline = 1;

int main(void)
{
    if (!Locale_Initialize())
        return 20;

    if (!OOP_ObtainAttrBases(abd))
    {
        Locale_Deinitialize();
        return 20;
    }
    
    if (GUIinit())
    {
        OOP_Object *hwRoot = OOP_NewObject(NULL, CLID_HW_Root, NULL);

        if (hwRoot)
        {
            /* This will kick our recursive enumeration into action */
            CALLHOOKPKT(&enum_hook, hwRoot, MUIV_NListtree_Insert_ListNode_Root);
        }

        SET(MainWindow, MUIA_Window_Open, TRUE);

        if (XGET(MainWindow, MUIA_Window_Open))
        {
            DoMethod(app, MUIM_Application_Execute);
            SET(MainWindow, MUIA_Window_Open, FALSE);
        }

        DisposeObject(app);
    }

    OOP_ReleaseAttrBases(abd);
    Locale_Deinitialize();
    return 0;
}
