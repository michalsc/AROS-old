/*
    (C) 1998 AROS - The Amiga Research OS
    $Id$

    Desc: Serial Unit hidd class implementation.
    Lang: english
*/

/*
  Right now I am assuming that there is a 1.8432 MHZ crystal connected to
  the 16550 UART.
*/

#define AROS_ALMOST_COMPATIBLE 1

/* the rest are Amiga includes */
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
#include <hidd/irq.h>

#include <devices/serial.h>

#include "serial_intern.h"

#undef  SDEBUG
#undef  DEBUG
#define SDEBUG 0
#define DEBUG 0
#include <aros/debug.h>

/* The speed of the crystal */
#define CRYSTAL_SPEED 	1842000 


void serialunit_receive_data();
void serialunit_write_more_data();

unsigned char get_lcr(struct HIDDSerialUnitData * data);
unsigned char get_fcr(ULONG baudrate);
BOOL set_baudrate(struct HIDDSerialUnitData * data, ULONG speed);


inline void outb(unsigned char value, unsigned short port)
{
  __asm__ __volatile__ ("outb %b0,%w1" : : "a" (value), "Nd"(port));
}

inline void outb_p(unsigned char value, unsigned short port)
{
  __asm__ __volatile__ ("outb %b0,%w1 \noutb %%al,$0x80" : : "a" (value), "Nd" (port));
}

inline unsigned char inb(unsigned short port)
{
  unsigned char _v;
  __asm__ __volatile__ ("inb %w1,%b0" : "=a" (_v) : "Nd" (port) );
  return _v;
}

inline unsigned char inb_p(unsigned short port)
{
  unsigned char _v;
  __asm__ __volatile__ ("inb %w1,%b0 \noutb %%al,$0x80" : "=a" (_v) : "Nd" (port) );
  return _v;
}

static inline void serial_out(struct HIDDSerialUnitData * data, 
                              int offset, 
                              int value)
{
  outb(value, data->baseaddr+offset);
}

static inline void serial_outp(struct HIDDSerialUnitData * data, 
                               int offset, 
                               int value)
{
  outb_p(value, data->baseaddr+offset);
}

static inline unsigned int serial_in(struct HIDDSerialUnitData * data,
                                     int offset)
{
  return inb(data->baseaddr+offset);
}

static inline unsigned int serial_inp(struct HIDDSerialUnitData * data,
                                      int offset)
{
  return inb_p(data->baseaddr+offset);
}



/*************************** Classes *****************************/

/* IO bases for every COM port */
ULONG bases[] = { 0x3f8, 0x2f8, 0x3e8, 0x2e8 };

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
  
  obj = (OOP_Object *)OOP_DoSuperMethod(cl, obj, (OOP_Msg)msg);

  if (obj)
  {
    data = OOP_INST_DATA(cl, obj);
    
    data->baseaddr = bases[unitnum];
    
    data->datalength = 8;
    data->parity     = FALSE;
    data->baudrate   = 0; /* will be initialize in set_baudrate() */
    data->unitnum    = unitnum;

    CSD(cl->UserData)->units[data->unitnum] = data;

    D(bug("Unit %d at 0x0%x\n", data->unitnum, data->baseaddr));

    /* Wake up UART */
    serial_outp(data, UART_LCR, 0xBF);
    serial_outp(data, UART_EFR, UART_EFR_ECB);
    serial_outp(data, UART_IER, 0);
    serial_outp(data, UART_EFR, 0);
    serial_outp(data, UART_LCR, 0);

    /* clear the FIFOs */
    serial_outp(data, UART_FCR, (UART_FCR_CLEAR_RCVR | UART_FCR_CLEAR_XMIT));
    
    /* clear the interrupt registers */
    (void)serial_inp(data, UART_RX);
    (void)serial_inp(data, UART_IIR);
    (void)serial_inp(data, UART_MSR);

    /* initilize the UART */
    serial_outp(data, UART_LCR, get_lcr(data));
     
    serial_outp(data, UART_MCR, 8);
    serial_outp(data, UART_IER, UART_IER_RDI);
     
    /* clear the interrupt registers again ... */
    (void)serial_inp(data, UART_LSR);
    (void)serial_inp(data, UART_RX);
    (void)serial_inp(data, UART_IIR);
    (void)serial_inp(data, UART_MSR);
     
     set_baudrate(data, SER_DEFAULT_BAUDRATE);
  } /* if (obj) */

  ReturnPtr("SerialUnit::New()", OOP_Object *, obj);
}

/******* SerialUnit::Dispose() ***********************************/
static OOP_Object *serialunit_dispose(OOP_Class *cl, OOP_Object *obj, OOP_Msg msg)
{
  struct HIDDSerialUnitData * data;
  EnterFunc(bug("SerialUnit::Dispose()\n"));

  data = OOP_INST_DATA(cl, obj);

  CSD(cl->UserData)->units[data->unitnum] = NULL;

  /* stop all interrupts */
  serial_outp(data, UART_IER, 0);

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
  unsigned char status;
  ULONG len = msg->Length;
  ULONG count = 0;
  
  EnterFunc(bug("SerialUnit::Write()\n"));

  status = serial_inp(data, UART_LSR);
  
  if (status & UART_LSR_THRE)
  {
    /* write data into FIFO */
    do
    {
      serial_outp(data, UART_TX, msg->Outbuffer[count++]);
      len--;
    } while (len > 0 && serial_inp(data, UART_LSR & UART_LSR_TEMT));
  }

  ReturnInt("SerialUnit::Write()",ULONG, count);
}

/***************************************************************************/

static ULONG valid_baudrates[] =
{
  2 | LIMIT_LOWER_BOUND,
  115200 | LIMIT_UPPER_BOUND,
  -1
};


/******* SerialUnit::SetBaudrate() **********************************/
BOOL serialunit_setbaudrate(OOP_Class *cl, OOP_Object *o, struct pHidd_SerialUnit_SetBaudrate *msg)
{
  struct HIDDSerialUnitData * data = OOP_INST_DATA(cl, o);
  BOOL valid = FALSE;
  
  if (msg->baudrate != data->baudrate)
  {
    valid = set_baudrate(data, msg->baudrate);
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

/******* SerialUnit::SetParameters() **********************************/
BOOL serialunit_setparameters(OOP_Class *cl, OOP_Object *o, struct pHidd_SerialUnit_SetParameters *msg)
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
        if (tags[i].ti_Data >= 5 && tags[i].ti_Data <= 8)
          data->datalength = tags[i].ti_Data;
        else
          valid = FALSE;
      break;
      
      case TAG_STOP_BITS:
        if (16 == tags[i].ti_Data ||
            32 == tags[i].ti_Data ||
            24 == tags[i].ti_Data)
          data->stopbits = tags[i].ti_Data;
        else
          valid = FALSE;            
      break;

      case TAG_PARITY:
        if (PARITY_0    == tags[i].ti_Data ||
            PARITY_1    == tags[i].ti_Data ||
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

      case TAG_SET_MCR:
	serial_outp(data, UART_MCR, (tags[i].ti_Data & 0x0f) | 0x08);
      break;
      
      default:
        valid = FALSE;
    }
    i++;
  }
  
  serial_outp(data, UART_LCR, get_lcr(data));

  return valid;
}

/******* SerialUnit::SendBreak() **********************************/
BYTE serialunit_sendbreak(OOP_Class *cl, OOP_Object *o, struct pHidd_SerialUnit_SendBreak *msg)
{
  struct HIDDSerialUnitData * data = OOP_INST_DATA(cl, o);
  
  return SerErr_LineErr;
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
  struct HIDDSerialUnitData * data = iD;
  int len = 0;
  UBYTE buffer[READBUFFER_SIZE];

  /*
  ** Read the data from the port ...
  */
  do
  {
    buffer[len++] = serial_inp(data, UART_RX);
  }
  while (serial_inp(data, UART_LSR) & UART_LSR_DR);
  
  /*
  ** ... and deliver them to whoever is interested. 
  */

  if (NULL != data->DataReceivedCallBack)
    data->DataReceivedCallBack(buffer, len, data->unitnum, data->DataReceivedUserData);
}

AROS_UFH3(void, serialunit_write_more_data,
   AROS_UFHA(APTR, iD, A1),
   AROS_UFHA(APTR, iC, A5),
   AROS_UFHA(struct ExecBase *, SysBase, A6))
{
  struct HIDDSerialUnitData * data = iD;

  /*
  ** Ask for more data be written to the unit
  */
  D(bug("Asking for more data to be written to unit %d\n",data->unitnum));

  if (NULL != data->DataWriteCallBack)
    data->DataWriteCallBack(data->unitnum, data->DataWriteUserData);
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


/* some help routines */

unsigned char get_lcr(struct HIDDSerialUnitData * data)
{
  char lcr;
  switch (data->datalength)
  {
    case 5: lcr = 0;
    break;
    
    case 6: lcr = 1;
    break;
    
    case 7: lcr = 2;
    break;
    
    case 8: lcr = 3;
    break;
  
    default: lcr = 0;
  }
  
  switch (data->stopbits)
  {
    case 16: /* 1 stopbit */
      /* nothing to do */
    break;
    
    case 24: /* 1.5 stopbits */
      if (data->datalength == 5)
        lcr |= (1 << 2);
    break;
    
    case 32: /* 2 stopbits */
      if (data->datalength >= 6 && data->datalength <= 8)
        lcr |= (1 << 2);
    break;
    
    default:
  }
  
  if (TRUE == data->parity)
  {
    lcr |= (1 << 3);
  
    switch (data->paritytype)
    {
      case PARITY_EVEN:
        lcr |= (1 << 4);
      break;
      
      case PARITY_1:
        lcr |= (1 << 5);
      break;
      
      case PARITY_0:
        lcr |= (1 << 4) | (1 << 5);
      break;
    
    }
  }
  
  if (TRUE == data->breakcontrol)
    lcr |= (1 << 6);
  return lcr;
}

unsigned char get_fcr(ULONG baudrate)
{
  unsigned char fcr;
  fcr = (1 << 0);
  
  /* 
    Depending on the baudrate set the fifo interrupt threshold to a
    different value.
  */
   
  if (baudrate < 1200)
    fcr |= (3 << 6);
  else
  if (baudrate < 9600)
    fcr |= (2 << 6);
  else
  if (baudrate < 38400)
    fcr |= (1 << 6);
  
  return fcr;
}

BOOL set_baudrate(struct HIDDSerialUnitData * data, ULONG speed)
{
    int quot;
    
    if (!(speed >= 50 && speed <= 115200))
      return FALSE;
    
    quot = CRYSTAL_SPEED / (speed << 4);
    
    /* set the speed on the UART now */
    serial_outp(data, UART_LCR, get_lcr(data) | UART_LCR_DLAB);
    serial_outp(data, UART_DLL, quot & 0xff);
    serial_outp(data, UART_DLM, quot >> 8);
    serial_outp(data, UART_LCR, get_lcr(data));
    serial_outp(data, UART_FCR, get_fcr(speed));
    
    return TRUE;
}

/* Serial interrupts */

#undef SysBase
#define SysBase (hw->sysBase)
#define csd ((struct class_static_data *)(irq->h_Data))

void serial_int_13(HIDDT_IRQ_Handler *irq, HIDDT_IRQ_HwInfo *hw)
{
    UBYTE code;

    code = 1;

    if (csd->units[0])
	code = serial_inp(csd->units[0], UART_IIR) & 0x07;

    switch (code)
    {
	case UART_IIR_RLSI:
	    (void)serial_inp(csd->units[0], UART_LSR);
	    break;
	case UART_IIR_RDI:
	    if (csd->units[0]) serialunit_receive_data(csd->units[0],NULL,SysBase);
	    break;
	case UART_IIR_MSI:
	    (void)serial_inp(csd->units[0], UART_MSR);
	    break;
    }	

    code = 1;
    if (csd->units[2])
	code = serial_inp(csd->units[2], UART_IIR) & 0x07;
    switch (code)
    {
	case UART_IIR_RLSI:
	    (void)serial_inp(csd->units[2], UART_LSR);
	    break;
	case UART_IIR_RDI:
	    if (csd->units[2]) serialunit_receive_data(csd->units[2],NULL,SysBase);
	    break;
	case UART_IIR_MSI:
	    (void)serial_inp(csd->units[2], UART_MSR);
	    break;
    }	

    return;
}

void serial_int_24(HIDDT_IRQ_Handler * irq, HIDDT_IRQ_HwInfo *hw)
{
    UBYTE code;

    code = 1;
    if (csd->units[1])
	code = serial_inp(csd->units[1], UART_IIR) & 0x07;
    switch (code)
    {
	case UART_IIR_RLSI:
	    (void)serial_inp(csd->units[1], UART_LSR);
	    break;
	case UART_IIR_RDI:
	    if (csd->units[1]) serialunit_receive_data(csd->units[1],NULL,SysBase);
	    break;
	case UART_IIR_MSI:
	    (void)serial_inp(csd->units[1], UART_MSR);
	    break;
    }

    code = 1;
    if (csd->units[3])
	code = serial_inp(csd->units[3], UART_IIR) & 0x07;
    switch (code)
    {
	case UART_IIR_RLSI:
	    (void)serial_inp(csd->units[3], UART_LSR);
	    break;
	case UART_IIR_RDI:
	    if (csd->units[3]) serialunit_receive_data(csd->units[3],NULL,SysBase);
	    break;
	case UART_IIR_MSI:
	    (void)serial_inp(csd->units[3], UART_MSR);
	    break;
    }	

    return;
}
