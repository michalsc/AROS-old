#include <aros/libcall.h>
#include <proto/exec.h>

#include "partition_intern.h"
#include "partition_support.h"

/*****************************************************************************

    NAME */
        AROS_LH4(LONG, ReadPartitionData,

/*  SYNOPSIS */
        AROS_LHA(UQUAD                   , StartBlock, D0), /* FIXME: Should be D0/D1 */
        AROS_LHA(ULONG                   , DataSize  , D2),
        AROS_LHA(struct PartitionHandle *, Partition , A0),
        AROS_LHA(APTR                    , Buffer    , A1),

/*  LOCATION */
        struct Library *, PartitionBase, 25, Partition)

/*  FUNCTION
        Read raw data from the partition.

    INPUTS
        StartBlock - Number of the first block to start reading from.
        DataSize   - Size of data to read in bytes. This size must be a multiple of block size,
                     in order to ensure correct operation
        Partition  - a handle to a partition to read from
        Buffer     - a pointer to a data buffer

    RESULT
        A return code of DoIO() function which was used to read the data

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    /*
     * FIXME: After UQUAD issue is fixed on m68k, throw away old duplicating code
     * from partition_support.c.
     * Doing this right now will break large disks support on m68k.
     */
    
    UQUAD offset = (getStartBlock(Partition) + StartBlock) * (Partition->de.de_SizeBlock << 2);
    struct IOExtTD *ioreq = Partition->bd->ioreq;

    ioreq->iotd_Req.io_Command = Partition->bd->cmdread;
    ioreq->iotd_Req.io_Length  = DataSize;
    ioreq->iotd_Req.io_Data    = Buffer;
    ioreq->iotd_Req.io_Offset  = offset;
    ioreq->iotd_Req.io_Actual  = offset >> 32;

    return DoIO((struct IORequest *)ioreq);

    AROS_LIBFUNC_EXIT
}
