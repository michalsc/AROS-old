/*
    Copyright � 2013, The AROS Development Team. All rights reserved.
    $Id$
*/

#ifndef USB2OTG_H
#define USB2OTG_H

/** Maximum number of Periodic FIFOs */

#define PERIFIFO_MAX                                    15                      // Max no. of Periodic FIFO's
#define TXFIFO_MAX                                      15                      // Max no. of Tx FIFO's
#define EPSCHANS_MAX                                    16                      // Max no. of Endpoints/Host Chan's

#define USB2OTG_BASE                                    (ARM_PERIIOBASE + 0x980000)

#define USB2OTG_OTGCTRL                                 (USB2OTG_BASE + 0x0000)
#define USB2OTG_OTGINTR                                 (USB2OTG_BASE + 0x0004)
#define USB2OTG_AHB                                     (USB2OTG_BASE + 0x0008)
#define USB2OTG_USB                                     (USB2OTG_BASE + 0x000c)
#define USB2OTG_RESET                                   (USB2OTG_BASE + 0x0010)
#define USB2OTG_INTR                                    (USB2OTG_BASE + 0x0014)
#define USB2OTG_INTRMASK                                (USB2OTG_BASE + 0x0018)
#define USB2OTG_RCVPEEK                                 (USB2OTG_BASE + 0x001c)
#define USB2OTG_RCVPOP                                  (USB2OTG_BASE + 0x0020)
#define USB2OTG_RCVSIZE                                 (USB2OTG_BASE + 0x0024)
#define USB2OTG_NONPERIFIFOSIZE                         (USB2OTG_BASE + 0x0028)
#define USB2OTG_NONPERIFIFOSTATUS                       (USB2OTG_BASE + 0x002c)
#define USB2OTG_I2CCTRL                                 (USB2OTG_BASE + 0x0030)
#define USB2OTG_PHYVENDCTRL                             (USB2OTG_BASE + 0x0034)
#define USB2OTG_GPIO                                    (USB2OTG_BASE + 0x0038)
#define USB2OTG_USERID                                  (USB2OTG_BASE + 0x003c)
#define USB2OTG_VENDORID                                (USB2OTG_BASE + 0x0040)
#define USB2OTG_HARDWARE                                (USB2OTG_BASE + 0x0044)
#define USB2OTG_HARDWARE2                               (USB2OTG_BASE + 0x0048)
#define USB2OTG_HARDWARE3                               (USB2OTG_BASE + 0x004c)
#define USB2OTG_HARDWARE4                               (USB2OTG_BASE + 0x0050)
#define USB2OTG_LPMCONFIG                               (USB2OTG_BASE + 0x0054)
#define USB2OTG_MDIOCTRL                                (USB2OTG_BASE + 0x0080)
#define USB2OTG_MDIOGEN                                 (USB2OTG_BASE + 0x0084)
#define USB2OTG_MDIOREAD                                USB2OTG_MDIOGEN
#define USB2OTG_MDIOWRITE                               USB2OTG_MDIOGEN
#define USB2OTG_MISCCTRL                                (USB2OTG_BASE + 0x0088)
#define USB2OTG_PERIFIFOSIZE                            (USB2OTG_BASE + 0x0100)
#define USB2OTG_PERIFIFOBASE                            (USB2OTG_BASE + 0x0104)

#define USB2OTG_HOSTCFG                                 (USB2OTG_BASE + 0x0400)
#define USB2OTG_HOSTFRAMEINTERV                         (USB2OTG_BASE + 0x0404)
#define USB2OTG_HOSTFRAMENO                             (USB2OTG_BASE + 0x0408)
#define USB2OTG_HOSTFIFOSTATUS                          (USB2OTG_BASE + 0x0410)
#define USB2OTG_HOSTINTR                                (USB2OTG_BASE + 0x0414)
#define USB2OTG_HOSTINTRMASK                            (USB2OTG_BASE + 0x0418)
#define USB2OTG_HOSTFRAMELST                            (USB2OTG_BASE + 0x041c)
#define USB2OTG_HOSTPORT                                (USB2OTG_BASE + 0x0440)

#define USB2OTG_HOSTPORT_PRTCONNSTS                     (1 << 0)
#define USB2OTG_HOSTPORT_PRTCONNDET                     (1 << 1)
#define USB2OTG_HOSTPORT_PRTENA                         (1 << 2)
#define USB2OTG_HOSTPORT_PRTENCHNG                      (1 << 3)
#define USB2OTG_HOSTPORT_PRTOVRCURRACT                  (1 << 4)
#define USB2OTG_HOSTPORT_PRTOVRCURRCHNG                 (1 << 5)
#define USB2OTG_HOSTPORT_PRTRES                         (1 << 6)
#define USB2OTG_HOSTPORT_PRTSUSP                        (1 << 7)
#define USB2OTG_HOSTPORT_PRTRST                         (1 << 8)
#define USB2OTG_HOSTPORT_RESERVED9                      (1 << 9)
#define USB2OTG_HOSTPORT_PRTLNSTS                       (1 << 10)
#define USB2OTG_HOSTPORT_PRTPWR                         (1 << 12)
#define USB2OTG_HOSTPORT_PRTTSTCTL                      (1 << 13)
#define USB2OTG_HOSTPORT_PRTSPD_HIGH                    (0 << 17)
#define USB2OTG_HOSTPORT_PRTSPD_FULL                    (1 << 17)
#define USB2OTG_HOSTPORT_PRTSPD_LOW                     (2 << 17)
#define USB2OTG_HOSTPORT_SC_BITS                        0x2e

#define USB2OTG_HOST_CHANBASE                           (USB2OTG_BASE + 0x0500)
#define USB2OTG_HOSTCHAN_CHARBASE                       (0x00)
#define USB2OTG_HOSTCHAN_SPLITCTRL                      (0x04)
#define USB2OTG_HOSTCHAN_INTR                           (0x08)
#define USB2OTG_HOSTCHAN_INTRMASK                       (0x0c)
#define USB2OTG_HOSTCHAN_TRANSSIZE                      (0x10)
#define USB2OTG_HOSTCHAN_DMAADDR                        (0x14)
#define USB2OTG_HOSTCHAN_DMABUFF                        (0x1c)
#define USB2OTG_HOST_CHANREGSIZE                        (0x20)

#define USB2OTG_HOSTCHAR_MAXPACKETSIZE(x)               ((x) & 1023)
#define USB2OTG_HOSTCHAR_EPNO(x)                        (((x) & 0x0f) << 11)
#define USB2OTG_HOSTCHAR_EPDIR(x)                       (((x) & 1) << 15)
#define USB2OTG_HOSTCHAR_ADDR(x)                        (((x) & 0x7f) << 22)
#define USB2OTG_HOSTCHAR_EC(x)                          (((x) & 3) << 20)
#define USB2OTG_HOSTCHAR_DISABLE                        (1 << 30)
#define USB2OTG_HOSTCHAR_ENABLE                         (1 << 31)
#define USB2OTG_HOSTCHAR_EPTYPE(x)                      (((x) & 3) << 18)
#define USB2OTG_HOSTCHAR_LOWSPEED                       (1 << 17)

#define USB2OTG_HOSTTSIZE_PID(x)                        (((x) & 3) << 29)
#define USB2OTG_HOSTTSIZE_PKTCNT(x)                     (((x) & 1023) << 19)
#define USB2OTG_HOSTTSIZE_PING                          (1 << 31)
#define USB2OTG_HOSTTSIZE_SIZE(x)                       ((x) & 524287)

#define USB2OTG_PID_DATA0                               0
#define USB2OTG_PID_DATA1                               2
#define USB2OTG_PID_DATA2                               1
#define USB2OTG_PID_MDATA                               3
#define USB2OTG_PID_SETUP                               3

#define USB2OTG_TYPE_CTRL                               0
#define USB2OTG_TYPE_ISO                                1
#define USB2OTG_TYPE_BULK                               2
#define USB2OTG_TYPE_INT                                3

#define USB2OTG_DEVCFG                                  (USB2OTG_BASE + 0x0800)
#define USB2OTG_DEVCTRL                                 (USB2OTG_BASE + 0x0804)
#define USB2OTG_DEVSTATUS                               (USB2OTG_BASE + 0x0808)
#define USB2OTG_DEVINEPMASK                             (USB2OTG_BASE + 0x0810)
#define USB2OTG_DEVOUTEPMASK                            (USB2OTG_BASE + 0x0814)
#define USB2OTG_DEVINTR                                 (USB2OTG_BASE + 0x0818)
#define USB2OTG_DEVINTRMASK                             (USB2OTG_BASE + 0x081c)
#define USB2OTG_DEVINTQRR1                              (USB2OTG_BASE + 0x0820)
#define USB2OTG_DEVINTQRR2                              (USB2OTG_BASE + 0x0824)
#define USB2OTG_DEVVBUSDIS                              (USB2OTG_BASE + 0x0828)
#define USB2OTG_DEVVBUSPULSE                            (USB2OTG_BASE + 0x082c)
#define USB2OTG_DEVINTQ_THRESHCTRL                      (USB2OTG_BASE + 0x0830)
#define USB2OTG_DEVINTQ_FIFOEMASK                       (USB2OTG_BASE + 0x0834)
#define USB2OTG_DEVEACHINT                              (USB2OTG_BASE + 0x0838)
#define USB2OTG_DEVEACHINTMSK                           (USB2OTG_BASE + 0x083c)


#define USB2OTG_DEV_INEP_BASE                           (USB2OTG_BASE + 0x0900)
#define USB2OTG_DEV_INEP_DIEPCTL                        (0x00)
#define USB2OTG_DEV_INEP_DIEPINT                        (0x08)
#define USB2OTG_DEV_INEP_DIEPTSIZ                       (0x10)
#define USB2OTG_DEV_INEP_DIEPDMA                        (0x14)
#define USB2OTG_DEV_INEP_DTXFSTS                        (0x18)
#define USB2OTG_DEV_INEP_DIEPDMAB                       (0x1c)

#define USB2OTG_DEV_OUTEP_BASE                          (USB2OTG_BASE + 0x0B00)
#define USB2OTG_DEV_OUTEP_DOEPCTL                       (0x00)
#define USB2OTG_DEV_OUTEP_DOEPFN                        (0x04)
#define USB2OTG_DEV_OUTEP_DOEPINT                       (0x08)
#define USB2OTG_DEV_OUTEP_DOEPTSIZ                      (0x10)
#define USB2OTG_DEV_OUTEP_DOEPDMA                       (0x14)
#define USB2OTG_DEV_OUTEP_DOEPDMAB                      (0x1c)

#define USB2OTG_DEV_EPSIZE                              (0x20)

#define USB2OTG_POWER                                   (USB2OTG_BASE + 0x0e00)

#define USB2OTG_FIFOBASE                                (USB2OTG_BASE + 0x1000)
#define USB2OTG_FIFOSIZE			        0x1000

/* Bits for USB2OTG_OTGCTRL */
#define USB2OTG_OTGCTRL_SESREQSCS                       (1 << 0)
#define USB2OTG_OTGCTRL_SESREQ                          (1 << 1)
#define USB2OTG_OTGCTRL_VBVALIDOVEN                     (1 << 2)
#define USB2OTG_OTGCTRL_VBVALIDOVVAL                    (1 << 3)
#define USB2OTG_OTGCTRL_AVALIDOVEN                      (1 << 4)
#define USB2OTG_OTGCTRL_AVALIDOVVAL                     (1 << 5)
#define USB2OTG_OTGCTRL_BVALIDOVEN                      (1 << 6)
#define USB2OTG_OTGCTRL_BVALIDOVVAL                     (1 << 7)
#define USB2OTG_OTGCTRL_HSTNEGSCS                       (1 << 8)
#define USB2OTG_OTGCTRL_HNPREQ                          (1 << 9)
#define USB2OTG_OTGCTRL_HOSTSETHNPENABLE                (1 << 10)
#define USB2OTG_OTGCTRL_DEVHNPEN                        (1 << 11)
#define USB2OTG_OTGCTRL_CONIDSTS                        (1 << 16)
#define USB2OTG_OTGCTRL_DBNCTIME                        (1 << 17)
#define USB2OTG_OTGCTRL_ASESSIONVALID                   (1 << 18)
#define USB2OTG_OTGCTRL_BSESSIONVALID                   (1 << 19)
#define USB2OTG_OTGCTRL_OTGVERSION                      (1 << 20)
#define USB2OTG_OTGCTRL_MULTVALIDBC                     (1 << 22)
#define USB2OTG_OTGCTRL_CHIRPEN                         (1 << 27)

/* Bits in USB2OTG_OTGINTR */
#define USB2OTG_OTGINTR_SESSENDDETECTED                 (1 << 2)
#define USB2OTG_OTGINTR_SESSRQSTSUCCESSSTATUSCHANGE     (1 << 8)
#define USB2OTG_OTGINTR_HOSTNEGSUCCESSSTATUSCHANGE      (1 << 9)
#define USB2OTG_OTGINTR_HOSTNEGDETECTED                 (1 << 17)
#define USB2OTG_OTGINTR_ADEVICETIMEOUTCHANGE            (1 << 18)
#define USB2OTG_OTGINTR_DEBOUNCEDONE                    (1 << 19)

/* Bits in USB2OTG_AHB */
#define USB2OTG_AHB_INTENABLE                           (1 << 0)
#define USB2OTG_AHB_AXIBURSTLENGTH                      (1 << 1)
#define USB2OTG_AHB_WAITFORAXIWRITES                    (1 << 4)
#define USB2OTG_AHB_DMAENABLE                           (1 << 5)
#define USB2OTG_AHB_TRANSFEREMPTYLEVEL                  (1 << 7)
#define USB2OTG_AHB_PERIODICTRANSFEREMPTYLEVEL          (1 << 8)
#define USB2OTG_AHB_REMMEMSUPP                          (1 << 21)
#define USB2OTG_AHB_NOTIALLDMAWRIT                      (1 << 22)
#define USB2OTG_AHB_DMAREMAINDERMODE                    23
#define USB2OTG_AHB_DMAREMAINDERMODE_INCR               (0 << 23)
#define USB2OTG_AHB_DMAREMAINDERMODE_SINGLE             (1 << 23)

/* Bits in USB2OTG_USB */
#define USB2OTG_USB_TOUTCAL                             (1 << 0)
#define USB2OTG_USB_PHYINTERFACE                        (1 << 3)
#define USB2OTG_USB_MODESELECT                          4
#define USB2OTG_USB_MODESELECT_ULPI                     (0 << USB2OTG_USB_MODESELECT)
#define USB2OTG_USB_MODESELECT_UTMI                     (1 << USB2OTG_USB_MODESELECT)
#define USB2OTG_USB_FSINTF                              (1 << 5)
#define USB2OTG_USB_PHYSEL                              (1 << 6)
#define USB2OTG_USB_DDRSEL                              (1 << 7)
#define USB2OTG_USB_SRPCAPABLE                          (1 << 8)
#define USB2OTG_USB_HNPCAPABLE                          (1 << 9)
#define USB2OTG_USB_USBTRDTIM(x)                        (x << 10)
#define USB2OTG_USB_PHY_LPM_CLK_SEL                     (1 << 15)
#define USB2OTG_USB_OTGUTMIFSSEL                        (1 << 16)
#define USB2OTG_USB_ULPIFSLS                            (1 << 17)
#define USB2OTG_USB_ULPI_AUTO_RES                       (1 << 18)
#define USB2OTG_USB_ULPI_CLK_SUS_M                      (1 << 19)
#define USB2OTG_USB_ULPIDRIVEEXTERNALVBUS               (1 << 20)
#define USB2OTG_USB_ULPI_INT_VBUS_INDICATOR             (1 << 21)
#define USB2OTG_USB_TSDLINEPULSEENABLE                  (1 << 22)
#define USB2OTG_USB_INDICATOR_COMPLEMENT                (1 << 23)
#define USB2OTG_USB_INDICATOR_PASS_THROUGH              (1 << 24)
#define USB2OTG_USB_ULPI_INT_PROT_DIS                   (1 << 25)
#define USB2OTG_USB_IC_USB_CAPABLE                      (1 << 26)
#define USB2OTG_USB_IC_TRAFFIC_PULL_REMOVE              (1 << 27)
#define USB2OTG_USB_TX_END_DELAY                        (1 << 28)
#define USB2OTG_USB_FORCE_HOST_MODE                     (1 << 29)
#define USB2OTG_USB_FORCE_DEV_MODE                      (1 << 30)

#define USB2OTG_RESET_CORESOFT                          (1 << 0)
#define USB2OTG_RESET_HCLKSOFT                          (1 << 1)
#define USB2OTG_RESET_HOSTFRAMECNTR                     (1 << 2)
#define USB2OTG_RESET_INTOKENQUEUEFLUSH                 (1 << 3)
#define USB2OTG_RESET_RXFIFOFLUSH                       (1 << 4)
#define USB2OTG_RESET_TXFIFOFLUSH                       (1 << 5)

/* Bits in USB2OTG_I2CCTRL */
#define USB2OTG_I2CCTRL_READWRITEDATA                   (1 << 0)
#define USB2OTG_I2CCTRL_REGISTERADDRESS                 (1 << 8)
#define USB2OTG_I2CCTRL_ADDRESS                         (1 << 16)
#define USB2OTG_I2CCTRL_I2CENABLE                       (1 << 23)
#define USB2OTG_I2CCTRL_ACKNOWLEDGE                     (1 << 24)
#define USB2OTG_I2CCTRL_I2CSUSPENDCONTROL               (1 << 25)
#define USB2OTG_I2CCTRL_I2CDEVICEADDRESS                (1 << 26)
#define USB2OTG_I2CCTRL_READWRITE                       (1 << 30)
#define USB2OTG_I2CCTRL_BSYDNE                          (1 << 31)

/* Bits in USB2OTG_LPMCONFIG */
#define USB2OTG_LPMCONFIG_LOWPOWERMODECAPABLE           (1 << 0)
#define USB2OTG_LPMCONFIG_APPLICATIONRESPONSE           (1 << 1)
#define USB2OTG_LPMCONFIG_HOSTINITIATEDRESUMEDURATION   (1 << 2)
#define USB2OTG_LPMCONFIG_REMOTEWAKEUPENABLED           (1 << 6)
#define USB2OTG_LPMCONFIG_UTMISLEEPENABLED              (1 << 7)
#define USB2OTG_LPMCONFIG_HOSTINITRESDURATIONTHRESHOLD  (1 << 8)
#define USB2OTG_LPMCONFIG_LOWPOWERMODERESPONSE          (1 << 13)
#define USB2OTG_LPMCONFIG_PORTSLEEPSTATUS               (1 << 15)
#define USB2OTG_LPMCONFIG_SLEEPSTATERESUMEOK            (1 << 16)
#define USB2OTG_LPMCONFIG_LOWPOWERMODECHANNELINDEX      (1 << 17)
#define USB2OTG_LPMCONFIG_RETRYCOUNT                    (1 << 21)
#define USB2OTG_LPMCONFIG_SENDLOWPOWERMODE              (1 << 24)
#define USB2OTG_LPMCONFIG_RETRYCOUNTSTATUS              (1 << 25)
#define USB2OTG_LPMCONFIG_HSICCONNECT                   (1 << 30)
#define USB2OTG_LPMCONFIG_INVERSESELECTHSIC             (1 << 31)

/* Bits in USB2OTG_MDIOSTRL */
#define USB2OTG_MDIOCTRL_READ                           (1 << 0)
#define USB2OTG_MDIOCTRL_CLOCKRATIO                     (1 << 16)
#define USB2OTG_MDIOCTRL_FREERUN                        (1 << 20)
#define USB2OTG_MDIOCTRL_BITHASHENABLE                  (1 << 21)
#define USB2OTG_MDIOCTRL_MDCWRITE                       (1 << 22)
#define USB2OTG_MDIOCTRL_MDOWRITE                       (1 << 23)
#define USB2OTG_MDIOCTRL_BUSY                           (1 << 31)

/* Bits in USB2OTG_MISCCTRL */
#define USB2OTG_MISCCTRL_SESSIONEND                     (1 << 0)
#define USB2OTG_MISCCTRL_VBUSVALID                      (1 << 1)
#define USB2OTG_MISCCTRL_BSESSIONVALID                  (1 << 2)
#define USB2OTG_MISCCTRL_ASESSIONVALID                  (1 << 3)
#define USB2OTG_MISCCTRL_DISCHARGEVBUS                  (1 << 4)
#define USB2OTG_MISCCTRL_CHARGEVBUS                     (1 << 5)
#define USB2OTG_MISCCTRL_DRIVEVBUS                      (1 << 6)
#define USB2OTG_MISCCTRL_DISABLEDRIVING                 (1 << 7)
#define USB2OTG_MISCCTRL_VBUSIRQENABLED                 (1 << 8)
#define USB2OTG_MISCCTRL_VBUSIRQ                        (1 << 9)
#define USB2OTG_MISCCTRL_AXIPRIORITYLEVEL               (1 << 16)

#define USB2OTG_USBOTGMODE                              (0 << 0)
#define USB2OTG_USBHOSTMODE                             (1 << 0)
#define USB2OTG_USBDEVICEMODE                           (1 << 1)

/* Bits in USB2OTG_INTR */
#define USB2OTG_INTRCORE_CURRENTMODE                    (1 << 0)
#define USB2OTG_INTRCORE_MODEMISMATCH                   (1 << 1)
#define USB2OTG_INTRCORE_OTG                            (1 << 2)
#define USB2OTG_INTRCORE_DMASTARTOFFRAME                (1 << 3)
#define USB2OTG_INTRCORE_RECEIVESTATUSLEVEL             (1 << 4)
#define USB2OTG_INTRCORE_NPTRANSMITFIFOEMPTY            (1 << 5)
#define USB2OTG_INTRCORE_GINNAKEFF                      (1 << 6)
#define USB2OTG_INTRCORE_GOUTNAKEFF                     (1 << 7)
#define USB2OTG_INTRCORE_ULPICK                         (1 << 8)
#define USB2OTG_INTRCORE_I2C                            (1 << 9)
#define USB2OTG_INTRCORE_EARLYSUSPEND                   (1 << 10)
#define USB2OTG_INTRCORE_USBSUSPEND                     (1 << 11)
#define USB2OTG_INTRCORE_USBRESET                       (1 << 12)
#define USB2OTG_INTRCORE_ENUMERATIONDONE                (1 << 13)
#define USB2OTG_INTRCORE_ISOCHRONOUSOUTDROP             (1 << 14)
#define USB2OTG_INTRCORE_EOPFRAME                       (1 << 15)
#define USB2OTG_INTRCORE_RESTOREDONE                    (1 << 16)
#define USB2OTG_INTRCORE_ENDPOINTMISMATCH               (1 << 17)
#define USB2OTG_INTRCORE_INENDPOINT                     (1 << 18)
#define USB2OTG_INTRCORE_OUTENDPOINT                    (1 << 19)
#define USB2OTG_INTRCORE_INCOMPLETEISOCHRONOUSIN        (1 << 20)
#define USB2OTG_INTRCORE_INCOMPLETEISOCHRONOUSOUT       (1 << 21)
#define USB2OTG_INTRCORE_FETSETUP                       (1 << 22)
#define USB2OTG_INTRCORE_RESETDETECT                    (1 << 23)
#define USB2OTG_INTRCORE_PORT                           (1 << 24)
#define USB2OTG_INTRCORE_HOSTCHANNEL                    (1 << 25)
#define USB2OTG_INTRCORE_HPTRANSMITFIFOEMPTY            (1 << 26)
#define USB2OTG_INTRCORE_LPMTRANSRCVD                   (1 << 27)
#define USB2OTG_INTRCORE_CONNECTIONIDSTATUSCHANGE       (1 << 28)
#define USB2OTG_INTRCORE_DISCONNECT                     (1 << 29)
#define USB2OTG_INTRCORE_SESSIONREQUEST                 (1 << 30)
#define USB2OTG_INTRCORE_WAKEUP                         (1 << 31)

/* Channel Interrupt bits */
#define USB2OTG_INTRCHAN_TRANSFERCOMPLETE               (1 << 0)
#define USB2OTG_INTRCHAN_HALT                           (1 << 1)
#define USB2OTG_INTRCHAN_AHBERROR                       (1 << 2)
#define USB2OTG_INTRCHAN_STALL                          (1 << 3)
#define USB2OTG_INTRCHAN_NEGATIVEACKNOWLEDGE            (1 << 4)
#define USB2OTG_INTRCHAN_ACKNOWLEDGE                    (1 << 5)
#define USB2OTG_INTRCHAN_NOTREADY                       (1 << 6)
#define USB2OTG_INTRCHAN_TRANSACTIONERROR               (1 << 7)
#define USB2OTG_INTRCHAN_BABBLEERROR                    (1 << 8)
#define USB2OTG_INTRCHAN_FRAMEOVERRUN                   (1 << 9)
#define USB2OTG_INTRCHAN_DATATOGGLEERROR                (1 << 10)
#define USB2OTG_INTRCHAN_BUFFERNOTAVAILABLE             (1 << 11)
#define USB2OTG_INTRCHAN_EXCESSIVETRANSMISSION          (1 << 12)
#define USB2OTG_INTRCHAN_FRAMELISTROLLOVER              (1 << 13)

/* Bits in the power register */
#define USB2OTG_POWER_STOPPCLOCK                        (1 << 0)
#define USB2OTG_POWER_GATEHCLOCK                        (1 << 1)
#define USB2OTG_POWER_POWERCLAMP                        (1 << 2)
#define USB2OTG_POWER_POWERDOWNMODULES                  (1 << 3)
#define USB2OTG_POWER_PHYSUSPENDED                      (1 << 4)
#define USB2OTG_POWER_ENABLESLEEPCLOCKGATING            (1 << 5)
#define USB2OTG_POWER_PHYSLEEPING                       (1 << 6)
#define USB2OTG_POWER_DEEPSLEEP                         (1 << 7)

/* Some useful macros */
#define USB2OTG_CHANNEL_REG(chan, reg)  ((USB2OTG_HOST_CHANBASE) + (0x20 * (chan)) + USB2OTG_HOSTCHAN_ ## reg)

#endif	/* USB2OTG_H */
