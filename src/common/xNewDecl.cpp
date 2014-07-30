#include <malloc.h>
// #include <ASSERT.h>
#include "xNewDecl.h"
#include "xHeap.h"

#ifdef new
#undef new
#endif

void* operator new( size_t /*size*/, void * p )
{
  return p;
}
void* operator new[]( size_t /*size*/, void * p )
{
  return p;
}

#ifdef DEBUG_APP_HEAP

void *xStdAllocImpl::operator new(size_t size, const char * /*file */,
    int /*line */)
{
  return malloc(size);
}

void *xStdAllocImpl::operator new[](size_t size, const char * /*file */,
    int /*line */)
{
  return malloc(size);
}

#if defined(AEE_SIMULATOR) || defined(WIN32)
void xStdAllocImpl::operator delete(void *p, const char * /*file */,
    int /*line */)
{
  free(p);
}

void xStdAllocImpl::operator delete[](void *p, const char * /*file */,
    int /*line */)
{
  free(p);
}
#endif // AEE_SIMULATOR

#endif /* DEBUG_APP_HEAP */

void *xStdAllocImpl::operator new(size_t size)
{
  return malloc(size);
}

void *xStdAllocImpl::operator new[](size_t size)
{
  return malloc(size);
}

void xStdAllocImpl::operator delete(void *p)
{
  free(p);
}

void xStdAllocImpl::operator delete[](void *p)
{
  free(p);
}

// ============================================================================

#include "xHeap.h"

void operator delete(void *p)
{
  xHeap::Instance()->Free(p);
}

void operator delete[](void *p)
{
  xHeap::Instance()->Free(p);
}

#ifdef DEBUG_APP_HEAP

void *operator new(size_t size, const char *file, int line)
{
  return xHeap::Instance()->Alloc((uint32)size, file, line);
}

void *operator new[](size_t size, const char *file, int line)
{
  return xHeap::Instance()->Alloc((uint32)size, file, line);
}

#if defined(AEE_SIMULATOR) || defined(WIN32)

void operator delete(void *p, const char * /*file*/,
    int /*line*/)
{
  xHeap::Instance()->Free(p);
}

void operator delete[](void *p, const char * /*file*/,
    int /*line*/)
{
  xHeap::Instance()->Free(p);
}

#endif // AEE_SIMULATOR

void *operator new(size_t size)
{
  return xHeap::Instance()->Alloc((uint32)size, "__DUMMY__", 0);
  // ASSERT(!"Wrong operator new is used. Ensure that you're including NewDecl.h");
  // return NULL;
}

void *operator new[](size_t size)
{
  return xHeap::Instance()->Alloc((uint32)size, "__DUMMY__", 0);
  // ASSERT(!"Wrong operator delete is used. Ensure that you're including NewDecl.h");
  // return NULL;
}

#else /* DEBUG_APP_HEAP */

void *operator new(size_t size)
{
  return xHeap::Instance()->Alloc((uint32)size);
}

void *operator new[](size_t size)
{
  return xHeap::Instance()->Alloc((uint32)size);
}

#endif /* DEBUG_APP_HEAP */
