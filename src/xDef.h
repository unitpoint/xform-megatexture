#ifndef __x_def_h__
#define __x_def_h__

#pragma once

#include <tchar.h>
#include <assert.h>
// #include "common/xNewDecl.h"

#define ASSERT assert

#define MEMCMP memcmp
#define MEMCPY memcpy
#define MEMSET memset

typedef unsigned char			byte;		// 8 bits
typedef unsigned short		word;		// 16 bits
typedef unsigned int			dword;	// 32 bits
typedef unsigned int			uint;
typedef unsigned long			ulong;

typedef int         int32;
typedef short       int16;
typedef signed char int8;

typedef unsigned int    uint32;
typedef unsigned short  uint16;
typedef unsigned char   uint8;

#define X_INLINE __forceinline 
#define _alloca16(x) ((void*)((((int)_alloca((x)+15)) + 15) & ~15))

#define ALIGN16(x)					__declspec(align(16)) x
#define PACKED

#define MAKEID(d,c,b,a)	(((int)(a) << 24) | ((int)(b) << 16) | ((int)(c) << 8) | ((int)(d)))

// =================================================================
// =================================================================
// =================================================================

#pragma warning(disable: 4714)				// function marked as __forceinline not inlined
#pragma warning(disable: 4311)				// pointer truncation from 'void *' to 'int'
#pragma warning(disable: 4312)				// conversion from 'int' to 'void *' of greater size
#pragma warning(disable: 4244)				// conversion to smaller type, possible loss of data
#pragma warning(disable: 4996)				// was declared deprecated
#pragma warning(disable: 4800)				// forcing to bool
 
#endif