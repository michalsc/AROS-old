#ifndef LOCALE_H
#define LOCALE_H


/****************************************************************************/


/* This file was created automatically by CatComp.
 * Do NOT edit by hand!
 */


#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifdef CATCOMP_ARRAY
#ifndef CATCOMP_NUMBERS
#define CATCOMP_NUMBERS
#endif
#ifndef CATCOMP_STRINGS
#define CATCOMP_STRINGS
#endif
#endif

#ifdef CATCOMP_BLOCK
#ifndef CATCOMP_STRINGS
#define CATCOMP_STRINGS
#endif
#endif


/****************************************************************************/


#ifdef CATCOMP_NUMBERS

#define MSG_UNIT_LBL 0
#define MSG_DEVICE_LBL 1
#define MSG_FILENAME_LBL 2
#define MSG_WRITEPROTECT_LBL 3
#define MSG_NO_DISK 4
#define MSG_APPDESCRIPTION 5
#define MSG_PRIORITY_LBL 50
#define MSG_PLUGIN_LBL 51
#define MSG_PLUGIN_GAD 52
#define MSG_AUTO_LBL 53
#define MSG_WRITESUPPORT_LBL 54
#define MSG_PROJECT_ABOUT 100
#define MSG_PROJECT_HIDE 101
#define MSG_PROJECT_ICONIFY 102
#define MSG_PROJECT_SNAPSHOT 103
#define MSG_PROJECT_QUIT 104
#define MSG_SETTINGS_MENU 105
#define MSG_SETTINGS_CHANGETEMPDIR 106
#define MSG_SETTINGS_SAVE 107
#define MSG_SETTINGS_PLUGINS 108
#define MSG_PROJECT_CLOSE 109
#define MSG_OK_GAD 200
#define MSG_ABOUT_REQ 201
#define MSG_ERROR 202
#define MSG_NOAISS_REQ 203
#define MSG_OLDAISS_REQ 204
#define MSG_IMAGENOTFOUND_REQ 205
#define MSG_CANCEL_GAD 206
#define MSG_SAVE_GAD 207
#define MSG_USE_GAD 208
#define MSG_INSERT_GAD 300
#define MSG_EJECT_GAD 301
#define MSG_WRITEPROTECT_GAD 302
#define MSG_REFRESH_GAD 303
#define MSG_PLUGINS_WND 304
#define MSG_MAIN_WND 305
#define MSG_ABOUT_WND 306
#define MSG_ERROR_WND 307
#define MSG_SETDEVICETYPE_GAD 308
#define MSG_SETDEVICETYPE_WND 309
#define MSG_DEVICETYPE_GAD 310
#define MSG_DEVICETYPE_DIRECT_ACCESS 311
#define MSG_DEVICETYPE_CDROM 312
#define MSG_DEVICETYPE_OPTICAL_DISK 313
#define MSG_REMOVABLE_GAD 314
#define MSG_REQUIRED_REQ 315
#define MSG_REQUIREDVERSION_REQ 316

#endif /* CATCOMP_NUMBERS */


/****************************************************************************/


#ifdef CATCOMP_STRINGS

#define MSG_UNIT_LBL_STR "Unit"
#define MSG_DEVICE_LBL_STR "Device"
#define MSG_FILENAME_LBL_STR "Disk Image"
#define MSG_WRITEPROTECT_LBL_STR "WP"
#define MSG_NO_DISK_STR "No disk present"
#define MSG_APPDESCRIPTION_STR "Control GUI for diskimage.device"
#define MSG_PRIORITY_LBL_STR "Pri"
#define MSG_PLUGIN_LBL_STR "Plugin"
#define MSG_PLUGIN_GAD_STR "Use plugin"
#define MSG_AUTO_LBL_STR "(Auto)"
#define MSG_WRITESUPPORT_LBL_STR "W"
#define MSG_PROJECT_ABOUT_STR "About..."
#define MSG_PROJECT_HIDE_STR "Hide"
#define MSG_PROJECT_ICONIFY_STR "Iconify"
#define MSG_PROJECT_SNAPSHOT_STR "Snapshot"
#define MSG_PROJECT_QUIT_STR "Quit"
#define MSG_SETTINGS_MENU_STR "Settings"
#define MSG_SETTINGS_CHANGETEMPDIR_STR "Change temp directory..."
#define MSG_SETTINGS_SAVE_STR "Save settings"
#define MSG_SETTINGS_PLUGINS_STR "Plugins..."
#define MSG_PROJECT_CLOSE_STR "Close"
#define MSG_OK_GAD_STR "_Ok"
#define MSG_ABOUT_REQ_STR "%s version %ld.%ld\n%s version %ld.%ld\n\nCopyright � 2007-2012 Fredrik Wikstrom\nWebsite: http://a500.org\nE-mail: fredrik@a500.org"
#define MSG_ERROR_STR "Error"
#define MSG_NOAISS_REQ_STR "AISS assign not found.\n\nDownload and install the latest version\nfrom here: http://masonicons.de"
#define MSG_OLDAISS_REQ_STR "AISS v%ld.%ld or newer required.\n\nDownload and install the latest version\nfrom here: http://masonicons.de"
#define MSG_IMAGENOTFOUND_REQ_STR "Image file '%s' not found."
#define MSG_CANCEL_GAD_STR "_Cancel"
#define MSG_SAVE_GAD_STR "_Save"
#define MSG_USE_GAD_STR "_Use"
#define MSG_INSERT_GAD_STR "Insert"
#define MSG_EJECT_GAD_STR "Eject"
#define MSG_WRITEPROTECT_GAD_STR "Write Protect"
#define MSG_REFRESH_GAD_STR "Refresh"
#define MSG_PLUGINS_WND_STR "Plugins"
#define MSG_MAIN_WND_STR "%s: Hot Key = <%s>"
#define MSG_ABOUT_WND_STR "About - %s"
#define MSG_ERROR_WND_STR "Error - %s"
#define MSG_SETDEVICETYPE_GAD_STR "Set Device Type"
#define MSG_SETDEVICETYPE_WND_STR "Unit: %ld - Device Type"
#define MSG_DEVICETYPE_GAD_STR "Device Type"
#define MSG_DEVICETYPE_DIRECT_ACCESS_STR "Direct Access (Default setting)"
#define MSG_DEVICETYPE_CDROM_STR "CD-ROM"
#define MSG_DEVICETYPE_OPTICAL_DISK_STR "Optical Disk"
#define MSG_REMOVABLE_GAD_STR "_Removable"
#define MSG_REQUIRED_REQ_STR "%s required"
#define MSG_REQUIREDVERSION_REQ_STR "%s v%ld or newer required"

#endif /* CATCOMP_STRINGS */


/****************************************************************************/


#ifdef CATCOMP_ARRAY

struct CatCompArrayType
{
    LONG         cca_ID;
    CONST_STRPTR cca_Str;
};

STATIC CONST struct CatCompArrayType CatCompArray[] =
{
    {MSG_UNIT_LBL,(CONST_STRPTR)MSG_UNIT_LBL_STR},
    {MSG_DEVICE_LBL,(CONST_STRPTR)MSG_DEVICE_LBL_STR},
    {MSG_FILENAME_LBL,(CONST_STRPTR)MSG_FILENAME_LBL_STR},
    {MSG_WRITEPROTECT_LBL,(CONST_STRPTR)MSG_WRITEPROTECT_LBL_STR},
    {MSG_NO_DISK,(CONST_STRPTR)MSG_NO_DISK_STR},
    {MSG_APPDESCRIPTION,(CONST_STRPTR)MSG_APPDESCRIPTION_STR},
    {MSG_PRIORITY_LBL,(CONST_STRPTR)MSG_PRIORITY_LBL_STR},
    {MSG_PLUGIN_LBL,(CONST_STRPTR)MSG_PLUGIN_LBL_STR},
    {MSG_PLUGIN_GAD,(CONST_STRPTR)MSG_PLUGIN_GAD_STR},
    {MSG_AUTO_LBL,(CONST_STRPTR)MSG_AUTO_LBL_STR},
    {MSG_WRITESUPPORT_LBL,(CONST_STRPTR)MSG_WRITESUPPORT_LBL_STR},
    {MSG_PROJECT_ABOUT,(CONST_STRPTR)MSG_PROJECT_ABOUT_STR},
    {MSG_PROJECT_HIDE,(CONST_STRPTR)MSG_PROJECT_HIDE_STR},
    {MSG_PROJECT_ICONIFY,(CONST_STRPTR)MSG_PROJECT_ICONIFY_STR},
    {MSG_PROJECT_SNAPSHOT,(CONST_STRPTR)MSG_PROJECT_SNAPSHOT_STR},
    {MSG_PROJECT_QUIT,(CONST_STRPTR)MSG_PROJECT_QUIT_STR},
    {MSG_SETTINGS_MENU,(CONST_STRPTR)MSG_SETTINGS_MENU_STR},
    {MSG_SETTINGS_CHANGETEMPDIR,(CONST_STRPTR)MSG_SETTINGS_CHANGETEMPDIR_STR},
    {MSG_SETTINGS_SAVE,(CONST_STRPTR)MSG_SETTINGS_SAVE_STR},
    {MSG_SETTINGS_PLUGINS,(CONST_STRPTR)MSG_SETTINGS_PLUGINS_STR},
    {MSG_PROJECT_CLOSE,(CONST_STRPTR)MSG_PROJECT_CLOSE_STR},
    {MSG_OK_GAD,(CONST_STRPTR)MSG_OK_GAD_STR},
    {MSG_ABOUT_REQ,(CONST_STRPTR)MSG_ABOUT_REQ_STR},
    {MSG_ERROR,(CONST_STRPTR)MSG_ERROR_STR},
    {MSG_NOAISS_REQ,(CONST_STRPTR)MSG_NOAISS_REQ_STR},
    {MSG_OLDAISS_REQ,(CONST_STRPTR)MSG_OLDAISS_REQ_STR},
    {MSG_IMAGENOTFOUND_REQ,(CONST_STRPTR)MSG_IMAGENOTFOUND_REQ_STR},
    {MSG_CANCEL_GAD,(CONST_STRPTR)MSG_CANCEL_GAD_STR},
    {MSG_SAVE_GAD,(CONST_STRPTR)MSG_SAVE_GAD_STR},
    {MSG_USE_GAD,(CONST_STRPTR)MSG_USE_GAD_STR},
    {MSG_INSERT_GAD,(CONST_STRPTR)MSG_INSERT_GAD_STR},
    {MSG_EJECT_GAD,(CONST_STRPTR)MSG_EJECT_GAD_STR},
    {MSG_WRITEPROTECT_GAD,(CONST_STRPTR)MSG_WRITEPROTECT_GAD_STR},
    {MSG_REFRESH_GAD,(CONST_STRPTR)MSG_REFRESH_GAD_STR},
    {MSG_PLUGINS_WND,(CONST_STRPTR)MSG_PLUGINS_WND_STR},
    {MSG_MAIN_WND,(CONST_STRPTR)MSG_MAIN_WND_STR},
    {MSG_ABOUT_WND,(CONST_STRPTR)MSG_ABOUT_WND_STR},
    {MSG_ERROR_WND,(CONST_STRPTR)MSG_ERROR_WND_STR},
    {MSG_SETDEVICETYPE_GAD,(CONST_STRPTR)MSG_SETDEVICETYPE_GAD_STR},
    {MSG_SETDEVICETYPE_WND,(CONST_STRPTR)MSG_SETDEVICETYPE_WND_STR},
    {MSG_DEVICETYPE_GAD,(CONST_STRPTR)MSG_DEVICETYPE_GAD_STR},
    {MSG_DEVICETYPE_DIRECT_ACCESS,(CONST_STRPTR)MSG_DEVICETYPE_DIRECT_ACCESS_STR},
    {MSG_DEVICETYPE_CDROM,(CONST_STRPTR)MSG_DEVICETYPE_CDROM_STR},
    {MSG_DEVICETYPE_OPTICAL_DISK,(CONST_STRPTR)MSG_DEVICETYPE_OPTICAL_DISK_STR},
    {MSG_REMOVABLE_GAD,(CONST_STRPTR)MSG_REMOVABLE_GAD_STR},
    {MSG_REQUIRED_REQ,(CONST_STRPTR)MSG_REQUIRED_REQ_STR},
    {MSG_REQUIREDVERSION_REQ,(CONST_STRPTR)MSG_REQUIREDVERSION_REQ_STR},
};

#endif /* CATCOMP_ARRAY */


/****************************************************************************/


#ifdef CATCOMP_BLOCK

STATIC CONST UBYTE CatCompBlock[] =
{
    "\x00\x00\x00\x00\x00\x06"
    MSG_UNIT_LBL_STR "\x00\x00"
    "\x00\x00\x00\x01\x00\x08"
    MSG_DEVICE_LBL_STR "\x00\x00"
    "\x00\x00\x00\x02\x00\x0C"
    MSG_FILENAME_LBL_STR "\x00\x00"
    "\x00\x00\x00\x03\x00\x04"
    MSG_WRITEPROTECT_LBL_STR "\x00\x00"
    "\x00\x00\x00\x04\x00\x10"
    MSG_NO_DISK_STR "\x00"
    "\x00\x00\x00\x05\x00\x22"
    MSG_APPDESCRIPTION_STR "\x00\x00"
    "\x00\x00\x00\x32\x00\x04"
    MSG_PRIORITY_LBL_STR "\x00"
    "\x00\x00\x00\x33\x00\x08"
    MSG_PLUGIN_LBL_STR "\x00\x00"
    "\x00\x00\x00\x34\x00\x0C"
    MSG_PLUGIN_GAD_STR "\x00\x00"
    "\x00\x00\x00\x35\x00\x08"
    MSG_AUTO_LBL_STR "\x00\x00"
    "\x00\x00\x00\x36\x00\x02"
    MSG_WRITESUPPORT_LBL_STR "\x00"
    "\x00\x00\x00\x64\x00\x0A"
    MSG_PROJECT_ABOUT_STR "\x00\x00"
    "\x00\x00\x00\x65\x00\x06"
    MSG_PROJECT_HIDE_STR "\x00\x00"
    "\x00\x00\x00\x66\x00\x08"
    MSG_PROJECT_ICONIFY_STR "\x00"
    "\x00\x00\x00\x67\x00\x0A"
    MSG_PROJECT_SNAPSHOT_STR "\x00\x00"
    "\x00\x00\x00\x68\x00\x06"
    MSG_PROJECT_QUIT_STR "\x00\x00"
    "\x00\x00\x00\x69\x00\x0A"
    MSG_SETTINGS_MENU_STR "\x00\x00"
    "\x00\x00\x00\x6A\x00\x1A"
    MSG_SETTINGS_CHANGETEMPDIR_STR "\x00\x00"
    "\x00\x00\x00\x6B\x00\x0E"
    MSG_SETTINGS_SAVE_STR "\x00"
    "\x00\x00\x00\x6C\x00\x0C"
    MSG_SETTINGS_PLUGINS_STR "\x00\x00"
    "\x00\x00\x00\x6D\x00\x06"
    MSG_PROJECT_CLOSE_STR "\x00"
    "\x00\x00\x00\xC8\x00\x04"
    MSG_OK_GAD_STR "\x00"
    "\x00\x00\x00\xC9\x00\x80"
    MSG_ABOUT_REQ_STR "\x00"
    "\x00\x00\x00\xCA\x00\x06"
    MSG_ERROR_STR "\x00"
    "\x00\x00\x00\xCB\x00\x60"
    MSG_NOAISS_REQ_STR "\x00"
    "\x00\x00\x00\xCC\x00\x6A"
    MSG_OLDAISS_REQ_STR "\x00"
    "\x00\x00\x00\xCD\x00\x1C"
    MSG_IMAGENOTFOUND_REQ_STR "\x00\x00"
    "\x00\x00\x00\xCE\x00\x08"
    MSG_CANCEL_GAD_STR "\x00"
    "\x00\x00\x00\xCF\x00\x06"
    MSG_SAVE_GAD_STR "\x00"
    "\x00\x00\x00\xD0\x00\x06"
    MSG_USE_GAD_STR "\x00\x00"
    "\x00\x00\x01\x2C\x00\x08"
    MSG_INSERT_GAD_STR "\x00\x00"
    "\x00\x00\x01\x2D\x00\x06"
    MSG_EJECT_GAD_STR "\x00"
    "\x00\x00\x01\x2E\x00\x0E"
    MSG_WRITEPROTECT_GAD_STR "\x00"
    "\x00\x00\x01\x2F\x00\x08"
    MSG_REFRESH_GAD_STR "\x00"
    "\x00\x00\x01\x30\x00\x08"
    MSG_PLUGINS_WND_STR "\x00"
    "\x00\x00\x01\x31\x00\x14"
    MSG_MAIN_WND_STR "\x00\x00"
    "\x00\x00\x01\x32\x00\x0C"
    MSG_ABOUT_WND_STR "\x00\x00"
    "\x00\x00\x01\x33\x00\x0C"
    MSG_ERROR_WND_STR "\x00\x00"
    "\x00\x00\x01\x34\x00\x10"
    MSG_SETDEVICETYPE_GAD_STR "\x00"
    "\x00\x00\x01\x35\x00\x18"
    MSG_SETDEVICETYPE_WND_STR "\x00"
    "\x00\x00\x01\x36\x00\x0C"
    MSG_DEVICETYPE_GAD_STR "\x00"
    "\x00\x00\x01\x37\x00\x20"
    MSG_DEVICETYPE_DIRECT_ACCESS_STR "\x00"
    "\x00\x00\x01\x38\x00\x08"
    MSG_DEVICETYPE_CDROM_STR "\x00\x00"
    "\x00\x00\x01\x39\x00\x0E"
    MSG_DEVICETYPE_OPTICAL_DISK_STR "\x00\x00"
    "\x00\x00\x01\x3A\x00\x0C"
    MSG_REMOVABLE_GAD_STR "\x00\x00"
    "\x00\x00\x01\x3B\x00\x0C"
    MSG_REQUIRED_REQ_STR "\x00"
    "\x00\x00\x01\x3C\x00\x1A"
    MSG_REQUIREDVERSION_REQ_STR "\x00"
};

#endif /* CATCOMP_BLOCK */


/****************************************************************************/



#endif /* LOCALE_H */
