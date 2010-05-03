#ifndef _ICONLIST_PRIVATE_H_
#define _ICONLIST_PRIVATE_H_

#include "iconlist.h"

/*** Instance data **********************************************************/
struct IconList_DATA
{
    APTR                          icld_Pool;                          /* Pool to allocate data from */

    struct RastPort               *icld_DisplayRastPort;
    struct RastPort               *icld_BufferRastPort;
    struct TextFont               *icld_IconLabelFont;
    struct TextFont               *icld_IconInfoFont;

    struct List                   icld_IconList;                      /* IconEntry(s)     */
    struct List                   icld_SelectionList;                 /* Selected Icon(s) */
    struct IconEntry              *icld_SelectionLastClicked;
    struct IconEntry              *icld_FocusIcon;

    LONG                          icld_ViewX,                         /* the leftmost/upper coordinates of the view */
                                  icld_ViewY;
    ULONG                         icld_ViewWidth,                     /* dimensions of the view (_mwidth(obj) and _mheight(obj)) */
                                  icld_ViewHeight,
                                  icld_AreaWidth,                     /* The whole width/height */
                                  icld_AreaHeight;

    /* Drag/Drop Info .. */

    struct IconList_Drop_Event    *icld_DragDropEvent;                /* the icon where the icons have been dropped */
    struct IconList_Click         icld_ClickEvent;

    /* Input / Event Information */
    struct MUI_EventHandlerNode   ehn;

    LONG                          touch_x;
    LONG                          touch_y;

    LONG                          click_x;
    LONG                          click_y;

    ULONG                         last_secs;                          /* DoubleClick stuff */
    ULONG                         last_mics;

    /* RENDERING DATA! ###### */
    LONG                          icld_DrawOffsetX,                   /* coordinates to render to */
                                  icld_DrawOffsetY;
    ULONG                         icld_DisplayFlags;                  /* Internal Sorting related stuff */
    ULONG                         icld_SortFlags;
    ULONG                         icld_IconAreaLargestWidth;          /* Used for icon/label rendering & */
    ULONG                         icld_IconAreaLargestHeight;         /* Positioning                     */
    ULONG                         icld_IconLargestHeight;
    ULONG                         icld_LabelLargestHeight;

    /* values for icld_UpdateMode - :

       UPDATE_SINGLEICON = draw the given single icon only
       UPDATE_SCROLL     = scroll the view by update_scrolldx/update_scrolldy
       UPDATE_RESIZE    = resizing window                                   */

    ULONG                         icld_UpdateMode;
    WORD                          update_scrolldx;
    WORD                          update_scrolldy;
    WORD                          update_oldwidth;
    WORD                          update_oldheight;

    struct IconEntry              *update_entry;
    struct Rectangle              *update_rect1;
    struct Rectangle              *update_rect2;
    struct Rectangle              view_rect;

    struct Rectangle              icld_LassoRectangle;                /* lasso data */
    BOOL                          icld_LassoActive;

#warning "TODO: move config options to a seperate struct"
    /* IconList configuration settings ... */
    ULONG                         icld_LabelPen;		
    ULONG                         icld_LabelShadowPen;
    ULONG                         icld_InfoPen;
    ULONG                         icld_InfoShadowPen;

    ULONG                         icld__Option_LabelTextMaxLen;                   /* max no. of chars to display in a line */
    ULONG                         icld__Option_LastLabelTextMaxLen;

    UBYTE                         icld__Option_IconListMode;                     /* */
    UBYTE                         icld__Option_LabelTextMode;                     /* */
    BOOL                          icld__Option_IconListFixedBackground;          /* */
    BOOL                          icld__Option_IconListScaledBackground;         /* */
    ULONG                         icld__Option_LabelTextMultiLine;               /* No. of lines to display for labels*/
    ULONG                         icld__Option_LastLabelTextMultiLine;

    BOOL                          icld__Option_TrimVolumeNames;

    BOOL                          icld__Option_LabelTextMultiLineOnFocus;        /* Only show "multiline" label for focused icon */
    UBYTE                         icld__Option_IconBorderOverlap;
    UBYTE                         icld__Option_IconHorizontalSpacing;            /* Horizontal/Vert Space between Icon "Areas" */
    UBYTE                         icld__Option_IconVerticalSpacing;
    UBYTE                         icld__Option_IconImageSpacing;                 /* Space between Icon Image and Label Frame */
    UBYTE                         icld__Option_LabelTextHorizontalPadding;       /* Outer padding between label text and frame */
    UBYTE                         icld__Option_LabelTextVerticalPadding;
    UBYTE                         icld__Option_LabelTextBorderWidth;             /* Label frame dimensions */
    UBYTE                         icld__Option_LabelTextBorderHeight;

    UBYTE                         mouse_pressed;
};

#endif /* _ICONLIST_PRIVATE_H_ */
