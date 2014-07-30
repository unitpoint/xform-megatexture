#include <xForm.h>

//===============================================================
//
//	SSE3 implementation of xSIMDProcessor
//
//===============================================================

#ifdef _WIN32

/*
============
xSIMD_SSE3::GetName
============
*/
const TCHAR * xSIMD_SSE3::Name() const {
	return _T("MMX & SSE & SSE2 & SSE3");
}

#endif /* _WIN32 */
