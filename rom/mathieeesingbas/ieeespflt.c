/*
    (C) 1995-97 AROS - The Amiga Replacement OS
    $Id$

    Desc:
    Lang:
*/

#include <libraries/mathieeesp.h>
#include <aros/libcall.h>
#include <proto/mathieeesingbas.h>
#include <proto/exec.h>
#include <exec/types.h>
#include "mathieeesp_intern.h"

/*****************************************************************************

    NAME */

        AROS_LH1(LONG, IEEESPFlt,

/*  SYNOPSIS */
        AROS_LHA(LONG, y, D0),

/*  LOCATION */
        struct MathIeeeSingBasBase *, MathIeeeSingBasBase, 6, Mathieeespbas)

/*  FUNCTION


    INPUTS
        y  - signed integer number

    RESULT


        Flags:
          zero     : result is zero
          negative : result is negative
          overflow : ffp is not exactly the integer

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO


    INTERNALS

    HISTORY

******************************************************************************/

{
  LONG Exponent = 0;
  LONG TestMask = 0xFFFFFFFF;
  LONG Res = 0;

  if (y == 0)
  {
    SetSR(Zero_Bit, Zero_Bit | Negative_Bit | Overflow_Bit);
    return 0;
  }

  if (y < 0)
  {
    Res = IEEESPSign_Mask;
    y= -y;
  }
  /* find out which is the number of the highest set bit */
  while (TestMask & y)
  {
    Exponent ++;
    TestMask <<= 1;
  }

  if (Exponent >= 26)
    y >>= (Exponent - 25) & IEEESPMantisse_Mask;
  else
    y <<= (25 - Exponent) & IEEESPMantisse_Mask;

  if ((char) (y & 1) != 0)
  {
    y ++;
    if (0x02000000 == y)
      Exponent++;
  }

  y >>= 1;
  y &= IEEESPMantisse_Mask;

  Exponent += 0x7E;

  /* adapt Exponent to IEEESP-Format */
  Exponent <<= 23;
  Res |= y | Exponent;
  if (Res < 0)
    SetSR(Negative_Bit, Zero_Bit | Negative_Bit | Overflow_Bit);

  /* The resulting IEEESP is lacking precision */
  if (Exponent > 0x4c800000)
    SetSR(Overflow_Bit, Zero_Bit | Negative_Bit | Overflow_Bit);

  return Res;
} /* IEEESPFlt */

