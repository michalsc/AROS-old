/*
 * ldiv.c
 *
 *  Created on: Aug 12, 2009
 *      Author: misc
 */


#include <inttypes.h>

void __attribute__((noreturn)) __aeabi_ldiv0(uint64_t);

int64_t __ldiv(int64_t a, int64_t b)
{
	int32_t sign = ((int32_t)(a >> 32))^((int32_t)(b >> 32));
	int64_t ret = 0;

	if (a < 0)
		a = -a;
	if (b < 0)
		b = -b;

	if (b == 0)
	{
		__aeabi_ldiv0(a);
	}
	else if (b > a)
	{
		return 0;
	}
	else
	{
		int first_bit_a = __builtin_clz(a);
		int first_bit_b = __builtin_clz(b);
		uint64_t mask = 0x00000001ULL << (first_bit_b-first_bit_a);
		b <<= (first_bit_b - first_bit_a);

		do
		{
			if (a >= b)
			{
				ret |= mask;
				a -= b;
			}

			mask >>=1;
			b >>=1;
		} while(mask);
	}

	if (sign < 0)
		ret = -ret;

	return ret;
}

int64_t __ldivmod_helper(int64_t a, int64_t b, int64_t *remainder)
{
	int64_t quotient;

	quotient = __ldiv(a, b);

	*remainder = a - b * quotient;

	return quotient;
}
