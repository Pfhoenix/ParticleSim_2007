#include "mt19937a.h"

MT19937A::MT19937A()
{
	mt_mti = MT_N + 1; /* mti==N+1 means mt[N] is not initialized */

	return;
}

/* initializes mt[N] with a seed */
void MT19937A::init_genrand(unsigned long s)
{
	mt_mt[0] = s & 0xffffffffUL;
	for (mt_mti = 1; mt_mti < MT_N; mt_mti++)
	{
		mt_mt[mt_mti] = (1812433253UL * (mt_mt[mt_mti - 1] ^ (mt_mt[mt_mti - 1] >> 30)) + mt_mti); 
		/* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
		/* In the previous versions, MSBs of the seed affect   */
		/* only MSBs of the array mt[].                        */
		/* 2002/01/09 modified by Makoto Matsumoto             */
		mt_mt[mt_mti] &= 0xffffffffUL;
		/* for >32 bit machines */
    }

	return;
}

/* generates a random number on [0,0xffffffff]-interval */
unsigned int MT19937A::genrand_int32(void)
{
	unsigned int y;
	static unsigned int mag01[2] = {0x0UL, MT_MATRIX_A};
	/* mag01[x] = x * MATRIX_A  for x=0,1 */

	if (mt_mti >= MT_N) /* generate N words at one time */
	{
		int kk;

		if (mt_mti == MT_N + 1)   /* if init_genrand() has not been called, */
			init_genrand(5489UL); /* a default initial seed is used */

		for (kk = 0; kk < MT_N - MT_M; kk++)
		{
			y = (mt_mt[kk] & MT_UPPER_MASK) | (mt_mt[kk + 1] & MT_LOWER_MASK);
			mt_mt[kk] = mt_mt[kk + MT_M] ^ (y >> 1) ^ mag01[y & 0x1UL];
		}
		for (; kk < MT_N - 1; kk++)
		{
			y = (mt_mt[kk] & MT_UPPER_MASK) | (mt_mt[kk + 1] & MT_LOWER_MASK);
			mt_mt[kk] = mt_mt[kk + (MT_M - MT_N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
		}
		y = (mt_mt[MT_N - 1] & MT_UPPER_MASK) | (mt_mt[0] & MT_LOWER_MASK);
		mt_mt[MT_N - 1] = mt_mt[MT_M - 1] ^ (y >> 1) ^ mag01[y & 0x1UL];

		mt_mti = 0;
	}
  
	y = mt_mt[mt_mti++];

	/* Tempering */
	y ^= (y >> 11);
	y ^= (y << 7) & 0x9d2c5680UL;
	y ^= (y << 15) & 0xefc60000UL;
	y ^= (y >> 18);

	return y;
}

unsigned int MT19937A::rand(unsigned int Max)
{
	return genrand_int32() % Max;
}

unsigned int MT19937A::randrange(unsigned int Low, unsigned int High)
{
	return (rand(High - Low) + Low);
}

float MT19937A::frand(void)
{
	return genrand_int32() / (float) 0xFFFFFFFF;
}

MT19937A::~MT19937A()
{
	return;
}
