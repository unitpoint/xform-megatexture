#include "xHeap.h"
#include "xString.h"
#include <math.h>
// #include <ASSERT.h>

/*
//#include "stdafx.h"
#include <stdio.h>
#include <ctype.h>
#include <ASSERT.h>
#include <math.h>

#include "xString.h"
#include "xHeap.h"
#include "../math/xMath.h"
*/

#define X_VA_BUF_SIZE (10*1024)

// =============================================
bool xString::initialized = false;
xString * xString::pEmpty;

xString::Data * xString::Data::Alloc(int size, int pad)
{
  Data * d = (Data*)new char[sizeof(Data) + size + pad + sizeof(TCHAR) + sizeof(TCHAR)/2];
  if(d){
    d->numRefs = 1;
    d->size = size;
  }
  return d;
}

void xString::Data::Release()
{
  if(!--numRefs)
    delete [] (char*)this;
}

void xString::Data::TerminateData()
{
  char * s = (char*)ToChar();
  if(sizeof(TCHAR) == 1){
    s[size] = 0;
  }else{
    if(size&1){
      s[size] = 0;
      *(TCHAR*)(s+size+1) = 0;
    }else
      *(TCHAR*)(s+size) = 0;
  }
}

// =============================================

struct __str_start__
{
  __str_start__()
  { 
    xString::Init();
  }
} __str_start__;

// =============================================

void xString::Init()
{
  static char EmptyStringData[sizeof(Data)+sizeof(_T(""))];
  static char EmptyString[sizeof(xString)];

  if(initialized)
    return;

  Data * d = (Data*)&EmptyStringData;
  d->numRefs = 2;
  d->size = 0;
  pEmpty = (xString*)&EmptyString;
  pEmpty->str = d->ToChar();
  memcpy(pEmpty->str, _T(""), sizeof(_T("")));
  
  atexit(Shutdown);
  initialized = true;
}

void __cdecl xString::Shutdown()
{
  Data * d = pEmpty->ToStringData();
  ASSERT(d->numRefs == 2);
  initialized = false;
}

// =============================================

void xString::Init(int size)
{
  if(size <= 0){
    Init(*pEmpty);
    return;
  }
  Data * d = Data::Alloc(size, 0);
  if(!d){
    Init(*pEmpty);
    return;
  }
  str = d->ToChar();
}

void xString::Init(const void * p, int size)
{
  if(size <= 0){
    Init(*pEmpty);
    return;
  }
  Data * d = Data::Alloc(size, 0);
  if(!d){
    Init(*pEmpty);
    return;
  }
  str = d->ToChar();
  memcpy(str, p, size);
  // d->TerminateData(); alloc - fill mem to zero
}

void xString::Init(const void * p1, int size1, const void * p2, int size2)
{
  if(size1 <= 0){
    Init(p2, size2);
    return;
  }
  if(size2 <= 0){
    Init(p1, size1);
    return;
  }
  int newSize = size1 + size2;
  Data * d = Data::Alloc(newSize, (size1 + size2) / 4);
  if(!d){
    Init(*pEmpty);
    return;
  }
  str = d->ToChar();
  memcpy(str, p1, size1);
  memcpy((char*)str + size1, p2, size2);
  // d->TerminateData();
}

void xString::Init(const void * p1, int size1, 
                   const void * p2, int size2, 
                   const void * p3, int size3)
{
  if(size1 <= 0){
    Init(p2, size2, p3, size3);
    return;
  }
  if(size2 <= 0){
    Init(p1, size1, p3, size3);
    return;
  }
  if(size3 <= 0){
    Init(p1, size1, p2, size2);
    return;
  }
  int newSize = size1 + size2 + size3;
  Data * d = Data::Alloc(newSize, (size1 + size2 + size3) / 4);
  if(!d){
    Init(*pEmpty);
    return;
  }
  str = d->ToChar();
  memcpy(str, p1, size1);
  memcpy((char*)str + size1, p2, size2);
  memcpy((char*)str + size1 + size2, p3, size3);
  // d->TerminateData();
}

#ifdef  _UNICODE
void xString::Init(const char * s)
{
  if(sizeof(char) == sizeof(TCHAR)){
    Init((const TCHAR*)s);
    return;
  }
  int size = (unsigned)strlen(s) * (unsigned)sizeof(TCHAR);
  Data * d = Data::Alloc(size, 0);
  if(!d){
    Init(*pEmpty);
    return;
  }
  str = d->ToChar();
  MultiByteToWideChar(CP_ACP, 0, (CHAR*)s, -1, (WCHAR*)str, (unsigned)size / (unsigned)sizeof(TCHAR));
}
#endif

xString& xString::SetSize(int size, int pad, bool keepData)
{
  Data * d2, * d = ToStringData();  
  if(d->numRefs == 1){
    int curMaxSize = xHeap::Instance()->Size(d) - sizeof(Data) - sizeof(TCHAR);
    if(curMaxSize >= size){
      int ZeroFrom = size < d->size ? size : d->size;
      memset((char*)str + ZeroFrom, 0, curMaxSize - ZeroFrom + sizeof(TCHAR));
      d->size = size;
      return *this;
    }
  }
  if(!keepData){
    d->Release();
    d2 = Data::Alloc(size, pad);
    if(!d2){
      Init(*pEmpty);
      return *this;
    }
    str = d2->ToChar();
    return *this;
  }

  d2 = Data::Alloc(size, pad);
  if(!d2){
    d->Release();
    Init(*pEmpty);
    return *this;
  }
  str = d2->ToChar();
  int copySize = d->size < size ? d->size : size;
  memcpy(str, d->ToChar(), copySize);
  d->Release();
  return *this;
}

xString& xString::SetFormat(const TCHAR * fmt, va_list va)
{
  TCHAR buf[X_VA_BUF_SIZE];
  _vstprintf(buf,fmt,va);
  return Assign(buf);
}

xString& xString::Separate()
{
  Data * d = ToStringData();
  if(d->numRefs == 1) 
      return *this;

  return Assign(str, d->size);
}
xString xString::Clone() const
{
  return xString((const void*)str, Size());
}

TCHAR xString::Char(int i) const 
{ 
  return i >= 0 && i < Len() ? str[i] : 0;
}
xString& xString::SetChar(int i, TCHAR c)
{ 
  Data * d = ToStringData();
  if(i >= 0 && i < Len()){
    Separate(); 
    str[i] = c; 
  }
  return *this;
}

xString& xString::Assign(const xString& a)
{
  Data * d = ToStringData();
  Init(a);
  d->Release();
  return *this;
}
xString& xString::Assign(const void * p, int size)
{
  Data * d = ToStringData();
  Init(p, size);
  d->Release();
  return *this;
}
xString& xString::Assign(const void * p1, int size1, const void * p2, int size2)
{
  Data * d = ToStringData();
  Init(p1, size1, p2, size2);
  d->Release();
  return *this;
}

xString& xString::InsertData(int i, const xString& a)
{
  return InsertData(i, a.str, a.Size());
}
xString& xString::InsertData(int i, const void * p, int size)
{
  Data * d = ToStringData();
  if(i < 0 || i > d->size)
    return *this;

  Init((const void*)str, i, p, size, (char*)str + i, d->size-i);
  d->Release();
  return *this;
}

xString xString::SubString(int start,int len) const
{
  return SubData((unsigned)start * (unsigned)sizeof(TCHAR),
                 (unsigned)len * (unsigned)sizeof(TCHAR));
}
xString xString::SubData(int start,int size) const
{
  Data * d = ((xString*)this)->ToStringData();
  if(start < 0){
    start = d->size + start;
    if(start < 0)
      start = 0;
  }
  if(start >= d->size)
    return xString();

  if(size < 0){
    size = d->size - start + size;
    if(size < 0)
      return xString();
  }
  if(start + size > d->size)
    size = d->size - start;

  if(!start && size == d->size)
    return xString(*this);

  return xString((const void*)((char*)str + start), size);
}

xString& xString::Append(const xString& a)
{
  return Append((void*)a.str, a.Size());
}
xString& xString::Append(const void * p, int size)
{
  if(size > 0){
    int oldSize = this->Size();
    int newSize = oldSize + size;
    SetSize(newSize, newSize / 2,  true);
    memcpy((char*)str + oldSize, p, size);
  }
  return *this;
}
xString& xString::Append(const void * p1, int size1, const void * p2, int size2)
{
  if(size1 <= 0)
    return Append(p2, size2);

  if(size2 <= 0)
    return Append(p1, size1);

  int oldSize = this->Size();
  int newSize = oldSize + size1 + size2;
  SetSize(newSize, newSize / 2, true);
  memcpy((char*)str + oldSize, p1, size1);
  memcpy((char*)str + oldSize + size1, p2, size2);
  return *this;
}

xString xString::Lower() const
{
  xString str = Clone();
  TCHAR * s = str.str;
  for(int len = str.Len(); len > 0; len--, s++)
    *s = _totlower(*s);
  
  return str;
}
xString xString::Upper() const
{
  xString str = Clone();
  TCHAR * s = str.str;
  for(int len = str.Len(); len > 0; len--, s++)
    *s = _totupper(*s);
  
  return str;
}
xString xString::Flower() const
{
  if(!Size())
    return xString();

  xString str = Clone();
  TCHAR * s = str.str;
  *s = _totupper(*s);
  int len = str.Len()-1;
  for(s++; len > 0; len--, s++)
    *s = _totlower(*s);
  
  return str;
}

xString xString::Trim(bool leftTrim, bool rightTrim) const
{
  const TCHAR * start = ToChar();
  const TCHAR * end = start + Len();
  bool realSub = false;
  if(leftTrim)
    while(_istspace(*start)){
      start++;
      realSub = true;
    }

  if(rightTrim)
    while(end > start && _istspace(end[-1])){
      end--;
      realSub = true;
    }

  return realSub ? xString((void*)start, (int)end - (int)start) : *this;
}

int xString::Cmp(const TCHAR * a, int aLen, const TCHAR * b, int bLen, int n)
{
  return CmpData((const void*)a, (unsigned)aLen * sizeof(TCHAR),
                  (const void*)b, (unsigned)bLen * sizeof(TCHAR),
                  (unsigned)n * sizeof(TCHAR));
}
int xString::Icmp(const TCHAR * a, int aLen, const TCHAR * b, int bLen, int n)
{
  int len = aLen < bLen ? aLen : bLen;
  if(len > n)
    len = n;
  //else if(n > len)
  //  n = len;

  for(int k, i = 0; i < len; i++){
    k = _totlower(*a++) - _totlower(*b++);
    if(k)
      return k;
  }
  return len == n || aLen == bLen ? 0 : (aLen < bLen ? -1 : 1);
}

int xString::Cmp(const TCHAR * b, int n) const 
{
  return CmpData((const void*)str, Size(), (const void*)b, (int)_tcslen(b) * sizeof(TCHAR),
                 (unsigned)n * sizeof(TCHAR));
}
int xString::Icmp(const TCHAR * b, int n) const 
{
  return Icmp(str, Len(), b, (int)_tcslen(b), n);
}

int xString::Cmp(const xString& b, int n) const 
{
  return CmpData((const void*)str, Size(), (const void*)b.str, b.Size(),
                 (unsigned)n * sizeof(TCHAR));
}
int xString::Icmp(const xString& b, int n) const 
{
  return Icmp(str, Len(), b.str, b.Len(), n);
}

int xString::CmpData(const void * a, int aSize, const void * b, int bSize, int n)
{
  int size = aSize < bSize ? aSize : bSize;
  if(size > n)
    size = n;
  //else if(n > size)
  //  n = size;
  /* int maxSize = aSize > bSize ? aSize : bSize;
  if(n > maxSize)
    n = maxSize; */

  int k = memcmp(a, b, size);
  if(k || size == n)
    return k;

  if(aSize < bSize) return -1;
  if(aSize > bSize) return 1;
  return 0;
}

xString xString::ToString(int a)
{
  return Format(_T("%i"), a);
}

TCHAR * xString::ToString(TCHAR * dst, float a, int precision)
{
  TCHAR fmt[32];
  if(precision <= 0){
    if(precision < 0){
      float p = powf(10.0, -precision);
      a = int(a / p) * p;
    }
    _stprintf(dst, _T("%.f"), a);
    return dst;
  }
  _stprintf(fmt,_T("%%.%df"), precision);
  int n = _stprintf(dst, fmt, a);
  
  while(n > 0 && dst[n-1] == '0') dst[--n] = (TCHAR)0;
  if(n > 0 && dst[n-1] == '.') dst[--n] = (TCHAR)0;
  
  return dst;
}

xString xString::ToString(float a, int precision)
{
  TCHAR s[256];
  return ToString(s, a, precision);
}

xString xString::FloatArrayToString(const float * a, int Count, int precision)
{
  if(Count <= 0)
    return xString();

  TCHAR s[256];
  xString r = ToString(*a++, precision);
  for(Count--; Count > 0; Count--){
    ToString(s, *a++, precision);
    r.Append(_T(" "), sizeof(TCHAR), s, (unsigned)lstrlen(s) * (unsigned)sizeof(TCHAR));
  }
  return r;  
}

int xString::HashData(const void * a, int size)
{
  #define KeyHashAddNum KeyNum = ((KeyNum << 5) + KeyNum) + *Key++
	
  unsigned KeyNum = 5381;
  char * Key = (char*)a;
  for(; size >= 8; size -= 8){
	  KeyHashAddNum;
	  KeyHashAddNum;
	  KeyHashAddNum;
	  KeyHashAddNum;
	  KeyHashAddNum;
	  KeyHashAddNum;
	  KeyHashAddNum;
	  KeyHashAddNum;
  }
  switch(size){
	  case 7: KeyHashAddNum;
	  case 6: KeyHashAddNum;
	  case 5: KeyHashAddNum;
	  case 4: KeyHashAddNum;
	  case 3: KeyHashAddNum;
	  case 2: KeyHashAddNum;
	  case 1: KeyHashAddNum;
	  case 0: break;
  }
  return (int)KeyNum;

  #undef KeyHashAddNum
}