#ifndef __X_BIT_ARRAY_H__
#define __X_BIT_ARRAY_H__

#pragma once

class xBitArray
{
  char * buf;
  int size;

  static const int granularity = 8;

  X_INLINE void Alloc(int size)
  {
    size = (size + granularity-1) & ~(granularity-1);
    char * newbuf = new char[size];
    memcpy(newbuf, buf, this->size < size ? this->size : size);
    delete [] buf;
    buf = newbuf;
    this->size = xHeap::Instance()->Size(buf);
  }

public:

  X_INLINE xBitArray(){ buf = NULL; size = 0; }
  X_INLINE xBitArray(int count)
  { 
    assert(count > 0);
    buf = new char[(count>>3) + 1];
    size = xHeap::Instance()->Size(buf);
  }
  X_INLINE ~xBitArray(){ delete [] buf; }

  X_INLINE xBitArray& operator=(const xBitArray& b)
  {
    delete [] buf;
    buf = new char[b.size];     
    size = xHeap::Instance()->Size(buf);
    memcpy(buf, b.buf, b.size);
  }

  X_INLINE bool operator[](int i) const { return Get(i); }
  X_INLINE bool Get(int i) const
  {
    assert(i >= 0);
    int a = i >> 3;
    return a < size ? (buf[a] >> (i&7)) & 1 : false;
  }
  X_INLINE void Set(int i)
  {
    assert(i >= 0);
    int a = i >> 3;
    if(a >= size)
      Alloc(a+1);
    
    buf[a] |= 1 << (i&7);
  }
  X_INLINE void Clear(int i)
  {
    assert(i >= 0);
    int a = i >> 3;
    if(a < size)
      buf[a] &= ~(1 << (i&7));
  }
  X_INLINE void Set(int i, bool value)
  {
    if(value)
      Set(i);
    else
      Clear(i);
  }
  X_INLINE void Clear()
  {
    delete [] buf;
    buf = NULL;
    size = 0;
  }
};

#endif