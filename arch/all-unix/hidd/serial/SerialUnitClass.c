/*
    (C) 1998 AROS - The Amiga Research OS
    $Id$

    Desc: Serial Unit hidd class implementation.
    Lang: english
*/

/* Some POSIX includes */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>


#define AROS_ALMOST_COMPATIBLE 1

/* the rest are Amiga includes */
#define timeval aros_timeval
#include <proto/exec.h>
#include <proto/utility.h>
#include <proto/oop.h>
#include <proto/alib.h>
#include <exec/libraries.h>
#include <exec/ports.h>
#include <exec/memory.h>
#include <exec/interrupts.h>
#include <exec/lists.h>

#include <utility/tagitem.h>
#include <hidd/serial.h>
#include <hidd/unixio.h>

#include <devices/serial.h>

#include "serial_intern.h"
#undef timeval

#undef  SDEBUG
#undef  DEBUG
#define SDEBUG 1
#define DEBUG 1
#include <aros/debug.h>

void serialunit_receive_data();
void serialunit_write_more_data();

/* Some utility functions */
void settermios(struct HIDDSerialUnitData * data);


char * unitname[] =
{
 "/dev/ttyS0",
 "/dev/ttyS1",
 "/dev/ttyS2",
 "/dev/ttyS3"
 };

/*************************** Classes *****************************/

static OOP_AttrBase HiddSerialUnitAB;

static struct OOP_ABDescr attrbases[] =
{
    { IID_Hidd_SerialUnit, &HiddSerialUnitAB },
    { NULL,	NULL }
};

/******* SerialUnit::New() ***********************************/
static OOP_Object *serialunit_new(OOP_Class *cl, OOP_Object *obj, struct pRoot_New *msg)
{
  struct HIDDSerialUnitData * data;
  static const struct TagItem tags[] = {{ TAG_END, 0}};
  struct TagItem *tag, *tstate;
  ULONG unitnum = 0;
  
  EnterFunc(bug("SerialUnit::New()\n"));

  tstate = msg->attrList;
  while ((tag = NextTagItem((const struct TagItem **)&tstate)))
  {
      ULONG idx;

      if (IS_HIDDSERIALUNIT_ATTR(tag->ti_Tag, idx))
      {
	  switch (idx)
	  {
	      case aoHidd_SerialUnit_Unit:
		  unitnum = (ULONG)tag->ti_Data;
		  break;
	  }
      }

  } /* while (tags to process) */
  
  D(bug("!!!!Request for unit number %d\n",unitnum));

  obj = (OOP_Object *)OOP_DoSuperMethod(cl, obj, (OOP_Msg)msg);

  if (obj)
  {
    struct termios _termios;
    
    data = OOP_INST_DATA(cl, obj);
    
    data->unitnum = unitnum;

    D(bug("Opening %s.\n",unitname[data->unitnum]));

    data->filedescriptor = open(unitname[data->unitnum], O_NONBLOCK|O_RDWR);

    D(bug("Opened %s on handle %d\n",unitname[data->unitnum], data->filedescriptor));
    
    if (-1 != data->filedescriptor)
    {
      /*
      ** Configure the tty driver
      */
      data->baudrate       = SER_DEFAULT_BAUDRATE;
      
      tcgetattr(data->filedescriptor, &data->orig_termios);
      tcgetattr(data->filedescriptor, &_termios); 
      cfmakeraw(&_termios);
      cfsetspeed(&_termios, data->baudrate);

/* !!! untested
      _termios.c_iflag |= (IXON|IXOFF);
*/
      if (tcsetattr(data->filedescriptor, TCSANOW, &_termios) >=0)
      {
        data->replyport_read = AllocMem(sizeof(struct MsgPort), MEMF_PUBLIC|MEMF_CLEAR);
        data->replyport_write= AllocMem(sizeof(struct MsgPort), MEMF_PUBLIC|MEMF_CLEAR);

        if (data->replyport_read && data->replyport_write)
        {
          /*
          ** Init the msg ports. They don't need a signal to be allocated
          */
          NEWLIST(&data->replyport_read->mp_MsgList);
          data->replyport_read ->mp_Node.ln_Type = NT_MSGPORT;

          NEWLIST(&data->replyport_write->mp_MsgList);
          data->replyport_write->mp_Node.ln_Type = NT_MSGPORT;

          data->softint_read  = AllocMem(sizeof(struct Interrupt), MEMF_CLEAR);
          data->softint_write = AllocMem(sizeof(struct Interrupt), MEMF_CLEAR);

          if (data->softint_read && data->softint_write)
          {
            data->softint_read->is_Data = data;
            data->softint_read->is_Code = serialunit_receive_data;

            data->softint_write->is_Data = data;
            data->softint_write->is_Code = serialunit_write_more_data;

            data->replyport_read->mp_Flags = PA_SOFTINT;
            data->replyport_read->mp_SoftInt = data->softint_read;

            data->replyport_write->mp_Flags = PA_SOFTINT;
            data->replyport_write->mp_SoftInt = data->softint_write;

            data->unixio_read  = OOP_NewObject(NULL, CLID_Hidd_UnixIO, (struct TagItem *)tags);
            data->unixio_write = OOP_NewObject(NULL, CLID_Hidd_UnixIO, (struct TagItem *)tags);

            if (NULL != data->unixio_read && NULL != data->unixio_write)
            {
              ULONG error;
              D(bug("Creating UnixIO AsyncIO command!\n"));

              error = Hidd_UnixIO_AsyncIO(data->unixio_read,
                                          data->filedescriptor,
                                          data->replyport_read,
                                          vHidd_UnixIO_Read);
              goto exit;

            }

            if (NULL != data->unixio_read)
              OOP_DisposeObject(data->unixio_read);

            if (NULL != data->unixio_write)
              OOP_DisposeObject(data->unixio_write);
          }
          
          if (data->softint_read) 
            FreeMem(data->softint_read, sizeof(struct Interrupt));
          if (data->softint_write)
            FreeMem(data->softint_write, sizeof(struct Interrupt));
        }
        
        if (data->replyport_read)
          FreeMem(data->replyport_read , sizeof(struct MsgPort));
        if (data->replyport_write)
          FreeMem(data->replyport_write, sizeof(struct MsgPort));

      } 
      
      close(data->filedescriptor);  
    }

    OOP_DisposeObject(obj);
    obj = NULL;
  } /* if (obj) */

  D(bug("%s - an error occurred!\n",__FUNCTION__));

exit:
  ReturnPtr("SerialUnit::New()", OOP_Object *, obj);
}

/******* SerialUnit::Dispose() ***********************************/
static OOP_Object *serialunit_dispose(OOP_Class *cl, OOP_Object *obj, OOP_Msg msg)
{
  struct HIDDSerialUnitData * data;
  EnterFunc(bug("SerialUnit::Dispose()\n"));

  data = OOP_INST_DATA(cl, obj);
  D(bug("Freeing filedescriptor (%d)!\n",data->filedescriptor));

  tcsetattr(data->filedescriptor, TCSANOW, &data->orig_termios);
  if (-1 != data->filedescriptor)
  { 
    Hidd_UnixIO_AbortAsyncIO(data->unixio_read,
                             data->filedescriptor);
//    Hidd_UnixIO_AbortAsyncIONotification(data->unixio_write,
//                                         data->filedescriptor);

    close(data->filedescriptor);
  
    FreeMem(data->replyport_read,  sizeof(struct MsgPort));
    FreeMem(data->replyport_write, sizeof(struct MsgPort));

    FreeMem(data->softint_read , sizeof(struct Interrupt));
    FreeMem(data->softint_write, sizeof(struct Interrupt));

    OOP_DisposeObject(data->unixio_read);
    OOP_DisposeObject(data->unixio_write);
  }
  OOP_DoSuperMethod(cl, obj, (OOP_Msg)msg);
  ReturnPtr("SerialUnit::Dispose()", OOP_Object *, obj);
}



/******* SerialUnit::Init() **********************************/
BOOL serialunit_init(OOP_Class *cl, OOP_Object *o, struct pHidd_SerialUnit_Init *msg)
{
  struct HIDDSerialUnitData * data = OOP_INST_DATA(cl, o);
  
  EnterFunc(bug("SerialUnit::Init()\n"));
  
  data->DataReceivedCallBack = msg->DataReceived;
  data->DataReceivedUserData = msg->DataReceivedUserData;
  data->DataWriteCallBack    = msg->WriteData;
  data->DataWriteUserData    = msg->WriteDataUserData;
  
  ReturnBool("SerialUnit::Init()", TRUE);
}

/******* SerialUnit::Write() **********************************/
ULONG serialunit_write(OOP_Class *cl, OOP_Object *o, struct pHidd_SerialUnit_Write *msg)
{
  struct HIDDSerialUnitData * data = OOP_INST_DATA(cl, o);
  ULONG len = 0;
  ULONG error;
  
  EnterFunc(bug("SerialUnit::Write()\n"));
  /*
   * If the output is currently suspended then I don't do anything...
   */

  if (TRUE == data->stopped)
    return 0;

  D(bug("Writing %d bytes to fd %d (stream: %s)\n",
        msg->Length,
        data->filedescriptor,
        msg->Outbuffer));

  len = write(data->filedescriptor,
              msg->Outbuffer,
              msg->Length);

  if (len < msg->Length)
  {

    // !!!!!! FROM WHAT I CAN TELL THE FOLLOWING LINE
    //        CAUSES PROBLEMS. IT IS NECESSARY TO HAVE IT, THOUGH.
    error = Hidd_UnixIO_AsyncIO(data->unixio_write,
                                data->filedescriptor,
                                data->replyport_write,
                                vHidd_UnixIO_Write);

  }

  ReturnInt("SerialUnit::Write()",ULONG, len);
}

/***************************************************************************/

static ULONG valid_baudrates[] =
{
  0,
  50,
  75,
  110,
  134,
  150,
  200,
  300,
  600,
  1200,
  1800,
  2400,
  4800,
  9600,
  19200,
  38400,
  57600,
  115200,
  -1
};

/*** unused due to cfsetspeed ***

static LONG unix_baudrates[] =
{
  B0,
  B50,
  B75,
  B110
  B134,
  B150,
  B200,
  B300,
  B600,
  B1200,
  B1800,
  B2400,
  B4800,
  B9600,
  B19200,
  B38400,
  B57600,
  B115200
};

********************************/

/******* SerialUnit::SetBaudrate() **********************************/
ULONG serialunit_setbaudrate(OOP_Class *cl, OOP_Object *o, struct pHidd_SerialUnit_SetBaudrate *msg)
{
  struct HIDDSerialUnitData * data = OOP_INST_DATA(cl, o);
  BOOL valid = FALSE;
  
  if (msg->baudrate != data->baudrate)
  {
    int i = 0;
    D(bug("Trying to adjust the baudrate to %d\n",msg->baudrate));
    while (FALSE == valid && -1 != valid_baudrates[i])
    {
      if (msg->baudrate == valid_baudrates[i])
      {
        struct termios _termios;
        tcgetattr(data->filedescriptor, &_termios); 
        cfsetspeed(&_termios, data->baudrate);

        if (tcsetattr(data->filedescriptor, TCSADRAIN, &_termios) <0)
        {
          D(bug("Failed to set to new baudrate"));
        }
        else
        {
          D(bug("Adjusted to new baudrate!\n"));
          valid = TRUE;
        }
      }
      i++;
    } /* while */
  } /* if */
  return valid;
}

static UBYTE valid_datalengths[] =
{
  5,
  6,
  7,
  8,
  -1
};

static UBYTE unix_datalengths[] =
{
  CS5,
  CS6,
  CS7,
  CS8
};

/******* SerialUnit::SetParameters() **********************************/
ULONG serialunit_setparameters(OOP_Class *cl, OOP_Object *o, struct pHidd_SerialUnit_SetParameters *msg)
{
  struct HIDDSerialUnitData * data = OOP_INST_DATA(cl, o);
  BOOL valid = TRUE;
  int i = 0;
  struct TagItem * tags = msg->tags;
  
  while (TAG_END != tags[i].ti_Tag && TRUE == valid)
  {
    switch (tags[i].ti_Tag)
    {
      case TAG_DATALENGTH:
      
        if ((UBYTE)tags[i].ti_Data != data->datalength)
        {
          int i = 0;
          BOOL found = FALSE;
          while (TRUE == valid && -1 != (BYTE)valid_datalengths[i])
          {
            if ((UBYTE)tags[i].ti_Data == valid_datalengths[i])
            {
              found = TRUE;
              data->datalength = unix_datalengths[i];
            }

            valid = found;
            i++;
          }
          
        }
      break;
        
      case TAG_STOP_BITS:
      break;
      
      case TAG_PARITY:
	  if ( /* PARITY_0    == tags[i].ti_Data ||
	          PARITY_1    == tags[i].ti_Data || */
	      PARITY_EVEN == tags[i].ti_Data ||
              PARITY_ODD  == tags[i].ti_Data)  
          {
	     data->parity     = TRUE;
	     data->paritytype = tags[i].ti_Data;
	  }
	  else
	    valid = FALSE;
      break;
      
      case TAG_PARITY_OFF:
        data->parity = FALSE;
      break;
      
      default: 
        valid = FALSE;
    }
    i++;
  }
  
  settermios(data);
  
  return valid;
}

/******* SerialUnit::SendBreak() **********************************/
BYTE serialunit_sendbreak(OOP_Class *cl, OOP_Object *o, struct pHidd_SerialUnit_SendBreak *msg)
{
  struct HIDDSerialUnitData * data = OOP_INST_DATA(cl, o);
  
  if (0 == tcsendbreak(data->filedescriptor, msg->duration))
    return 0;
  
  return SerErr_LineErr;
}

/******* SerialUnit::Start() **********************************/
VOID serialunit_start(OOP_Class *cl, OOP_Object *o, struct pHidd_SerialUnit_Start *msg)
{
  struct HIDDSerialUnitData * data = OOP_INST_DATA(cl, o);

  /*
   * Allow or start feeding the UART with data. Get the data
   * from upper layer.
   */
  if (NULL != data->DataWriteCallBack)
     data->DataWriteCallBack(data->unitnum, data->DataWriteUserData);
   /*
    * Also mark the stopped flag as FALSE.
    */
   data->stopped = FALSE;
}  
  

/******* SerialUnit::SendBreak() **********************************/
VOID serialunit_stop(OOP_Class *cl, OOP_Object *o, struct pHidd_SerialUnit_Stop *msg)
{
  struct HIDDSerialUnitData * data = OOP_INST_DATA(cl, o);

  /*
   * The next time the interrupt comes along and asks for
   * more data we just don't do anything...
   */
  data->stopped = TRUE;
}

/****** SerialUnit::GetCapabilities ********************************/
VOID serialunit_getcapabilities(OOP_Class * cl, OOP_Object *o, struct TagItem * tags)
{
  if (NULL != tags)
  {
    int i = 0;
    BOOL end = FALSE;
    while (FALSE == end)
    {
      switch (tags[i].ti_Tag)
      {
        case HIDDA_SerialUnit_BPSRate:
          tags[i].ti_Data = (STACKIPTR)valid_baudrates;
        break;
        
        case HIDDA_SerialUnit_DataLength:
          tags[i].ti_Data = (STACKIPTR)valid_datalengths;
        break;
        
        case TAG_DONE:
          end = TRUE;
        break;
      }
      i++;
    }
  }
}

/************* The software interrupt handler that gets data from UART *****/


#undef OOPBase
#undef SysBase
#undef UtilityBase

#define READBUFFER_SIZE 513

AROS_UFH3(void, serialunit_receive_data,
   AROS_UFHA(APTR, iD, A1),
   AROS_UFHA(APTR, iC, A5),
   AROS_UFHA(struct ExecBase *, SysBase, A6))
{
  AROS_USERFUNC_INIT

  ULONG error;
  struct HIDDSerialUnitData * data = iD;
  ssize_t len;
  UBYTE buffer[READBUFFER_SIZE];
  struct Message * msg;

  /*
  ** Get the unixio message from my port and free it
  */
  msg = GetMsg(data->replyport_read);
  FreeMem(msg, sizeof(struct uioMessage));

  /*
  ** Read the data from the port ...
  */
  len = read(data->filedescriptor, buffer, READBUFFER_SIZE);
  /*
  ** ... and deliver them to whoever is interested. 
  */

  if (NULL != data->DataReceivedCallBack)
    data->DataReceivedCallBack(buffer, len, data->unitnum, data->DataReceivedUserData);

  /*
  ** I want to be notified when the next data are coming in.
  */
  error = Hidd_UnixIO_AsyncIO(data->unixio_read,
                              data->filedescriptor,
                              data->replyport_read,
                              vHidd_UnixIO_Read);
  AROS_USERFUNC_EXIT
}

AROS_UFH3(void, serialunit_write_more_data,
   AROS_UFHA(APTR, iD, A1),
   AROS_UFHA(APTR, iC, A5),
   AROS_UFHA(struct ExecBase *, SysBase, A6))
{
  AROS_USERFUNC_INIT

  struct HIDDSerialUnitData * data = iD;
  struct Message * msg;

  /*
  ** Get the unixio message from my port and free it
  */
  msg = GetMsg(data->replyport_read);
  FreeMem(msg, sizeof(struct uioMessage));

  /*
  ** Ask for more data be written to the unit
  ** but only if output is not currently suspended.
  */
  if (TRUE == data->stopped)
    return;
  D(bug("Asking for more data to be written to unit %d\n",data->unitnum));

  if (NULL != data->DataWriteCallBack)
    data->DataWriteCallBack(data->unitnum, data->DataWriteUserData);

  AROS_USERFUNC_EXIT
}


/******* init_serialunitclass ********************************/

#define SysBase     (csd->sysbase)
#define OOPBase     (csd->oopbase)
#define UtilityBase (csd->utilitybase)


#define NUM_ROOT_METHODS 2
#define NUM_SERIALUNIT_METHODS moHidd_SerialUnit_NumMethods

OOP_Class *init_serialunitclass (struct class_static_data *csd)
{
    OOP_Class *cl = NULL;
    
    struct OOP_MethodDescr serialunithiddroot_descr[NUM_ROOT_METHODS + 1] = 
    {
        {(IPTR (*)())serialunit_new,		moRoot_New},
        {(IPTR (*)())serialunit_dispose,	moRoot_Dispose},
/*
        {(IPTR (*)())serialunit_set,		moRoot_Set},
        {(IPTR (*)())serialunit_get,		moRoot_Get},
*/
        {NULL, 0UL}
    };
    
    struct OOP_MethodDescr serialunithidd_descr[NUM_SERIALUNIT_METHODS + 1] =
    {
        {(IPTR (*)())serialunit_init,		moHidd_SerialUnit_Init},
        {(IPTR (*)())serialunit_write,		moHidd_SerialUnit_Write},
        {(IPTR (*)())serialunit_setbaudrate,	moHidd_SerialUnit_SetBaudrate},
        {(IPTR (*)())serialunit_setparameters,	moHidd_SerialUnit_SetParameters},
        {(IPTR (*)())serialunit_sendbreak,	moHidd_SerialUnit_SendBreak},
        {(IPTR (*)())serialunit_start,          moHidd_SerialUnit_Start},
        {(IPTR (*)())serialunit_stop,           moHidd_SerialUnit_Stop}, 
        {(IPTR (*)())serialunit_getcapabilities,moHidd_SerialUnit_GetCapabilities},
        {NULL, 0UL}
    };
    
    struct OOP_InterfaceDescr ifdescr[] =
    {
        {serialunithiddroot_descr	, IID_Root		, NUM_ROOT_METHODS},
        {serialunithidd_descr		, IID_Hidd_SerialUnit	, NUM_SERIALUNIT_METHODS},
        {NULL, NULL, 0}
    };

    OOP_AttrBase MetaAttrBase = OOP_GetAttrBase(IID_Meta);
        
    struct TagItem tags[] =
    {
        { aMeta_SuperID,                (IPTR)CLID_Root},
        { aMeta_InterfaceDescr,         (IPTR)ifdescr},
        { aMeta_ID,                     (IPTR)CLID_Hidd_SerialUnit},
        { aMeta_InstSize,               (IPTR)sizeof (struct HIDDSerialUnitData) },
        {TAG_DONE, 0UL}
    };


    EnterFunc(bug("    init_serialunitclass(csd=%p)\n", csd));

    cl = OOP_NewObject(NULL, CLID_HiddMeta, tags);
    D(bug("Class=%p\n", cl));
    if(cl)
    {
	if (OOP_ObtainAttrBases(attrbases))
	{
            D(bug("SerialUnit Class ok\n"));
            cl->UserData = (APTR)csd;

            OOP_AddClass(cl);
	} else {
	    free_serialunitclass(csd);
	    cl = NULL;
	}
    }

    ReturnPtr("init_serialunitclass", OOP_Class *, cl);
}


void free_serialunitclass(struct class_static_data *csd)
{
    EnterFunc(bug("free_serialhiddclass(csd=%p)\n", csd));

    if(csd)
    {
        OOP_RemoveClass(csd->serialhiddclass);
	
        if(csd->serialhiddclass) OOP_DisposeObject((OOP_Object *) csd->serialhiddclass);
        csd->serialhiddclass = NULL;
    }

    ReturnVoid("free_serialhiddclass");
}


/**************************************************************/

void settermios(struct HIDDSerialUnitData * data)
{

  struct termios _termios;
  tcgetattr(data->filedescriptor, &_termios);

  _termios.c_cflag &= ~CSIZE;
  _termios.c_cflag |= data->datalength;

  if (FALSE == data->parity)
    _termios.c_cflag &= ~(PARENB|PARODD);
  else
  {
    _termios.c_cflag |= PARENB;
    switch (data->paritytype)
    {
      case PARITY_EVEN:
        _termios.c_cflag &= ~PARODD;
      break;
      
      case PARITY_ODD:
        _termios.c_cflag |= PARODD;
      break;
    }
  }

  if (tcsetattr(data->filedescriptor, TCSADRAIN, &_termios) < 0)
  {
//    D(bug("Failed to set new termios\n"));
  }
  else
  {
//    D(bug("Adjusted to new termios!\n"));
  }

}
