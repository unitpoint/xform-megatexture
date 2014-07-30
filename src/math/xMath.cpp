#include <xForm.h>

const float	xMath::PI				= 3.14159265358979323846f;
const float	xMath::TWO_PI			= 2.0f * PI;
const float	xMath::HALF_PI			= 0.5f * PI;
const float	xMath::ONEFOURTH_PI	= 0.25f * PI;
const float xMath::E				= 2.71828182845904523536f;
const float xMath::SQRT_TWO		= 1.41421356237309504880f;
const float xMath::SQRT_THREE		= 1.73205080756887729352f;
const float	xMath::SQRT_1OVER2		= 0.70710678118654752440f;
const float	xMath::SQRT_1OVER3		= 0.57735026918962576450f;
const float	xMath::M_DEG2RAD		= PI / 180.0f;
const float	xMath::M_RAD2DEG		= 180.0f / PI;
const float	xMath::M_SEC2MS		= 1000.0f;
const float	xMath::M_MS2SEC		= 0.001f;
const float	xMath::INFINITY		= 1e30f;
const float xMath::FLOAT_EPSILON		= 1.192092896e-07f;
const float	xMath::WORLD_INFINITY 	= 1e4f; // 10 000 = 10 km

bool		xMath::initialized		= false;
dword		xMath::iSqrt[SQRT_TABLE_SIZE];		// inverse square root lookup table

/*
===============
xMath::Init
===============
*/
void xMath::Init()
{
  if(initialized)
    return;

  union _flint fi, fo;

  for (int i = 0; i < SQRT_TABLE_SIZE; i++) {
      fi.i	 = ((EXP_BIAS-1) << EXP_POS) | (i << LOOKUP_POS);
      fo.f	 = (float)(1.0 / sqrt(fi.f));
      iSqrt[i] = ((dword)(((fo.i + (1<<(SEED_POS-2))) >> SEED_POS) & 0xFF))<<SEED_POS;
  }
    
	iSqrt[SQRT_TABLE_SIZE / 2] = ((dword)(0xFF))<<(SEED_POS); 

  xAngles a(0,0,0);
  a.ToVectors((xVec3*)&vec3_forward, (xVec3*)&vec3_right, (xVec3*)&vec3_up);

  atexit(Shutdown);
	initialized = true;
}

struct __xMath_Init__
{
  __xMath_Init__(){ xMath::Init(); }
} __xMath_Init__;

void __cdecl xMath::Shutdown()
{
  if(!initialized)
    return;

	initialized = false;
}

/*
================
xMath::FloatToBits
================
*/
int xMath::FloatToBits(float f, int exponentBits, int mantissaBits) {
	int i, sign, exponent, mantissa, value;

	assert(exponentBits >= 2 && exponentBits <= 8);
	assert(mantissaBits >= 2 && mantissaBits <= 23);

	int maxBits = (((1 << (exponentBits - 1)) - 1) << mantissaBits) | ((1 << mantissaBits) - 1);
	int minBits = (((1 <<   exponentBits  ) - 2) << mantissaBits) | 1;

	float max = BitsToFloat(maxBits, exponentBits, mantissaBits);
	float min = BitsToFloat(minBits, exponentBits, mantissaBits);

	if (f >= 0.0f) {
		if (f >= max) {
			return maxBits;
		} else if (f <= min) {
			return minBits;
		}
	} else {
		if (f <= -max) {
			return (maxBits | (1 << (exponentBits + mantissaBits)));
		} else if (f >= -min) {
			return (minBits | (1 << (exponentBits + mantissaBits)));
		}
	}

	exponentBits--;
	i = *(int*)&f;
	sign = (i >> IEEE_FLT_SIGN_BIT) & 1;
	exponent = ((i >> IEEE_FLT_MANTISSA_BITS) & ((1 << IEEE_FLT_EXPONENT_BITS) - 1)) - IEEE_FLT_EXPONENT_BIAS;
	mantissa = i & ((1 << IEEE_FLT_MANTISSA_BITS) - 1);
	value = sign << (1 + exponentBits + mantissaBits);
	value |= ((INTSIGNBITSET(exponent) << exponentBits) | (abs(exponent) & ((1 << exponentBits) - 1))) << mantissaBits;
	value |= mantissa >> (IEEE_FLT_MANTISSA_BITS - mantissaBits);
	return value;
}

/*
================
xMath::BitsToFloat
================
*/
float xMath::BitsToFloat(int i, int exponentBits, int mantissaBits) {
	static int exponentSign[2] = { 1, -1 };
	int sign, exponent, mantissa, value;

	assert(exponentBits >= 2 && exponentBits <= 8);
	assert(mantissaBits >= 2 && mantissaBits <= 23);

	exponentBits--;
	sign = i >> (1 + exponentBits + mantissaBits);
	exponent = ((i >> mantissaBits) & ((1 << exponentBits) - 1)) * exponentSign[(i >> (exponentBits + mantissaBits)) & 1];
	mantissa = (i & ((1 << mantissaBits) - 1)) << (IEEE_FLT_MANTISSA_BITS - mantissaBits);
	value = sign << IEEE_FLT_SIGN_BIT | (exponent + IEEE_FLT_EXPONENT_BIAS) << IEEE_FLT_MANTISSA_BITS | mantissa;
	return *(float*)&value;
}
