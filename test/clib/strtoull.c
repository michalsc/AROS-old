#include <stdlib.h>
#include <stdio.h>
#include "test.h"

int main(void)
{
#if defined(AROS_HAVE_LONG_LONG)
    TEST((strtoull("0xff", NULL, 0) == 255ULL))
    TEST((strtoull("0xff", NULL, 16) == 255ULL))
    TEST((strtoull("0x0", NULL, 0) == 0ULL))
    TEST((strtoull("0x0", NULL, 16) == 0ULL))
    TEST((strtoull("0", NULL, 0) == 0ULL))
    TEST((strtoull("0", NULL, 16) == 0ULL))
    TEST((strtoul("0x0 ", NULL, 0) == 0ULL))
    TEST((strtoul("0x0 ", NULL, 16) == 0ULL))
    TEST((strtoul("0 ", NULL, 0) == 0ULL))
    TEST((strtoul("0 ", NULL, 16) == 0ULL))
    TEST((strtoull("0377", NULL, 0) == 255ULL))
    TEST((strtoull("255", NULL, 0) == 255ULL))
    TEST((strtoul("-1", NULL, 0) == -1ULL))
    TEST((strtoul("-0xff", NULL, 0) == -255ULL))
    TEST((strtoul("-0xff", NULL, 16) == -255ULL))
    TEST((strtoul("-ff", NULL, 16) == -255ULL))
    TEST((strtoul("-0377", NULL, 0) == -255ULL))
    TEST((strtoul("-377", NULL, 8) == -255ULL))
#endif
    return OK;
}

void cleanup(void)
{
}
