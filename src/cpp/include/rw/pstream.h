#ifndef __RWPSTREAM_H__
#define __RWPSTREAM_H__

/*
 * rwp[io]stream ---    Portable I/O streams (use escape sequences for 
 *                      writing special characters.
 *
 * $Id: pstream.h 208302 2012-09-27 11:56:01Z mt $
 *
 * Copyright (c) 1989-1999 Rogue Wave Software, Inc.  All Rights Reserved.
 *
 * This computer software is owned by Rogue Wave Software, Inc. and is
 * protected by U.S. copyright laws and other laws and by international
 * treaties.  This computer software is furnished by Rogue Wave Software,
 * Inc. pursuant to a written license agreement and may be used, copied,
 * transmitted, and stored only in accordance with the terms of such
 * license and with the inclusion of the above copyright notice.  This
 * computer software or any other copies thereof may not be provided or
 * otherwise made available to any other person.
 *
 * U.S. Government Restricted Rights.  This computer software is provided
 * with Restricted Rights.  Use, duplication, or disclosure by the
 * Government is subject to restrictions as set forth in subparagraph (c)
 * (1) (ii) of The Rights in Technical Data and Computer Software clause
 * at DFARS 252.227-7013 or subparagraphs (c) (1) and (2) of the
 * Commercial Computer Software – Restricted Rights at 48 CFR 52.227-19,
 * as applicable.  Manufacturer is Rogue Wave Software, Inc., 5500
 * Flatiron Parkway, Boulder, Colorado 80301 USA.
 *
 ***************************************************************************/

#include "rw/vstream.h"

/************************************************
 *                                              *
 *              class RWpistream                *
 *                                              *
 ************************************************/

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWpistream : public RWvistream
{

public:

  RWpistream(RW_SL_IO_STD(istream)& str);
  RWpistream(RW_SL_IO_STD(streambuf)* s);
  virtual ~RWpistream();

  virtual int           eof()          {return istr_.eof();    }
  virtual int           fail()         {return istr_.fail();   }
  virtual int           bad()          {return istr_.bad();    }
  virtual int           good()         {return istr_.good();   }
  virtual int           rdstate()      {return istr_.rdstate();}
  virtual void          clear(int v=0) {       istr_.clear(v); }
  RW_SL_IO_STD(streambuf)*            rdbuf()        {return istr_.rdbuf();  }

  virtual int           get();
  virtual RWvistream&   get(char&);
#ifndef RW_NO_OVERLOAD_WCHAR
  virtual RWvistream&   get(wchar_t&);
#endif
#ifndef RW_NO_OVERLOAD_UCHAR
  virtual RWvistream&   get(unsigned char&);
#endif
#ifndef RW_NO_BOOL
  virtual RWvistream&   get(RWBoolean& b);
  virtual RWvistream&   get(RWBoolean*, size_t N);
#endif
  virtual RWvistream&   get(char*,   size_t N);
#ifdef RW_SEGMENTED_ARCHITECTURE
  virtual RWvistream&   get(char rwhuge*, unsigned long N);
#endif
  virtual RWvistream&   get(double*, size_t N);
  virtual RWvistream&   get(float*,  size_t N);
  virtual RWvistream&   get(int*,    size_t N);
  virtual RWvistream&   get(long*,   size_t N);
  virtual RWvistream&   get(short*,  size_t N);
#ifndef RW_NO_OVERLOAD_WCHAR
  virtual RWvistream&   get(wchar_t*,size_t N);
#endif
#ifndef RW_NO_OVERLOAD_UCHAR
  virtual RWvistream&   get(unsigned char*,  size_t N);
#endif
  virtual RWvistream&   get(unsigned short*, size_t N);
  virtual RWvistream&   get(unsigned int*,   size_t N);
  virtual RWvistream&   get(unsigned long*,  size_t N);
  virtual RWvistream&   getString(char* s,   size_t maxlen);
#ifdef RW_FIX_XSIZE_T
  virtual RWvistream&   getSizeT(size_t&);
#endif
  virtual RWvistream&   operator>>(char&);
  virtual RWvistream&   operator>>(bool& b) { return this->get(b); }
  virtual RWvistream&   operator>>(double&);
  virtual RWvistream&   operator>>(float&);
  virtual RWvistream&   operator>>(int&);
  virtual RWvistream&   operator>>(long&);
  virtual RWvistream&   operator>>(short&);

#ifndef RW_NO_OVERLOAD_WCHAR
  virtual RWvistream&   operator>>(wchar_t&);
#endif
#ifndef RW_NO_OVERLOAD_UCHAR
  virtual RWvistream&   operator>>(unsigned char&);
#endif
  virtual RWvistream&   operator>>(unsigned int&);
  virtual RWvistream&   operator>>(unsigned long&);
  virtual RWvistream&   operator>>(unsigned short&);
protected:
  void                  syntaxErr(const char* expect, char was);
  char                  getCChar();
  RW_SL_IO_STD(istream)               istr_;
private:
  // Disallow copying:
  RWpistream(const RWpistream&);
  RWpistream& operator=(const RWpistream&);
};

/************************************************
 *                                              *
 *              class RWpostream                *
 *                                              *
 ************************************************/

class RWExport RWpostream : public RWvostream
{

public:

  RWpostream(RW_SL_IO_STD(ostream)& str);
  RWpostream(RW_SL_IO_STD(streambuf)* s);
  virtual ~RWpostream();

  virtual int           eof()          {return ostr_.eof();    }
  virtual int           fail()         {return ostr_.fail();   }
  virtual int           bad()          {return ostr_.bad();    }
  virtual int           good()         {return ostr_.good();   }
  virtual int           rdstate()      {return ostr_.rdstate();}
  virtual void          clear(int v=0) {       ostr_.clear(v); }
  RW_SL_IO_STD(streambuf)*            rdbuf()        {return ostr_.rdbuf();  }
  
  RW_SL_IO_STD(streamsize)                   precision(int p)  { return ostr_.precision(p); }
// Some platforms do not have ostream::precision() as const fn.
#ifdef RW_NO_OSTR_REF_CAST
  std::streamsize       precision() const { return ostr_.precision(); } // AON 22.08.2012: Removed VS2010 compiler warning: Changed return type from int to std::streamsize.
#else
  RW_SL_IO_STD(streamsize)                   precision() const
        { return ((RW_SL_IO_STD(ostream)&)ostr_).precision(); }
#endif  

  virtual RWvostream&   putString(const char*, size_t);
  virtual RWvostream&   operator<<(const char*);
  virtual RWvostream&   operator<<(char);
#ifndef RW_NO_OVERLOAD_WCHAR
  virtual RWvostream&   operator<<(wchar_t);
#endif
#ifndef RW_NO_OVERLOAD_UCHAR
  virtual RWvostream&   operator<<(unsigned char);
#endif
  virtual RWvostream&   operator<<(bool b) {return this->put(b);}
  virtual RWvostream&   operator<<(double);
  virtual RWvostream&   operator<<(float);
  virtual RWvostream&   operator<<(int);
  virtual RWvostream&   operator<<(unsigned int);
  virtual RWvostream&   operator<<(long);
  virtual RWvostream&   operator<<(unsigned long);
  virtual RWvostream&   operator<<(short);
  virtual RWvostream&   operator<<(unsigned short);

    
#ifndef RW_NAMESPACE_COLLISION_MSVC_BUG
  virtual RWvostream& flush();
#else
  virtual RWvostream& vflush();
  virtual RWvostream& rw_iomanip(
    RW_SL_IO_STD(ostream)& (*f)(RW_SL_IO_STD(ostream)&));
#endif



  virtual RWvostream&   put(char);
#ifndef RW_NO_BOOL
  virtual RWvostream&   put(RWBoolean);
  virtual RWvostream&   put(const RWBoolean* p, size_t N);
#endif
  virtual RWvostream&   put(const char* p, size_t N);
#ifdef RW_SEGMENTED_ARCHITECTURE
  virtual RWvostream&   put(const char rwhuge* p, unsigned long N);
#endif
#ifndef RW_NO_OVERLOAD_WCHAR
  virtual RWvostream&   put(wchar_t);
  virtual RWvostream&   put(const wchar_t* p, size_t N);
#endif
#ifndef RW_NO_OVERLOAD_UCHAR
  virtual RWvostream&   put(unsigned char);
  virtual RWvostream&   put(const unsigned char* p,  size_t N);
#endif
  virtual RWvostream&   put(const short* p,          size_t N);
  virtual RWvostream&   put(const unsigned short* p, size_t N);
  virtual RWvostream&   put(const int* p,            size_t N);
  virtual RWvostream&   put(const unsigned int* p,   size_t N);
  virtual RWvostream&   put(const long* p,           size_t N);
  virtual RWvostream&   put(const unsigned long* p,  size_t N);
  virtual RWvostream&   put(const float* p,          size_t N);
  virtual RWvostream&   put(const double* p,         size_t N);
#ifdef RW_FIX_XSIZE_T
  virtual RWvostream&   putSizeT(size_t);
#endif
protected:
  static const unsigned MAXCOL;
  void                  putwrap(char);
#ifndef RW_NO_OVERLOAD_WCHAR
  void                  putwrap(wchar_t c);
#endif
#ifndef RW_NO_OVERLOAD_UCHAR
  void                  putwrap(unsigned char c);
#endif
  void                  putwrap(const char* s, size_t len);
  void                  putCChars(const char* s, size_t len);
  RW_SL_IO_STD(ostream)    ostr_;
  unsigned              column_;
private:
  // Disallow copying:
  RWpostream(const RWpostream&);
  RWpostream& operator=(const RWpostream&);
};


#ifndef RW_NAMESPACE_COLLISION_MSVC_BUG

#else
RWvostream&  operator<<(RWpostream& v, RWvostream& (*f)(RWvostream&));
RWvostream&  operator<<(RWpostream& v, 
			RW_SL_IO_STD(ostream)& (*f)(RW_SL_IO_STD(ostream)&));
#endif


#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWPSTREAM_H__ */
