/*
    Copyright � 1995-2001, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: English
*/

/*********************************************************************************************/

#include "global.h"
#include "calendarclass.h"

#include <string.h>
#include <stdio.h>

#include <aros/debug.h>
#include <aros/asmcall.h>

/*********************************************************************************************/

#define CELL_EXTRAWIDTH 6
#define CELL_EXTRAHEIGHT 4

/*********************************************************************************************/

struct CalendarData
{
    struct MUI_EventHandlerNode ehn;
    struct ClockData	    	clockdata;
    STRPTR  	    	    	*daylabels;
    STRPTR  	    	    	defdaylabels[12];
    WORD    	    	    	cellwidth, base_cellwidth;
    WORD    	    	    	cellheight, base_cellheight;
    WORD    	    	    	mwday; /* weekday of 1st of month */
    WORD    	    	    	old_mday;
};

/*********************************************************************************************/

STRPTR def_daylabels[] =
{
    "Sun",
    "Mon",
    "Tue",
    "Wed",
    "Thu",
    "Fri",
    "Sat"
};

/*********************************************************************************************/

static IPTR Calendar_New(Class *cl, Object *obj, struct opSet *msg)
{
    struct CalendarData *data;
    struct TagItem  	*ti, tags[] =
    {
    	{MUIA_Background, MUII_ButtonBack   	    },
	{TAG_MORE   	, (IPTR)msg->ops_AttrList   }
    };
    
    msg->ops_AttrList = tags;
    
    obj = (Object *)DoSuperMethodA(cl, obj, (Msg)msg);
	
    if (!obj) return 0;
    
    data = INST_DATA(cl, obj);

    data->daylabels = (STRPTR *)GetTagData(MUIA_Calendar_DayLabels, 0, msg->ops_AttrList);
    if (!data->daylabels)
    {
    	struct Locale 	*locale;
	WORD	    	i;
	
	locale = LocaleBase ? OpenLocale(NULL) : NULL;
	
    	data->daylabels = data->defdaylabels;
	for(i = 0; i < 12; i++)
	{
	    if (locale)
	    {
	    	data->defdaylabels[i] = GetLocaleStr(locale, ABDAY_1 + i);
	    }
	    else
	    {
	    	data->defdaylabels[i] = def_daylabels[i];
	    }
	}
	
	if (locale) CloseLocale(locale);
    }
    
    if ((ti = FindTagItem(MUIA_Calendar_Date, msg->ops_AttrList)))
    {
    	struct ClockData *cd = (struct ClockData *)ti->ti_Data;
	
	data->clockdata = *cd;
    }
    else
    {
    	struct timeval tv;
	
    	GetSysTime(&tv);
	Amiga2Date(tv.tv_secs, &data->clockdata);
    }
    
    data->ehn.ehn_Events   = IDCMP_MOUSEBUTTONS;
    data->ehn.ehn_Priority = 0;
    data->ehn.ehn_Flags    = 0;
    data->ehn.ehn_Object   = obj;
    data->ehn.ehn_Class    = cl;
    
    return (IPTR)obj;
}

/*********************************************************************************************/

static IPTR Calendar_Dispose(Class *cl, Object *obj, Msg msg)
{
    return DoSuperMethodA(cl, obj, msg);
}

/*********************************************************************************************/

static IPTR Calendar_Set(Class *cl, Object *obj, struct opSet *msg)
{
    struct CalendarData *data = INST_DATA(cl, obj);
    struct ClockData	 old_clockdata;
    struct TagItem      *tags  = msg->ops_AttrList;
    struct TagItem      *tag;
    BOOL    	    	 redraw = FALSE;
    
    old_clockdata = data->clockdata;
    
    while ((tag = NextTagItem(&tags)) != NULL)
    {
    	switch(tag->ti_Tag)
	{
    	    case MUIA_Calendar_Date:
		data->clockdata = *(struct ClockData *)tag->ti_Data;
		redraw = TRUE;
		break;

	    case MUIA_Calendar_Year:
		data->clockdata.year = tag->ti_Data;
		redraw = TRUE;
		break;

	    case MUIA_Calendar_Month:
		data->clockdata.month = tag->ti_Data;
		redraw = TRUE;
		break;

	    case MUIA_Calendar_Month0:
		data->clockdata.month = tag->ti_Data + 1;
		redraw = TRUE;
		break;

	    case MUIA_Calendar_MonthDay:
		data->clockdata.mday = tag->ti_Data;
		redraw = TRUE;
		break;

	    case MUIA_Calendar_MonthDay0:
		data->clockdata.mday = tag->ti_Data + 1;
		redraw = TRUE;
		break;
		
	} /* switch(tag->ti_Tag) */
	
    } /* while ((tag = NextTagItem(&tags)) != NULL) */
    
    if (redraw)
    {
    	struct ClockData cd2;
    	ULONG 	    	 secs;
	
	cd2 = data->clockdata;
	cd2.mday = 1;
	
	if (data->clockdata.mday > NumMonthDays(&cd2))
	{
	    data->clockdata.mday = NumMonthDays(&cd2);
	}
	
	secs = Date2Amiga(&data->clockdata);
	
	Amiga2Date(secs, &data->clockdata);
	
	if ((data->clockdata.year != old_clockdata.year) ||
	    (data->clockdata.month != old_clockdata.month) ||
	    (data->clockdata.mday != old_clockdata.mday))
	{
	    if ((data->clockdata.year == old_clockdata.year) &&
	    	(data->clockdata.month == old_clockdata.month))
	    {
	    	data->old_mday = old_clockdata.mday;
	    }
    	    MUI_Redraw(obj, MADF_DRAWUPDATE);
	    data->old_mday = 0;
	}
    }
    
    return DoSuperMethodA(cl, obj, (Msg)msg);
}

/*********************************************************************************************/

static IPTR Calendar_Get(Class *cl, Object *obj, struct opGet *msg)
{
    struct CalendarData *data = INST_DATA(cl, obj);
    IPTR    	    	 retval = TRUE;
    
    switch(msg->opg_AttrID)
    {
    	case MUIA_Calendar_Date:
	    *(struct ClockData **)msg->opg_Storage = &data->clockdata;
	    break;

	case MUIA_Calendar_MonthDay:
	    *msg->opg_Storage = data->clockdata.mday;
	    break;

	case MUIA_Calendar_MonthDay0:
	    *msg->opg_Storage = data->clockdata.mday - 1;
	    break;
	    
	case MUIA_Calendar_Month:
	    *msg->opg_Storage = data->clockdata.month;
	    break;

	case MUIA_Calendar_Month0:
	    *msg->opg_Storage = data->clockdata.month - 1;
	    break;
	    
	case MUIA_Calendar_Year:
	    *msg->opg_Storage = data->clockdata.year;
	    break;
	    
    	default:
	    retval = DoSuperMethodA(cl, obj, (Msg)msg);
	    break;
	    
    }
    
    return retval;
}

/*********************************************************************************************/

static IPTR Calendar_Setup(Class *cl, Object *obj, struct MUIP_Setup *msg)
{
    struct CalendarData *data = INST_DATA(cl, obj);
    struct RastPort 	 rp;
    WORD    	    	 i, w;
    
    if (!DoSuperMethodA(cl, obj, (Msg)msg)) return FALSE;

    InitRastPort(&rp);
    SetFont(&rp, _font(obj));
    
    data->cellheight = _font(obj)->tf_YSize + CELL_EXTRAHEIGHT;
    
    SetSoftStyle(&rp, FSF_BOLD, AskSoftStyle(&rp));
    
    for(i = 0; i < 7; i++)
    {
    	w = TextLength(&rp, data->daylabels[i], strlen(data->daylabels[i]));
	if (w > data->cellwidth) data->cellwidth = w;
    }
    
    SetSoftStyle(&rp, FS_NORMAL, AskSoftStyle(&rp));
 
    for(i = 1; i <= 31; i++)
    {
    	char s[3];
	
	sprintf(s, "%d", i);
	
	w = TextLength(&rp, s, strlen(s));
	if (w > data->cellwidth) data->cellwidth = w;
    }
    
    data->cellwidth += CELL_EXTRAWIDTH;
    
    data->base_cellwidth = data->cellwidth;
    data->base_cellheight = data->cellheight;
    
    DoMethod(_win(obj), MUIM_Window_AddEventHandler, &data->ehn);
    
    return TRUE;
}

/*********************************************************************************************/

static IPTR Calendar_Cleanup(Class *cl, Object *obj, struct MUIP_Cleanup *msg)
{
    struct CalendarData *data = INST_DATA(cl, obj);
 
    DoMethod(_win(obj), MUIM_Window_RemEventHandler, &data->ehn);
    
    return DoSuperMethodA(cl, obj, (Msg)msg);
}

/*********************************************************************************************/

static IPTR Calendar_AskMinMax(Class *cl, Object *obj, struct MUIP_AskMinMax *msg)
{
    struct CalendarData *data = INST_DATA(cl, obj);

    DoSuperMethodA(cl, obj, (Msg)msg);
    
    msg->MinMaxInfo->MinWidth  += data->cellwidth * 7 + 2;
    msg->MinMaxInfo->MinHeight += data->cellheight * 7 + 1;
    msg->MinMaxInfo->DefWidth  += data->cellwidth * 7 + 2;
    msg->MinMaxInfo->DefHeight += data->cellheight * 7 + 1;
    msg->MinMaxInfo->MaxWidth   = MUI_MAXMAX;
    msg->MinMaxInfo->MaxHeight  = MUI_MAXMAX;
    
    
    return TRUE;
}

/*********************************************************************************************/

static IPTR Calendar_Draw(Class *cl, Object *obj, struct MUIP_Draw *msg)
{
    struct CalendarData *data = INST_DATA(cl, obj);
    WORD    	    	 x, y, offx, offy, day, mdays;
    
    DoSuperMethodA(cl, obj, (Msg)msg);
    
    if (!(msg->flags & (MADF_DRAWOBJECT | MADF_DRAWUPDATE))) return 0;
    
    x = (_mwidth(obj) - 2) / 7;
    y = x * data->base_cellheight / data->base_cellwidth;
    
    data->cellheight = (_mheight(obj) - 1) / 7;
    data->cellwidth = data->cellheight * data->base_cellwidth / data->base_cellheight;
    
    if ((data->cellwidth > x) || (data->cellheight > y))
    {
    	data->cellwidth = x;
	data->cellheight = y;
    }
    
    
    offx = _mleft(obj) + (_mwidth(obj) - data->cellwidth * 7 - 2) / 2 + 1;
    offy = _mtop(obj) + (_mheight(obj) - data->cellheight * 7 - 1) / 2;
    
    /* ~random 8 * 7 to make sure expression inside brackets is positive */
    data->mwday = (8 * 7 + data->clockdata.wday - data->clockdata.mday + 1) % 7;
    
    mdays = NumMonthDays(&data->clockdata);

    //kprintf("actdate = %d.%d.%d  wday = %d\n", data->clockdata.mday, data->clockdata.month, data->clockdata.year, data->clockdata.wday);

    //kprintf("mwday = %d  mdays = %d\n", data->mwday, mdays);
    
    day = firstweekday - data->mwday + 1 - 7;
    
    SetFont(_rp(obj), _font(obj));
    SetDrMd(_rp(obj), JAM1);
    
    if (!(msg->flags & MADF_DRAWUPDATE))
    {
    	SetAPen(_rp(obj), _dri(obj)->dri_Pens[SHADOWPEN]);
	Move(_rp(obj), offx - 1,
	    	       offy);
	Draw(_rp(obj), offx - 1,
	    	       offy + data->cellheight * 7);
	Draw(_rp(obj), offx +  data->cellwidth * 7,
	    	       offy + data->cellheight * 7);
	Draw(_rp(obj), offx + data->cellwidth * 7,
	    	       offy);
    }
    
    for(y = 0; y < 7; y++)
    {
    	if (!y && (msg->flags & MADF_DRAWUPDATE))
	{
	    day += 7;
	    continue;
    	}
	
    	for(x = 0; x < 7; x++)
	{
	    STRPTR text;	    
	    UBYTE buf[3];
	    
	    if (data->old_mday && (day != data->old_mday) && (day != data->clockdata.mday))
	    {
	    	day++;
		continue;
	    }
	    
	    text = NULL;
	    if (day == data->clockdata.mday)
	    {
	    	SetAPen(_rp(obj), _pens(obj)[MPEN_FILL]);
	    }
	    else
	    {
	    	SetAPen(_rp(obj), _dri(obj)->dri_Pens[y ? SHINEPEN : SHADOWPEN]);
	    }
	    
	    RectFill(_rp(obj), x * data->cellwidth + 1 + offx,
	    	    	       y * data->cellheight + 1 + offy,
			       x * data->cellwidth + data->cellwidth - 2 + offx,
			       y * data->cellheight + data->cellheight - 2 + offy);
			       
	    SetAPen(_rp(obj), _dri(obj)->dri_Pens[SHADOWPEN]);
	    Move(_rp(obj), x * data->cellwidth + offx,
		    	   y * data->cellheight + offy);
	    Draw(_rp(obj), x * data->cellwidth + data->cellwidth - 1 + offx,
		    	   y * data->cellheight + offy);
	    Draw(_rp(obj), x * data->cellwidth + data->cellwidth - 1 + offx,
		    	   y * data->cellheight + data->cellheight - 1 + offy);
	    Draw(_rp(obj), x * data->cellwidth + offx,
		    	   y * data->cellheight + data->cellheight - 1 + offy);
	    Draw(_rp(obj), x * data->cellwidth + offx,
		    	   y * data->cellheight + offy);	    

    	    if (y > 0)
	    {
	    	if ((day >= 1) && (day <= mdays))
		{
		    sprintf(buf, "%d", day);
	    	    SetSoftStyle(_rp(obj), FS_NORMAL, AskSoftStyle(_rp(obj)));
		    SetAPen(_rp(obj), _dri(obj)->dri_Pens[day == data->clockdata.mday ? SHADOWPEN : FILLTEXTPEN]);
		    text = buf;
		}
	    }
	    else
	    {
	    	SetSoftStyle(_rp(obj), FSF_BOLD, AskSoftStyle(_rp(obj)));
		SetAPen(_rp(obj), _dri(obj)->dri_Pens[SHINEPEN]);
		
		text = data->daylabels[(x + firstweekday) % 7];
	    }
	    
	    if (text)
	    {
	    	WORD tx, ty, tw;
		
		tw = TextLength(_rp(obj), text, strlen(text));
		tx = offx + x * data->cellwidth + (data->cellwidth - tw) / 2;
		ty = offy + y * data->cellheight + (data->cellheight - _font(obj)->tf_YSize) / 2;
		
		Move(_rp(obj), tx, ty + _font(obj)->tf_Baseline);
		Text(_rp(obj), text, strlen(text));
	    }
	    
	    day++;
	    
	} /* for(x = 0; x < 7; x++) */
	
    } /* for(y = 0; y < 7; y++) */
    
    return 0;
}

/*********************************************************************************************/

static WORD DayUnderMouse(Object *obj, struct CalendarData *data, struct IntuiMessage *imsg)
{
    WORD x, y, offx, offy, i;
    
    offx = (_mwidth(obj) - data->cellwidth * 7 - 2) / 2 + 1;
    offy = (_mheight(obj) - data->cellheight * 7 - 1) / 2 + data->cellheight;
    
    x = imsg->MouseX - _mleft(obj) - offx;
    y = imsg->MouseY - _mtop(obj) - offy;
    
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x >= data->cellwidth * 7) x = data->cellwidth * 7 - 1;
    if (y >= data->cellheight * 6) y = data->cellheight * 6 - 1;
    
    x /= data->cellwidth;
    y /= data->cellheight;
    
    i = y * 7 + x;
    
    i += firstweekday - data->mwday + 1;
    
    if (i < 1)
    {
    	i = 1;
    }
    else if (i > NumMonthDays(&data->clockdata))
    {
    	i = NumMonthDays(&data->clockdata);
    }
    
    return i;
}

/*********************************************************************************************/

#define _between(a,x,b) ((x)>=(a) && (x)<=(b))

/*********************************************************************************************/

static IPTR Calendar_HandleEvent(Class *cl, Object *obj, struct MUIP_HandleEvent *msg)
{
    struct CalendarData *data = INST_DATA(cl, obj);
    
    if (msg->muikey == MUIKEY_NONE)
    {
    	WORD x1 = _mleft(obj) + (_mwidth(obj)  - data->cellwidth  * 7 - 2) / 2 + 1;
	WORD y1 = _mtop(obj)  + (_mheight(obj) - data->cellheight * 7 - 1) / 2 + data->cellheight;
	WORD x2 = x1 + data->cellwidth * 7 - 1;
	WORD y2 = y1 + data->cellheight * 6 - 1;
	WORD day;
	
	BOOL in = _between(x1, msg->imsg->MouseX, x2) && _between(y1, msg->imsg->MouseY, y2);

    	switch(msg->imsg->Class)
	{
	    case IDCMP_MOUSEBUTTONS:
	    	switch(msg->imsg->Code)
		{
		    case SELECTDOWN:
		    	if (in)
			{
			    day = DayUnderMouse(obj, data, msg->imsg);
			    if (day != data->clockdata.mday)
			    {
			    	set(obj, MUIA_Calendar_MonthDay, day);
			    }
			    DoMethod(_win(obj), MUIM_Window_RemEventHandler, (IPTR)&data->ehn);
			    data->ehn.ehn_Events |= IDCMP_MOUSEMOVE;
	                    DoMethod(_win(obj), MUIM_Window_AddEventHandler, (IPTR)&data->ehn);
			    return MUI_EventHandlerRC_Eat;
			}
			break;
			
		    case SELECTUP:
		    	if (data->ehn.ehn_Events & IDCMP_MOUSEMOVE)
			{
			    DoMethod(_win(obj), MUIM_Window_RemEventHandler, (IPTR)&data->ehn);
			    data->ehn.ehn_Events &= ~IDCMP_MOUSEMOVE;
	                    DoMethod(_win(obj), MUIM_Window_AddEventHandler, (IPTR)&data->ehn);
		    	    return MUI_EventHandlerRC_Eat;
			}
			break;
		}
		break;
		  	   	
	    case IDCMP_MOUSEMOVE:
	    	if (data->ehn.ehn_Events & IDCMP_MOUSEMOVE)
		{
	    	    day = DayUnderMouse(obj, data, msg->imsg);
		    if (day != data->clockdata.mday)
		    {
			set(obj, MUIA_Calendar_MonthDay, day);
		    }
		    return MUI_EventHandlerRC_Eat;		    
		}
	    	break;
		
	} /* switch(msg->imsg->Class) */
	
    } /* if (msg->muikey == MUIKEY_NONE) */
    
    return 0;
}

/*********************************************************************************************/

AROS_UFH3S(IPTR, Calendar_Dispatcher,
	AROS_UFHA(Class  *, cl,  A0),
	AROS_UFHA(Object *, obj, A2),
	AROS_UFHA(Msg     , msg, A1))
{
    IPTR retval = 0;
    
    switch (msg->MethodID)
    {
	case OM_NEW:
	    retval = Calendar_New(cl, obj, (struct opSet *)msg);
	    break;
	    
	case OM_DISPOSE:
	    retval = Calendar_Dispose(cl, obj, msg);
	    break;
	    
	case OM_SET:
	    retval = Calendar_Set(cl, obj, (struct opSet *)msg);
	    break;
	    
	case OM_GET:
	    retval = Calendar_Get(cl, obj, (struct opGet *)msg);
    	    break;
	    
	case MUIM_Setup:
	    retval = Calendar_Setup(cl, obj, (struct MUIP_Setup *)msg);
	    break;
	    
	case MUIM_Cleanup:
	    retval = Calendar_Cleanup(cl, obj, (struct MUIP_Cleanup *)msg);
	    break;
	    
	case MUIM_AskMinMax:
	    retval = Calendar_AskMinMax(cl, obj, (struct MUIP_AskMinMax *)msg);
	    break;
	    
	case MUIM_Draw:
	    retval = Calendar_Draw(cl, obj, (struct MUIP_Draw *)msg);
	    break;
	    
	case MUIM_HandleEvent:
	    retval = Calendar_HandleEvent(cl, obj, (struct MUIP_HandleEvent *)msg);
	    break;
	    
	default:
	    retval = DoSuperMethodA(cl, obj, msg);
	    break;	       
    }
    
    return retval;
}

/*********************************************************************************************/

BOOL MakeCalendarClass(void)
{
    calendarmcc = MUI_CreateCustomClass(NULL, MUIC_Area, NULL,
    	    	    	    	    	sizeof(struct CalendarData), Calendar_Dispatcher);    
    return calendarmcc ? TRUE : FALSE;
}

/*********************************************************************************************/

void KillCalendarClass(void)
{
    if (calendarmcc) MUI_DeleteCustomClass(calendarmcc);
}

/*********************************************************************************************/
