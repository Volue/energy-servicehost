#ifndef __RWBSTREAM_H__
#define __RWBSTREAM_H__

/*
 * rwb[io]stream ---    Use Binary reads and writes
 *
 * $Id: bstream.h 18623 2000-03-03 08:28:13Z sih $
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
 *              class RWbistream                *
 *                                              *
 ************************************************/

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWbistream : public virtual RW_SL_IO_STD(ios), public RWvistream
{

public:

  RWbistream(RW_SL_IO_STD(istream)& str);
  RWbistream(RW_SL_IO_STD(streambuf)* s);
  virtual ~RWbistream();

  virtual int           eof()          {return ((RW_SL_IO_STD(ios)*)(this))->eof();    }
  virtual int           fail()         {return ((RW_SL_IO_STD(ios)*)(this))->fail();   }
  virtual int           bad()          {return ((RW_SL_IO_STD(ios)*)(this))->bad();    }
  virtual int           good()         {return ((RW_SL_IO_STD(ios)*)(this))->good();   }
  virtual int           rdstate()      {return ((RW_SL_IO_STD(ios)*)(this))->rdstate();}
  virtual void          clear(int v=0) {       ((RW_SL_IO_STD(ios)*)(this))->clear(v); }

#ifndef __ATT__
  operator void*()      { return RWvistream::operator void*(); }
#endif
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
  virtual RWvistream&   get(char*  , size_t);
#ifdef RW_SEGMENTED_ARCHITECTURE
  virtual RWvistream&   get(char rwhuge*    , unsigned long);
#endif
#ifndef RW_NO_OVERLOAD_UCHAR
  virtual RWvistream&   get(unsigned char*  , size_t);
#endif
  virtual RWvistream&   get(         double*, size_t);
  virtual RWvistream&   get(         float* , size_t);
  virtual RWvistream&   get(         int*   , size_t);
  virtual RWvistream&   get(unsigned int*   , size_t);
  virtual RWvistream&   get(         long*  , size_t);
  virtual RWvistream&   get(unsigned long*  , size_t);
  virtual RWvistream&   get(         short* , size_t);
  virtual RWvistream&   get(unsigned short* , size_t);
#ifndef RW_NO_OVERLOAD_WCHAR
  virtual RWvistream&   get(wchar_t*, size_t N);
#endif
  virtual RWvistream&   getString(char* s  , size_t maxlen);
#ifdef RW_FIX_XSIZE_T
  virtual RWvistream&   getSizeT(size_t&);
#endif
  virtual RWvistream&   operator>>(char&  );
  virtual RWvistream&   operator>>(bool& b) { return this->get(b); }
  virtual RWvistream&   operator>>(double&);
  virtual RWvistream&   operator>>(float& );
  virtual RWvistream&   operator>>(int&   );
  virtual RWvistream&   operator>>(long&  );
  virtual RWvistream&   operator>>(short& );
#ifndef RW_NO_OVERLOAD_WCHAR
  virtual RWvistream&   operator>>(wchar_t&);
#endif
#ifndef RW_NO_OVERLOAD_UCHAR
  virtual RWvistream&   operator>>(unsigned char& );
#endif
  virtual RWvistream&   operator>>(unsigned int&  );
  virtual RWvistream&   operator>>(unsigned long& );
  virtual RWvistream&   operator>>(unsigned short&);

};

/************************************************
 *                                              *
 *              class RWbostream                *
 *                                              *
 ************************************************/

class RWExport RWbostream : public virtual RW_SL_IO_STD(ios), public RWvostream
{

public:

  RWbostream(RW_SL_IO_STD(ostream)& str);
  RWbostream(RW_SL_IO_STD(streambuf)* s);
  virtual ~RWbostream();

  virtual int           eof()          {return ((RW_SL_IO_STD(ios)*)(this))->eof();    }
  virtual int           fail()         {return ((RW_SL_IO_STD(ios)*)(this))->fail();   }
  virtual int           bad()          {return ((RW_SL_IO_STD(ios)*)(this))->bad();    }
  virtual int           good()         {return ((RW_SL_IO_STD(ios)*)(this))->good();   }
  virtual int           rdstate()      {return ((RW_SL_IO_STD(ios)*)(this))->rdstate();}
  virtual void          clear(int v=0) {       ((RW_SL_IO_STD(ios)*)(this))->clear(v); }

#ifndef __ATT__
  operator void*()      { return RWvostream::operator void*(); }
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
  virtual RWvostream&   operator<<(         bool b) {return this->put(b);}
  virtual RWvostream&   operator<<(         double);
  virtual RWvostream&   operator<<(         float );
  virtual RWvostream&   operator<<(         int   );
  virtual RWvostream&   operator<<(unsigned int   );
  virtual RWvostream&   operator<<(         long  );
  virtual RWvostream&   operator<<(unsigned long  );
  virtual RWvostream&   operator<<(         short );
  virtual RWvostream&   operator<<(unsigned short );

    
#ifndef RW_NAMESPACE_COLLISION_MSVC_BUG
  virtual RWvostream& flush();
#else
  virtual RWvostream& vflush();
  virtual RWvostream& rw_iomanip(RW_SL_IO_STD(ostream)& (*)(RW_SL_IO_STD(ostream)&));
#endif


  virtual RWvostream&   put(char);
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
  virtual RWvostream&   put(const unsigned char* p, size_t N);
#endif
#ifndef RW_NO_BOOL
  virtual RWvostream&   put(RWBoolean);
  virtual RWvostream&   put(const RWBoolean* p, size_t N);
#endif
  virtual RWvostream&   put(const          short* , size_t);
  virtual RWvostream&   put(const unsigned short* , size_t);
  virtual RWvostream&   put(const          int*   , size_t);
  virtual RWvostream&   put(const unsigned int*   , size_t);
  virtual RWvostream&   put(const          long*  , size_t);
  virtual RWvostream&   put(const unsigned long*  , size_t);
  virtual RWvostream&   put(const          float* , size_t);
  virtual RWvostream&   put(const          double*, size_t);
#ifdef RW_FIX_XSIZE_T
  virtual RWvostream&   putSizeT(size_t);
#endif
};


#ifndef RW_NAMESPACE_COLLISION_MSVC_BUG

#else
RWvostream&  operator<<(RWbostream& v, RWvostream& (*f)(RWvostream&));
RWvostream&  operator<<(RWbostream& v, 
			RW_SL_IO_STD(ostream)& (*f)(RW_SL_IO_STD(ostream)&));
#endif


#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWBSTREAM_H__ */

