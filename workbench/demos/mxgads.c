/** testmxgads.c
*
*   This program gives a demonstration of how to make mutual exclusion
*   of boolean gadgets work even if they contain GadgetText.
*   The method is still somewhat of a cludge, but then, it will last
*   until 1.3 comes out... Hope this proves of some use.
* 
*                                       Willy.
*
*   The usual disclaimer:
*
*               Copyright 1986,  W.G.J. Langeveld
*
*   Permission is granted for unlimited commercial and/or noncommercial
*   use. This program  may  not  be uploaded  to  any   commercial  BBS
*   except Byte Information Exchange. The author accepts no responsibi-
*   lity for this program whatsoever.
*
**/


#include "exec/types.h"
#include "exec/io.h"
#include "exec/memory.h"
#include "dos/dos.h"
#include "intuition/intuition.h"

//#include "functions.h"
#include "intuition/iobsolete.h"
#include "proto/exec.h"
#include "proto/intuition.h"

#include <stdio.h>
#include <stdlib.h>

#define SHORT WORD
void Test(void);



/* The following source fragment was generated by PowerWindows */

WORD BorderVectors1[] = {0,0,48,0,48,14,0,14,0,0};
struct Border Border1 = {
   -2,-1,
   3,0,JAM1,
   5,
   BorderVectors1,
   NULL
};

struct IntuiText IText1 = {
   1,0,JAM2,
   6,3,
   NULL,
   (UBYTE *)"Quit",
   NULL
};

struct Gadget Gadg5 = {
   NULL,
   89,51,
   45,13,
   GADGHCOMP,
   RELVERIFY,
   BOOLGADGET,
   (APTR)&Border1,
   NULL,
   &IText1,
   0,
   NULL,
   7,
   NULL
};

SHORT BorderVectors2[] = {0,0,48,0,48,14,0,14,0,0};
struct Border Border2 = {
   -2,-1,
   3,0,JAM1,
   5,
   BorderVectors2,
   NULL
};

struct IntuiText IText2 = {
   3,0,INVERSVID,
   7,3,
   NULL,
   (UBYTE *)"Four",
   NULL
};

struct Gadget Gadg4 = {
   &Gadg5,
   166,18,
   45,13,
   GADGHCOMP,
   GADGIMMEDIATE,
   BOOLGADGET,
   (APTR)&Border2,
   NULL,
   &IText2,
   0,
   NULL,
   6,
   NULL
};

SHORT BorderVectors3[] = {0,0,48,0,48,14,0,14,0,0};
struct Border Border3 = {
   -2,-1,
   3,0,JAM1,
   5,
   BorderVectors3,
   NULL
};

struct IntuiText IText3 = {
   2,0,INVERSVID,
   2,3,
   NULL,
   (UBYTE *)"Three",
   NULL
};

struct Gadget Gadg3 = {
   &Gadg4,
   114,18,
   45,13,
   GADGHCOMP,
   GADGIMMEDIATE,
   BOOLGADGET,
   (APTR)&Border3,
   NULL,
   &IText3,
   0,
   NULL,
   5,
   NULL
};

SHORT BorderVectors4[] = {0,0,48,0,48,14,0,14,0,0};
struct Border Border4 = {
   -2,-1,
   3,0,JAM1,
   5,
   BorderVectors4,
   NULL
};

struct IntuiText IText4 = {
   1,0,JAM1,
   9,3,
   NULL,
   (UBYTE *)"One",
   NULL
};

struct Gadget Gadg1 = {
   &Gadg3,
   10,18,
   45,13,
   GADGHCOMP+SELECTED,
   GADGIMMEDIATE,
   BOOLGADGET,
   (APTR)&Border4,
   NULL,
   &IText4,
   0,
   NULL,
   4,
   NULL
};

SHORT BorderVectors5[] = {0,0,48,0,48,14,0,14,0,0};
struct Border Border5 = {
   -2,-1,
   3,0,JAM1,
   5,
   BorderVectors5,
   NULL
};

struct IntuiText IText5 = {
   3,0,JAM2,
   8,3,
   NULL,
   (UBYTE *)"Two",
   NULL
};

struct Gadget Gadg2 = {
   &Gadg1,
   62,18,
   45,13,
   GADGHCOMP,
   GADGIMMEDIATE,
   BOOLGADGET,
   (APTR)&Border5,
   NULL,
   &IText5,
   0,
   NULL,
   3,
   NULL
};

struct NewWindow NewWindowStructure = {
   351,39,
   220,70,
   0,1,
   GADGETDOWN+GADGETUP+CLOSEWINDOW,
   WINDOWDRAG+WINDOWCLOSE+ACTIVATE,
   &Gadg2,
   NULL,
   (UBYTE *)"Mutual Exclude Test",
   NULL,
   NULL,
   5,5,
   640,200,
   WBENCHSCREEN
};

/* end of PowerWindows source generation */
 
#define GAD1 4
#define GAD2 3
#define GAD3 5
#define GAD4 6
#define GAD5 7

struct IntuitionBase *IntuitionBase = 0L;

struct Window *twindow;
void GadMXSel(), GadMXSD();

int main(void)
{
   IntuitionBase = (struct IntuitionBase *) 
                    OpenLibrary("intuition.library", 0L);
   if (IntuitionBase == NULL) {
      printf("Failed to open intuition\n");
      exit(0);
   }

   Test();  /* Do the work: put up a "requester" */

   if (IntuitionBase != NULL) CloseLibrary((struct Library *)IntuitionBase);
   return(0);
}

/**
*
*   Routine to put up the "Requester" and handle IDCMP messages.
*
**/
void Test(void)
{
   static ULONG class;

   static struct IntuiMessage *message;
   static struct Gadget *gad;
   static int looping;

   twindow = OpenWindow(&NewWindowStructure); /* open the "Requester" */
   if ( twindow == NULL ) {
      printf ("open window failed\n");
      return;
   }

   looping = TRUE;
   while (looping) {
      WaitPort(twindow->UserPort);
      while((message = (struct IntuiMessage *)
                       GetMsg(twindow->UserPort)) != NULL) {
         class = message->Class;
         gad = (struct Gadget *)message->IAddress;
         ReplyMsg((struct Message *)message);

         if (class == CLOSEWINDOW) looping = FALSE;

         if ((class == GADGETUP) || (class == GADGETDOWN)) {
            switch (gad->GadgetID) {
               case GAD5 :
                  printf("QUIT\n");
                  looping = FALSE;
                  break;
               case GAD1 :
                  printf("ONE\n");
                  GadMXSel(twindow,&Gadg1,&Gadg2,&Gadg3,&Gadg4,NULL,NULL);
                  break;
               case GAD2 :
                  printf("TWO\n");
                  GadMXSel(twindow,&Gadg2,&Gadg1,&Gadg3,&Gadg4,NULL,NULL);
                  break;
               case GAD3 :
                  printf("THREE\n");
                  GadMXSel(twindow,&Gadg3,&Gadg1,&Gadg2,&Gadg4,NULL,NULL);
                  break;
               case GAD4 :
                  printf("FOUR\n");
                  GadMXSel(twindow,&Gadg4,&Gadg1,&Gadg2,&Gadg3,NULL,NULL);
                  break;
            }
         }
      }
   }
   CloseWindow(twindow);
   return;
}

/**
*
*   This routine selects gad1 and deselects the previously selected one.
*   Maximum number of gadgets to be mutually excluded is six, obviously.
*   Extension to more gadgets is obvious, too. This routine assumes that
*   only one gadget is selected at a time.
*
**/
void GadMXSel(win,gad1,gad2,gad3,gad4,gad5,gad6)
struct Window *win;
struct Gadget *gad1,*gad2,*gad3,*gad4,*gad5,*gad6;
{
   static struct Gadget *gadprev;

   gadprev = NULL;
   if (gad1 != NULL)
      if (gad1->Flags & SELECTED) return;
   if (gad2 != NULL)
      if (gad2->Flags & SELECTED) gadprev = gad2; 
   if (gad3 != NULL)
      if (gad3->Flags & SELECTED) gadprev = gad3;
   if (gad4 != NULL)
      if (gad4->Flags & SELECTED) gadprev = gad4;
   if (gad5 != NULL)
      if (gad5->Flags & SELECTED) gadprev = gad5;
   if (gad6 != NULL) 
      if (gad6->Flags & SELECTED) gadprev = gad6;

   GadMXSD(win,gad1,gadprev);

   return;
}

/**
*
*   This routine selects gad1 and deselects gad2.
*   Notice, that this version removes gadgets from the gadget list and adds
*   them to the end. If you're sensitive to the location of the gadgets
*   in the gadgetlist, you have to use: gadloc = RemoveGadget... and
*   AddGadget(...,...,(long) gadloc), where gadloc is a USHORT. Then you
*   have to refresh all gadgets to make sure the two you changed get
*   refreshed.
*
**/
void GadMXSD(win,gad1,gad2)
struct Window *win;
struct Gadget *gad1,*gad2;
{
/*
*   First select gad2 (yes!) and refresh.
*/
   if (gad2 != NULL) {
      RemoveGadget(win,gad2);
      gad2->Flags |= SELECTED;
      AddGadget(win,gad2,-1L);
   }

   if (gad2 != NULL) RefreshGadgets(gad2,win,NULL);
/*
*   Now select gad1 and deselect gad2 and refresh.
*/
   if (gad1 != NULL) {
      RemoveGadget(win,gad1);
      gad1->Flags |= SELECTED;
      AddGadget(win,gad1,-1L);
   }

   if (gad2 != NULL) {
      RemoveGadget(win,gad2);
      gad2->Flags &= ~SELECTED;
      AddGadget(win,gad2,-1L);
   }

   if (gad1 != NULL) RefreshGadgets(gad1,win,NULL);

   return;
}

