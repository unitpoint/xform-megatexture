#ifndef __X_SIMD_SSE3_H__
#define __X_SIMD_SSE3_H__

#pragma once

/*
===============================================================================

	SSE3 implementation of xSIMDProcessor

===============================================================================
*/

class xSIMD_SSE3 : public xSIMD_SSE2 {
#ifdef _WIN32
public:
	virtual const TCHAR * VPCALL Name() const;

#endif
};

#endif /* !__X_SIMD_SSE3_H__ */
