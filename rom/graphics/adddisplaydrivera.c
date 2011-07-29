/*
    Copyright � 1995-2011, The AROS Development Team. All rights reserved.
    $Id$

    Desc: AROS-specific function for adding a display driver
    Lang: english
*/

#include <aros/debug.h>
#include <graphics/driver.h>
#include <oop/oop.h>
#include <proto/utility.h>

#include "graphics_intern.h"
#include "dispinfo.h"

/*****************************************************************************

    NAME */
#include <proto/graphics.h>

	AROS_LH2(ULONG, AddDisplayDriverA,

/*  SYNOPSIS */
	AROS_LHA(APTR, gfxhidd, A0),
	AROS_LHA(const struct TagItem *, tags, A1),

/*  LOCATION */
	struct GfxBase *, GfxBase, 181, Graphics)

/*  FUNCTION
	Add a display driver to the system.

    INPUTS
	gfxhidd - A newly created driver object
	tags    - An optional TagList. Valid tags are:

	    DDRV_BootMode     - A boolean value telling that a boot mode driver
			        is being added. Boot mode drivers will automatically
			        shutdown on next AddDisplayDriverA() call, unless
			        DDRV_KeepBootMode = TRUE is specified. Defaults to FALSE.
	    DDRV_MonitorID    - Starting monitor ID to assign to the driver. Use it
				with care. Attempt to add already existing ID will
				fail with DD_ID_EXISTS code. By default a next available
				ID will be picked up automatically.
	    DDRV_ReserveIDs   - A number of subsequent monitor IDs to reserve. Reserved IDs
				can be reused only with DDRV_MonitorID tag. This tag is
				provided as an aid to support possible removable display
				devices. Defaults to 1.
	    DDRV_KeepBootMode - Do not shut down boot mode drivers. Use this tag if you
				are 100% sure that your driver won't conflict with boot mode
				driver (like VGA or VESA) and won't attempt to take over its
				hardware. Defaults to FALSE.
	    DDRV_ResultID     - A pointer to ULONG location where ID assigned to your driver
	    			will be placed. Useful if you reserve some ID for future use.
	    			Note that returned ID will be the one just assigned to your
	    			driver instance. Increment it yourself in order to obtain
	    			other reserved IDs.
	    DDRV_IDMask	      - A mask for separating monitor ID from HIDD-specific part.
	    			This mask specifies what mode ID bits are monitor ID and
	    			what bits actually specify the mode. A default value is
	    			0xFFFF0000.
	    			
	    			Using the mask you can split your monitor ID into 'sub-Ids'.
	    			Example:

	    			Supplied tags: DDRV_IDMask, 0xFFFFFF00, DDRV_ResultID, &myid

	    			After succesfull call myid will contain base ID assigned by
	    			graphics.library to your driver, let's say 0x00140000. However,
	    			since you specified longer mask, you leave only one byte for mode
	    			designation, and reserve the whole range of IDs from 0x001400xx to
	    			0x0014FFxx for different instances of your driver. They can now be
	    			used by specifying DDRV_MonitorID with corresponding value.

				Note that for this feature to work correctly, you also need to override
				mode ID processing in your driver class. Default methods provided by
				hidd.graphics.graphics base class suppose that the whole lower word
				of mode ID specifies the display mode.

				It is generally not allowed to specify shorter masks than 0xFFFF0000.
				The only driver which can do this is Amiga(tm) chipset driver, which
				need to occupy the reserved range of IDs from 0x0000xxxx to 0x000Axxxx.
				In any other case supplying short mask will cause undefined behavior.

				Since DDRV_ReserveIDs provide simpler way to reserve IDs for your driver
				(without the need to override mode ID processing), this	option can be
				considered experimental and even private. In fact the primary reason for
				it to exist is to provide support for Amiga(tm) chipset	driver.

    RESULT
    	error - One of following codes:

	    DD_OK        - Operation completed OK.
	    DD_NO_MEM	 - There is not enough memory to set up internal data.
	    DD_ID_EXISTS - Attempt to assign monitor IDs that are already used.

    NOTES
	This function is AROS-specific.

    EXAMPLE

    BUGS
	graphics.library tracks down usage of display drivers. If a driver currently
	has something on display, it will not be shut down, even if it's boot mode
	driver. This can cause problems if the new driver attempts to take over
	the same hardware (for example native mode driver vs VESA driver). So be careful
	while adding new display drivers on a working system. Know what you do.

    SEE ALSO

    INTERNALS

    HISTORY
	29-10-95    digulla automatically created from
			    graphics_lib.fd and clib/graphics_protos.h

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    struct TagItem *tag;
    struct monitor_driverdata *mdd;
    ULONG FirstID = INVALID_ID;
    ULONG NextID;
    ULONG NumIDs = 1;
    ULONG IDMask = AROS_MONITOR_ID_MASK;
    BOOL keep_boot = FALSE;
    UWORD flags = 0;
    ULONG *ResultID = NULL;
    ULONG ret = DD_OK;

    EnterFunc(bug("AddDisplayDriverA(0x%p)\n", gfxhidd));

    /* First parse parameters */
    while ((tag = NextTagItem(&tags)))
    {
    	switch (tag->ti_Tag)
    	{
    	case DDRV_MonitorID:
    	    FirstID = tag->ti_Data;
    	    break;

    	case DDRV_ReserveIDs:
    	    NumIDs = tag->ti_Data;
    	    break;

	case DDRV_IDMask:
	    IDMask = tag->ti_Data;
	    break;
	
	case DDRV_KeepBootMode:
	    keep_boot = tag->ti_Data;
	    break;

	case DDRV_BootMode:
	    flags = tag->ti_Data ? DF_BootMode : 0;
	    break;

	case DDRV_ResultID:
	    ResultID = (ULONG *)tag->ti_Data;
	    break;
	}
    }

    /* We lock for the entire function because we want to be sure that
       IDs will remain free during driver_Setup() */
    ObtainSemaphore(&CDD(GfxBase)->displaydb_sem);

    /* Default value for monitor ID */
    if (FirstID == INVALID_ID)
	FirstID = CDD(GfxBase)->last_id;

    /*
     * Calculate next free ID.
     * Mechanism of increment calculation: we invert the mask and add 1 to it.
     * This way for example 0xFFFF0000 becomes 0x00010000.
     * Before doing this we make sure that mask used in this equation is not
     * longer than 0xFFFF0000, for proper ID counting.
     */
    NextID = FirstID + NumIDs * (~(IDMask & AROS_MONITOR_ID_MASK) + 1);
    D(bug("[AddDisplayDriverA] First ID 0x%08X, next ID 0x%08X\n", FirstID, NextID));

    /* First check if the operation can actually be performed */
    for (mdd = CDD(GfxBase)->monitors; mdd; mdd = mdd->next)
    {
    	/* Check if requested IDs are already allocated */
	if ((mdd->id >= FirstID && mdd->id < NextID))
	{
	    ret = DD_ID_EXISTS;
	    break;
	}

	/*
	 * Now check if boot mode drivers can really be unloaded.
	 * Some drivers can start playing with their hardware during
	 * early init, so we need to check it before driver_Setup().
	 */
	if (!keep_boot)
	{
	    /* The driver can be unloaded if it has nothing on display */
	    if ((mdd->flags & DF_BootMode) && (mdd->display))
	    {
	    	ret = DD_IN_USE;
	    	break;
	    }
	}
    }

    if (ret == DD_OK)
    {
	/* Attach system structures to the driver */
	D(bug("[AddDisplayDriverA] Installing driver\n"));
	mdd = driver_Setup(gfxhidd, GfxBase);
	D(bug("[AddDisplayDriverA] monitor_driverdata 0x%p\n", mdd));

	if (mdd)
	{
	    struct monitor_driverdata *last, *old;

	    mdd->id    =  FirstID;
	    mdd->mask  =  IDMask;
	    mdd->flags |= flags;

	    if (CDD(GfxBase)->DriverNotify)
		/* Use mdd->gfxhidd here because it can be substituted by fakegfx object */
		mdd->userdata = CDD(GfxBase)->DriverNotify(mdd, TRUE, CDD(GfxBase)->notify_data);

	    /* Remove boot mode drivers */
	    if (!keep_boot)
	    {
		D(bug("[AddDisplayDriverA] Shutting down boot mode drivers\n"));
		for (last = (struct monitor_driverdata *)CDD(GfxBase);; last = last->next)
		{
		    D(bug("[AddDisplayDriverA] Current 0x%p, next 0x%p\n", last, last->next));

		    while (last->next && (last->next->flags & DF_BootMode))
		    {
		        old = last->next;
			D(bug("[AddDisplayDriverA] Shutting down driver 0x%p (ID 0x%08lX, next 0x%p)\n", old, old->id, old->next));
			last->next = old->next;
			driver_Expunge(old, GfxBase);
			D(bug("[AddDisplayDriverA] Shutdown OK, next 0x%p\n", last->next));
		    }

		    /* We check this condition here explicitly because last->next is modified inside loop body.
		       If we check it in for() statement, last = last->next will be executed BEFORE the check,
		       and NULL pointer may be hit. */
		    if (!last->next)
			break;
		}
	    }

	    /* Insert the driverdata into chain, sorted by ID */
	    D(bug("[AddDisplayDriverA] Inserting driver 0x%p, ID 0x%08lX\n", mdd, mdd->id));
	    for (last = (struct monitor_driverdata *)CDD(GfxBase); last->next; last = last->next)
	    {
	        D(bug("[AddDisplayDriverA] Current 0x%p, next 0x%p, ID 0x%08lX\n", last, last->next, last->next->id));
		if (mdd->id < last->next->id)
		    break;
	    }
	    D(bug("[AddDisplayDriverA] Inserting after 0x%p\n", last));
	    mdd->next = last->next;
	    last->next = mdd;

	    /* Remember next available ID */
	    if (NextID > CDD(GfxBase)->last_id)
		CDD(GfxBase)->last_id = NextID;

	    /* Return the assigned ID if the caller asked to do so */
	    if (ResultID)
	    	*ResultID = FirstID;

	} else
	    ret = DD_NO_MEM;
    }

    ReleaseSemaphore(&CDD(GfxBase)->displaydb_sem);

    /* Set the first non-boot non-planar driver as default */
    if ((ret == DD_OK) && (!GfxBase->default_monitor) && (!(mdd->flags & DF_BootMode)))
    {
    	OOP_Class *cl = OOP_OCLASS(mdd->gfxhidd_orig);

	/*
	 * Amiga(tm) chipset driver does not become a default.
	 * This is done because RTG modes (if any) are commonly preferred
	 * over it.
	 * TODO: in future some prefs program could be implemented. It would
	 * allow the user to describe the physical placement of several displays
	 * in his environment, and explicitly set the preferred display.
	 */
	if (strcmp(cl->ClassNode.ln_Name, "hidd.gfx.amigavideo"))
	{
	    /*
	     * graphics.library uses struct MonitorSpec pointers for historical reasons,
	     * so we satisfy it.
	     * Here we just get the first available sync object from the driver and
	     * set default_monitor fo its MonitorSpec. This allows BestModeIDA() to
	     * obtain preferred monitor back from this MonitorSpec (by asking the associated
	     * sync object about its parent driver).
	     *
	     * TODO:
	     * Originally display drivers in AmigaOS had a concept of "preferred mode ID".
	     * Every driver supplied own hardcoded ID which can be retrieved by GetDisplayInfoData()
	     * in MonitorInfo->PreferredModeID. Currently AROS does not implement this concept.
	     * However this sync could be a preferred mode's sync.
	     * It needs to be researched what exactly this mode ID is. Implementing this concept would
	     * improve AmigaOS(tm) compatibility.
	     */
    	    OOP_Object *sync = HIDD_Gfx_GetSync(mdd->gfxhidd_orig, 0);

	    OOP_GetAttr(sync, aHidd_Sync_MonitorSpec, (IPTR *)&GfxBase->default_monitor);
	}
    }

    D(bug("[AddDisplayDriverA] Returning %u\n", ret));
    return ret;

    AROS_LIBFUNC_EXIT
} /* LateGfxInit */
