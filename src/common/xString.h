#ifndef __x_string_h__
#define __x_string_h__

#pragma once

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <stdarg.h>
#include "xdef.h"

#ifndef  _UNICODE
#error _UNICODE must be defined???
#endif

class xString
{
protected:

  friend class xRefClass;

  union { // for show string in debuger
    TCHAR * str;
    char * c_str;
  };

  static bool initialized;
  static xString * pEmpty;

  struct Data
  {
    int numRefs;
    int size;

    X_INLINE TCHAR * ToChar(){ return (TCHAR*)(this + 1); }
    // char * ToMemory() { return (char*)(this + 1); }

    static Data * Alloc(int size, int pad);
    void Release();
    void TerminateData();
  };
  //static char EmptyStringData[];
  //static const Data * EmptyData;

  X_INLINE Data * ToStringData(){ return ((Data*)str)-1; }
  X_INLINE void Release(){ ToStringData()->Release(); }

  X_INLINE void Init(const xString& a){ str = a.str; ToStringData()->numRefs++; }
  void Init(const void * p, int size);
  void Init(const void * p1, int size1, const void * p2, int size2);
  void Init(const void * p1, int size1, const void * p2, int size2, const void * p3, int size3);
  X_INLINE void Init(const TCHAR * s){ Init((const void*)s, (unsigned)lstrlen(s) * (unsigned)sizeof(TCHAR)); }
  X_INLINE void Init(const TCHAR * s, int len){ Init((const void*)s, (unsigned)len * (unsigned)sizeof(TCHAR)); }
  X_INLINE void Init(TCHAR c){ Init((const void*)&c, sizeof(TCHAR)); }

#ifdef  _UNICODE
  void Init(const char * s);
#endif

  void Init(int size);

public:

  // static const int Granularity = 16;

  static void Init();
  static void __cdecl Shutdown();

  X_INLINE operator const TCHAR * () const { return (const TCHAR*)str; }
  X_INLINE const TCHAR * ToChar() const { return (const TCHAR*)str; }
  X_INLINE const void * ToMemory() const { return (const void*)str; }
  X_INLINE int NumRefs() const { return ((xString*)this)->ToStringData()->numRefs; }
  X_INLINE int Size() const { return ((xString*)this)->ToStringData()->size; }
  X_INLINE int Len() const { return (unsigned)Size() / (unsigned)sizeof(TCHAR); }

  X_INLINE xString(){ Init(*pEmpty); }
  X_INLINE xString(const xString& s){ Init(s); }
  X_INLINE xString(const xString& a, const xString& b){ Init((const void*)a.str, a.Size(), (const void*)b.str, b.Size()); }
  X_INLINE xString(const void * p, int size){ Init(p, size); }
  X_INLINE xString(const void * p1, int size1, const void * p2, int size2){ Init(p1, size1, p2, size2); }
  
  X_INLINE xString(TCHAR * s){ Init((const TCHAR*)s); }
  X_INLINE xString(const TCHAR * s){ Init(s); }
  
  X_INLINE xString(TCHAR * s, int len){ Init((const TCHAR*)s, len); }
  X_INLINE xString(const TCHAR * s, int len){ Init(s, len); }

#ifdef  _UNICODE
  X_INLINE xString(char * s){ Init((const char*)s); }
  X_INLINE xString(const char * s){ Init(s); }
#endif

  X_INLINE xString(TCHAR c){ Init(c); }

  X_INLINE xString(int size){ Init(size); }
  // xString(int Value){ Init(*pEmpty); SetFormat(_T("%i"), Value); }
  X_INLINE ~xString(){ Release(); }

  X_INLINE bool IsEmpty() const { return Size() == 0; }
  X_INLINE xString& Clear(){ return Assign(*pEmpty); }
  
  xString& SetSize(int size, int pad, bool keepData = false);
  X_INLINE xString& SetLen(int len, int pad, bool keepData = false)
  { 
    return SetSize((unsigned)len * (unsigned)sizeof(TCHAR), (unsigned)pad * (unsigned)sizeof(TCHAR), keepData); 
  }

  xString& SetFormat(const TCHAR * fmt, va_list va);
  X_INLINE xString& SetFormat(TCHAR * fmt, va_list va){ return SetFormat((const TCHAR*)fmt, va); }
  
  X_INLINE xString& SetFormat(const TCHAR * fmt, ...)
  { 
    va_list va;
    va_start(va,fmt);
    SetFormat(fmt, va);
    va_end(va);
    return *this;
  }
  X_INLINE xString& SetFormat(TCHAR * fmt, ...)
  { 
    va_list va;
    va_start(va,fmt);
    SetFormat(fmt, va);
    va_end(va);
    return *this;
  }

  static X_INLINE xString Format(const TCHAR * fmt, va_list va){ return xString().SetFormat(fmt, va); }
  static X_INLINE xString Format(TCHAR * fmt, va_list va){ return Format((const TCHAR*)fmt, va); }
  
  static X_INLINE xString Format(const TCHAR * fmt, ...)
  { 
    va_list va;
    va_start(va,fmt);
    xString s = Format(fmt, va);
    va_end(va);
    return s;
  }
  static X_INLINE xString Format(TCHAR * fmt, ...)
  { 
    va_list va;
    va_start(va,fmt);
    xString s = Format(fmt, va);
    va_end(va);
    return s;
  }

  xString& Separate();
  xString Clone() const;
  
  xString& Assign(const xString& a);
  xString& Assign(const void * p, int size);
  xString& Assign(const void * p1, int size1, const void * p2, int size2);
  X_INLINE xString& Assign(const TCHAR * s){ return Assign((const void*)s, (unsigned)lstrlen(s) * (unsigned)sizeof(TCHAR)); }
  X_INLINE xString& Assign(const TCHAR * s, int len){ return Assign((const void*)s, (unsigned)len * (unsigned)sizeof(TCHAR)); }
  X_INLINE xString& Assign(TCHAR c){ return Assign((const void*)&c, sizeof(TCHAR)); }

  X_INLINE xString& operator=(const xString& a){ return Assign(a); }
  X_INLINE xString& operator=(TCHAR * a){ return Assign((const TCHAR*)a); }
  X_INLINE xString& operator=(const TCHAR * a){ return Assign(a); }
  X_INLINE xString& operator=(TCHAR c){ return Assign(c); }

  TCHAR Char(int i) const;
	xString& SetChar(int i, TCHAR c);

  X_INLINE xString& Insert(int i, const xString& a){ return InsertData((unsigned)i * (unsigned)sizeof(TCHAR), a); }
  X_INLINE xString& Insert(int i, const TCHAR * s){ return InsertData((unsigned)i * (unsigned)sizeof(TCHAR), (const void*)s, (unsigned)lstrlen(s) * (unsigned)sizeof(TCHAR)); }
  X_INLINE xString& Insert(int i, const TCHAR * s, int len){ return InsertData((unsigned)i * (unsigned)sizeof(TCHAR), (const void*)s, (unsigned)len * (unsigned)sizeof(TCHAR)); }
  X_INLINE xString& Insert(int i, TCHAR c){ return InsertData((unsigned)i * (unsigned)sizeof(TCHAR), (const void*)&c, sizeof(TCHAR)); }

  xString& InsertData(int i, const xString& a);
  xString& InsertData(int i, const void * p, int size);

  xString SubString(int start,int len) const;
  xString SubData(int start,int len) const;

  xString Lower() const;
  xString Upper() const;
  xString Flower() const; // 1st char upper, else lower

  xString Trim(bool leftTrim = true, bool rightTrim = true) const;

  xString& Append(const xString& a);
  xString& Append(const void * p, int size);
  xString& Append(const void * p1, int size1, const void * p2, int size2);
  X_INLINE xString& Append(const TCHAR * s){ return Append((const void*)s, (unsigned)lstrlen(s) * (unsigned)sizeof(TCHAR)); }
  X_INLINE xString& Append(const TCHAR * s, int len){ return Append((const void*)s, (unsigned)len * (unsigned)sizeof(TCHAR)); }
  X_INLINE xString& Append(TCHAR c){ return Append((const void*)&c, sizeof(TCHAR)); }

  X_INLINE xString& operator+=(const xString& b){ return Append(b); }
  X_INLINE xString& operator+=(TCHAR * b){ return Append((const TCHAR*)b); }
  X_INLINE xString& operator+=(const TCHAR * b){ return Append(b); }
  X_INLINE xString& operator+=(TCHAR c){ return Append(c); }
  
  friend X_INLINE xString operator+(const xString& a,const xString& b){ return xString(a, b); }

  friend X_INLINE xString operator+(TCHAR * a,const xString& b){ return xString((const void*)a, (unsigned)lstrlen(a) * (unsigned)sizeof(TCHAR), b.str, b.Size()); }
  friend X_INLINE xString operator+(const TCHAR * a,const xString& b){ return xString((const void*)a, (unsigned)lstrlen(a) * (unsigned)sizeof(TCHAR), b.str, b.Size()); }
  
  friend X_INLINE xString operator+(const xString& a,TCHAR * b){ return xString(a.str,a.Size(), (const void*)b, (unsigned)lstrlen(b) * (unsigned)sizeof(TCHAR)); }
  friend X_INLINE xString operator+(const xString& a,const TCHAR * b){ return xString(a.str, a.Size(), (const void*)b, (unsigned)lstrlen(b) * (unsigned)sizeof(TCHAR)); }
  
  friend X_INLINE xString operator+(char c,const xString& b){ return xString((const void*)&c, (unsigned)sizeof(TCHAR), b.str, b.Size()); }
  friend X_INLINE xString operator+(const xString& a,char c){ return xString(a.str, a.Size(), (const void*)&c, (unsigned)sizeof(TCHAR)); }

  static int Cmp(const TCHAR * a, int aLen, const TCHAR * b, int bLen, int n = 0x7fffffff / sizeof(TCHAR));
  static int Icmp(const TCHAR * a, int aLen, const TCHAR * b, int bLen, int n = 0x7fffffff / sizeof(TCHAR));

  int Cmp (const TCHAR * b, int n = 0x7fffffff / sizeof(TCHAR)) const;
  int Icmp (const TCHAR * b, int n = 0x7fffffff / sizeof(TCHAR)) const;

  int Cmp (const xString& b, int n = 0x7fffffff / sizeof(TCHAR)) const;
  int Icmp (const xString& b, int n = 0x7fffffff / sizeof(TCHAR)) const;

  static int CmpData(const void * a, int aSize, const void * b, int bSize, int n = 0x7fffffff);
  static int HashData(const void * a, int size);

  X_INLINE int Hash() const { return HashData((const void*)str, Size()); }

  friend X_INLINE bool operator==(const xString& a, const xString& b){ return CmpData(a.ToMemory(),       a.Size(),    b.ToMemory(), b.Size()) == 0; }
  friend X_INLINE bool operator==(TCHAR * a,        const xString& b){ return CmpData((const TCHAR*)a, (unsigned)lstrlen(a) * (unsigned)sizeof(TCHAR), b.ToMemory(), b.Size()) == 0; }
  friend X_INLINE bool operator==(const TCHAR * a,  const xString& b){ return CmpData((const TCHAR*)a, (unsigned)lstrlen(a) * (unsigned)sizeof(TCHAR), b.ToMemory(), b.Size()) == 0; }
  friend X_INLINE bool operator==(const xString& a, TCHAR * b)       { return CmpData(a.ToMemory(), a.Size(), (const TCHAR*)b, (unsigned)lstrlen(b) * (unsigned)sizeof(TCHAR)) == 0; }
  friend X_INLINE bool operator==(const xString& a, const TCHAR * b) { return CmpData(a.ToMemory(), a.Size(), (const TCHAR*)b, (unsigned)lstrlen(b) * (unsigned)sizeof(TCHAR)) == 0; }

  friend X_INLINE bool operator!=(const xString& a, const xString& b){ return CmpData(a.ToMemory(), a.Size(), b.ToMemory(), b.Size()) != 0; }
  friend X_INLINE bool operator!=(TCHAR * a,        const xString& b){ return CmpData((const TCHAR*)a, (unsigned)lstrlen(a) * (unsigned)sizeof(TCHAR), b.ToMemory(), b.Size()) != 0; }
  friend X_INLINE bool operator!=(const TCHAR * a,  const xString& b){ return CmpData((const TCHAR*)a, (unsigned)lstrlen(a) * (unsigned)sizeof(TCHAR), b.ToMemory(), b.Size()) != 0; }
  friend X_INLINE bool operator!=(const xString& a, TCHAR * b)       { return CmpData(a.ToMemory(), a.Size(), (const TCHAR*)b, (unsigned)lstrlen(b) * (unsigned)sizeof(TCHAR)) != 0; }
  friend X_INLINE bool operator!=(const xString& a, const TCHAR * b) { return CmpData(a.ToMemory(), a.Size(), (const TCHAR*)b, (unsigned)lstrlen(b) * (unsigned)sizeof(TCHAR)) != 0; }

  friend X_INLINE bool operator<=(const xString& a, const xString& b){ return CmpData(a.ToMemory(), a.Size(), b.ToMemory(), b.Size()) <= 0; }
  friend X_INLINE bool operator<=(TCHAR * a,        const xString& b){ return CmpData((const TCHAR*)a, (unsigned)lstrlen(a) * (unsigned)sizeof(TCHAR), b.ToMemory(), b.Size()) <= 0; }
  friend X_INLINE bool operator<=(const TCHAR * a,  const xString& b){ return CmpData((const TCHAR*)a, (unsigned)lstrlen(a) * (unsigned)sizeof(TCHAR), b.ToMemory(), b.Size()) <= 0; }
  friend X_INLINE bool operator<=(const xString& a, TCHAR * b)       { return CmpData(a.ToMemory(), a.Size(), (const TCHAR*)b, (unsigned)lstrlen(b) * (unsigned)sizeof(TCHAR)) <= 0; }
  friend X_INLINE bool operator<=(const xString& a, const TCHAR * b) { return CmpData(a.ToMemory(), a.Size(), (const TCHAR*)b, (unsigned)lstrlen(b) * (unsigned)sizeof(TCHAR)) <= 0; }

  friend X_INLINE bool operator< (const xString& a, const xString& b){ return CmpData(a.ToMemory(), a.Size(), b.ToMemory(), b.Size()) < 0; }
  friend X_INLINE bool operator< (TCHAR * a,        const xString& b){ return CmpData((const TCHAR*)a, (unsigned)lstrlen(a) * (unsigned)sizeof(TCHAR), b.ToMemory(), b.Size()) < 0; }
  friend X_INLINE bool operator< (const TCHAR * a,  const xString& b){ return CmpData((const TCHAR*)a, (unsigned)lstrlen(a) * (unsigned)sizeof(TCHAR), b.ToMemory(), b.Size()) < 0; }
  friend X_INLINE bool operator< (const xString& a, TCHAR * b)       { return CmpData(a.ToMemory(), a.Size(), (const TCHAR*)b, (unsigned)lstrlen(b) * (unsigned)sizeof(TCHAR)) < 0; }
  friend X_INLINE bool operator< (const xString& a, const TCHAR * b) { return CmpData(a.ToMemory(), a.Size(), (const TCHAR*)b, (unsigned)lstrlen(b) * (unsigned)sizeof(TCHAR)) < 0; }

  friend X_INLINE bool operator>=(const xString& a, const xString& b){ return CmpData(a.ToMemory(), a.Size(), b.ToMemory(), b.Size()) >= 0; }
  friend X_INLINE bool operator>=(TCHAR * a,        const xString& b){ return CmpData((const TCHAR*)a, (unsigned)lstrlen(a) * (unsigned)sizeof(TCHAR), b.ToMemory(), b.Size()) >= 0; }
  friend X_INLINE bool operator>=(const TCHAR * a,  const xString& b){ return CmpData((const TCHAR*)a, (unsigned)lstrlen(a) * (unsigned)sizeof(TCHAR), b.ToMemory(), b.Size()) >= 0; }
  friend X_INLINE bool operator>=(const xString& a, TCHAR * b)       { return CmpData(a.ToMemory(), a.Size(), (const TCHAR*)b, (unsigned)lstrlen(b) * (unsigned)sizeof(TCHAR)) >= 0; }
  friend X_INLINE bool operator>=(const xString& a, const TCHAR * b) { return CmpData(a.ToMemory(), a.Size(), (const TCHAR*)b, (unsigned)lstrlen(b) * (unsigned)sizeof(TCHAR)) >= 0; }

  friend X_INLINE bool operator>(const xString& a, const xString& b){ return CmpData(a.ToMemory(), a.Size(), b.ToMemory(), b.Size()) > 0; }
  friend X_INLINE bool operator>(TCHAR * a,        const xString& b){ return CmpData((const TCHAR*)a, (unsigned)lstrlen(a) * (unsigned)sizeof(TCHAR), b.ToMemory(), b.Size()) > 0; }
  friend X_INLINE bool operator>(const TCHAR * a,  const xString& b){ return CmpData((const TCHAR*)a, (unsigned)lstrlen(a) * (unsigned)sizeof(TCHAR), b.ToMemory(), b.Size()) > 0; }
  friend X_INLINE bool operator>(const xString& a, TCHAR * b)       { return CmpData(a.ToMemory(), a.Size(), (const TCHAR*)b, (unsigned)lstrlen(b) * (unsigned)sizeof(TCHAR)) > 0; }
  friend X_INLINE bool operator>(const xString& a, const TCHAR * b) { return CmpData(a.ToMemory(), a.Size(), (const TCHAR*)b, (unsigned)lstrlen(b) * (unsigned)sizeof(TCHAR)) > 0; }

  static xString ToString(int a);
  static TCHAR * ToString(TCHAR * dst, float a, int precision = 2);
  static xString ToString(float a, int precision = 2);
  static xString FloatArrayToString(const float * a, int Count, int precision = 2);
};

#endif