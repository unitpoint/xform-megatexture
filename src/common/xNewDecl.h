#ifndef __X_NEW_DECL_H__
#define __X_NEW_DECL_H__

#if !defined(DEBUG_APP_HEAP) && !defined(DEBUG_APP_HEAP_DISABLE) && defined(_DEBUG)
#define DEBUG_APP_HEAP
#endif

#include "xDef.h"

struct xStdAllocImpl
{
  void* operator new( size_t size );
  void* operator new[]( size_t size );
  
  void operator delete( void * p );
  void operator delete[]( void * p );

#if defined(DEBUG_APP_HEAP)

  void* operator new( size_t size, const char* file, int line );
  void* operator new[]( size_t size, const char* file, int line );
  
  #if defined(AEE_SIMULATOR) || defined(WIN32)
  void operator delete( void * p, const char* file, int line );
  void operator delete[]( void * p, const char* file, int line );
  #endif /* AEE_SIMULATOR */

#endif /* DEBUG_APP_HEAP */
};

void *operator new( size_t size );
void *operator new[]( size_t size );

void operator delete( void * p );
void operator delete[]( void * p );
inline void operator delete(void*, void*) {} // fixed warning when doing Vector<Vector<T> >

void *operator new( size_t size, void * p );
void *operator new[]( size_t size, void * p );

#ifdef DEBUG_APP_HEAP

  void* operator new( size_t size, const char* file, int line );
  void* operator new[]( size_t size, const char* file, int line );

  #if defined(AEE_SIMULATOR) || defined(WIN32)
  void operator delete( void * p, const char* file, int line );
  void operator delete[]( void * p, const char* file, int line );
  #endif /* AEE_SIMULATOR */

  #include "xNewDebugDecl.h"

#endif /* DEBUG_APP_HEAP */

#endif

