#ifndef INTUITION_CLASSES_H
#define INTUITION_CLASSES_H

/*
    (C) 1997 AROS - The Amiga Research OS
    $Id$

    Desc: BOOPSI structures
    Lang: english
*/

#ifndef INTUITION_CLASSUSR_H
#   include <intuition/classusr.h>
#endif
#ifndef UTILITY_HOOKS_H
#   include <utility/hooks.h>
#endif

/*#ifndef AROS_USE_OOP*/

/* The following structure is READ-ONLY */
typedef struct IClass
{
    struct Hook     cl_Dispatcher;
    ULONG           cl_Reserved;
    struct IClass * cl_Super;         /* Super-Class */
    ClassID         cl_ID;
    UWORD           cl_InstOffset;
    UWORD           cl_InstSize;
    IPTR            cl_UserData;      /* application specific */
    ULONG           cl_SubclassCount; /* # of direct suclasses */
    ULONG           cl_ObjectCount;   /* # of objects, made from this class
                                         must be 0, if the class is to be
                                         deleted */
    ULONG           cl_Flags;         /* see below */
} Class;
/*#endif*/

/* cl_Flags */
#define CLF_INLIST (1L<<0)

/*#ifndef AROS_USE_OOP*/

/* This structure is situated before the pointer. It may grow in future,
   but o_Class will always stay at the end, so that you can substract
   the size of a pointer from the object-pointer to get a pointer to the
   pointer to the class of the object. */
struct _Object
{
    struct MinNode  o_Node;  /* PRIVATE */
    struct IClass * o_Class;
};
/*#endif*/

#define _OBJ(obj) ((struct _Object *)(obj))
#define BASEOBJECT(obj) ((Object *)(_OBJ(obj) + 1))
#define _OBJECT(obj) (_OBJ(obj) - 1)

/*#ifndef AROS_USE_OOP*/
#define OCLASS(obj) ((_OBJECT(obj))->o_Class)

#define INST_DATA(class, obj) ((VOID *)(obj) + (class)->cl_InstOffset)
/*#endif*/

#define SIZEOF_INSTANCE(class) ((class)->cl_InstOffset + (class)->cl_InstSize \
                               + sizeof(struct _Object))

#endif /* INTUITION_CLASSES_H */
