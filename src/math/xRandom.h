#ifndef __X_RANDOM_H__
#define __X_RANDOM_H__

#pragma once

/*
===============================================================================

	Random number generator

===============================================================================
*/

class xRandom {
public:
						xRandom(int seed = 0);

	void				SetSeed(int seed);
	int					GetSeed() const;

	int					RandomInt();			// random integer in the range [0, MAX_RAND]
	int					RandomInt(int max);		// random integer in the range [0, max[
	float				RandomFloat();		// random number in the range [0.0f, 1.0f]
	float				CRandomFloat();		// random number in the range [-1.0f, 1.0f]

	static const int	MAX_RAND = 0x7fff;

private:
	int					seed;
};

X_INLINE xRandom::xRandom(int seed) {
	this->seed = seed;
}

X_INLINE void xRandom::SetSeed(int seed) {
	this->seed = seed;
}

X_INLINE int xRandom::GetSeed() const {
	return seed;
}

X_INLINE int xRandom::RandomInt() {
	seed = 69069 * seed + 1;
	return (seed & xRandom::MAX_RAND);
}

X_INLINE int xRandom::RandomInt(int max) {
	if (max == 0) {
		return 0;			// avoid divide by zero error
	}
	return RandomInt() % max;
}

X_INLINE float xRandom::RandomFloat() {
	return (RandomInt() / (float)(xRandom::MAX_RAND + 1));
}

X_INLINE float xRandom::CRandomFloat() {
	return (2.0f * (RandomFloat() - 0.5f));
}


/*
===============================================================================

	Random number generator

===============================================================================
*/

class xRandom2 {
public:
							xRandom2(unsigned long seed = 0);

	void					SetSeed(unsigned long seed);
	unsigned long			GetSeed() const;

	int						RandomInt();			// random integer in the range [0, MAX_RAND]
	int						RandomInt(int max);		// random integer in the range [0, max]
	float					RandomFloat();		// random number in the range [0.0f, 1.0f]
	float					CRandomFloat();		// random number in the range [-1.0f, 1.0f]

	static const int		MAX_RAND = 0x7fff;

private:
	unsigned long			seed;

	static const unsigned long	IEEE_ONE = 0x3f800000;
	static const unsigned long	IEEE_MASK = 0x007fffff;
};

X_INLINE xRandom2::xRandom2(unsigned long seed) {
	this->seed = seed;
}

X_INLINE void xRandom2::SetSeed(unsigned long seed) {
	this->seed = seed;
}

X_INLINE unsigned long xRandom2::GetSeed() const {
	return seed;
}

X_INLINE int xRandom2::RandomInt() {
	seed = 1664525L * seed + 1013904223L;
	return ((int) seed & xRandom2::MAX_RAND);
}

X_INLINE int xRandom2::RandomInt(int max) {
	if (max == 0) {
		return 0;		// avoid divide by zero error
	}
	return (RandomInt() >> (16 - xMath::BitsForInteger(max))) % max;
}

X_INLINE float xRandom2::RandomFloat() {
	unsigned long i;
	seed = 1664525L * seed + 1013904223L;
	i = xRandom2::IEEE_ONE | (seed & xRandom2::IEEE_MASK);
	return ((*(float *)&i) - 1.0f);
}

X_INLINE float xRandom2::CRandomFloat() {
	unsigned long i;
	seed = 1664525L * seed + 1013904223L;
	i = xRandom2::IEEE_ONE | (seed & xRandom2::IEEE_MASK);
	return (2.0f * (*(float *)&i) - 3.0f);
}

#endif /* !__X_RANDOM_H__ */
