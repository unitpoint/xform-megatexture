#include <xForm.h>

//===============================================================
//
//	SSE2 implementation of xSIMDProcessor
//
//===============================================================

#ifdef _WIN32

/*
============
xSIMD_SSE2::GetName
============
*/
const TCHAR * xSIMD_SSE2::Name() const {
	return _T("MMX & SSE & SSE2");
}

#endif /* _WIN32 */
