#ifndef __X_SIMD_3DNOW_H__
#define __X_SIMD_3DNOW_H__

#pragma once

/*
===============================================================================

	3DNow! implementation of xSIMDProcessor

===============================================================================
*/

#include "xSimd_MMX.h"

class xSIMD_3DNow : public xSIMD_MMX
{
#ifdef _WIN32
public:
	virtual const TCHAR * VPCALL Name() const;

	virtual void VPCALL Memcpy(void *dst,			const void *src,		const int count);

#endif
};

#endif /* !__X_SIMD_3DNOW_H__ */
