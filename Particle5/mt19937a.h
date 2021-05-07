#define MT_N 624
#define MT_M 397
#define MT_MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define MT_UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define MT_LOWER_MASK 0x7fffffffUL /* least significant r bits */

class MT19937A
{
public:

	unsigned long mt_mt[MT_N]; /* the array for the state vector  */
	int mt_mti;

	MT19937A();

	void init_genrand(unsigned long);
	unsigned int genrand_int32(void);
	unsigned int rand(unsigned int);
	unsigned int randrange(unsigned int, unsigned int);
	float frand(void);

	~MT19937A();
};
