#ifndef __X_SIMD_SSE2_H__
#define __X_SIMD_SSE2_H__

#pragma once

/*
===============================================================================

	SSE2 implementation of xSIMDProcessor

===============================================================================
*/

class xSIMD_SSE2 : public xSIMD_SSE {
#ifdef _WIN32
public:
	virtual const TCHAR * VPCALL Name() const;

#endif
};

#endif /* !__X_SIMD_SSE2_H__ */
