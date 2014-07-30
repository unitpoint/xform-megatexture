#ifndef __X_SIMD_MMX_H__
#define __X_SIMD_MMX_H__

#pragma once

/*
===============================================================================

	MMX implementation of xSIMDProcessor

===============================================================================
*/

#include "xSimd_Generic.h"

class xSIMD_MMX : public xSIMD_Generic
{
#ifdef _WIN32
public:
	virtual const TCHAR * VPCALL Name() const;

	virtual void VPCALL Memcpy(void *dst,			const void *src,		const int count);
	virtual void VPCALL Memset(void *dst,			const int val,			const int count);

#endif
};

#endif /* !__X_SIMD_MMX_H__ */
