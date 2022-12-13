#ifndef __RWXDRSTREA_H__
#define __RWXDRSTREA_H__

/*
 * rwxdr[io]stream ---  Portable I/O streams based on XDR format.
 *
 * $Id: xdrstrea.h 18623 2000-03-03 08:28:13Z sih $
 *
 ****************************************************************************
 *
 *      Copyright (C) 1992 Sun Microsystems, Inc.  All Rights Reserved.
 *
 ****************************************************************************
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

#include "rw/compiler.h"
STARTWRAP
#include <stddef.h>     /* Required for IBM xlC */
#include <stdio.h>      /* Required for IBM xlC */
ENDWRAP
#ifdef RW_RPC_TYPES_BUG
/*******************************************************************/
// since hp has made it impossible to use their <rpc/types.h>
// for some of what we do, we will prevent entering it...
#  ifndef _RPC_TYPES_INCLUDED
#  define _RPC_TYPES_INCLUDED
// but since we do that, we had _better_ do everything it did...
#  include <sys/types.h>
#  include <sys/time.h>	       
   extern "C" {
////// including the following weird or apparently useless stuff
#  define bool_t	int
#  define enum_t	int
#  define KU_SERVER 1
#  define KU_CLIENT 0
#  define __dontcare__	(-1)
////// here is what we needed //////////////////////
#  define mem_alloc(bsize)	_rpc_malloc(bsize)
#  define mem_free(ptr, bsize)	_rpc_free(ptr)
   char *_rpc_malloc(u_int);
   void _rpc_free(char*);
////////////////////////////////////////////////////
  }
#endif /* not _RPC_TYPES_INCLUDED */
/*******************************************************************/
#else
STARTWRAP
#  include <rpc/types.h>  /* Due to bug in some rpc/xdr.h. */
ENDWRAP
/* Sun cfront V2.X has bug in prototype for xdr_u_char(); see toolread.doc */
#endif /* RW_RPC_TYPES_BUG */
STARTWRAP
#include <rpc/xdr.h>
ENDWRAP
#include "rw/vstream.h"

class RWExport RWCollectable;

int xdr(XDR*, RWCollectable*&);

/************************************************
 *                                              *
 *              class RWXDRIStream              *
 *                                              *
 ************************************************/

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWXDRistream : public RWvistream, public RWios
{

public:

  RWXDRistream(XDR*);
  RWXDRistream(RW_SL_IO_STD(streambuf*));
  RWXDRistream(RW_SL_IO_STD(istream&));
  virtual ~RWXDRistream();

  virtual int           eof()           {return RWios::eof();    }
  virtual int           fail()          {return RWios::fail();   }
  virtual int           bad()           {return RWios::bad();    }
  virtual int           good()          {return RWios::good();   }
  virtual int           rdstate()       {return RWios::rdstate();}
  virtual void          clear(int v=0)  {       RWios::clear(v); }

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
//virtual RWvistream&   operator>>(bool& b ) { return this->get(b); }
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

private:

  XDR*  xdrp;
  int   streambufXDR; 
};

/************************************************
 *                                              *
 *              class RWXDROStream              *
 *                                              *
 ************************************************/

class RWExport RWXDRostream : public RWvostream, public RWios
{

public:

  RWXDRostream(XDR*);
  RWXDRostream(RW_SL_IO_STD(streambuf*));
  RWXDRostream(RW_SL_IO_STD(ostream&));
  virtual ~RWXDRostream();

  virtual int           eof()           {return RWios::eof();    }
  virtual int           fail()          {return RWios::fail();   }
  virtual int           bad()           {return RWios::bad();    }
  virtual int           good()          {return RWios::good();   }
  virtual int           rdstate()       {return RWios::rdstate();}
  virtual void          clear(int v=0)  {       RWios::clear(v); }

  virtual RWvostream&   putString(const char*, size_t);
  virtual RWvostream&   operator<<(const char*);
  virtual RWvostream&   operator<<(char);
#ifndef RW_NO_OVERLOAD_WCHAR
  virtual RWvostream&   operator<<(wchar_t);
#endif
#ifndef RW_NO_OVERLOAD_UCHAR
  virtual RWvostream&   operator<<(unsigned char);
#endif
//virtual RWvostream&   operator<<(         bool b){ return this->put(b); }
  virtual RWvostream&   operator<<(         double);
  virtual RWvostream&   operator<<(         float );
  virtual RWvostream&   operator<<(         int   );
  virtual RWvostream&   operator<<(unsigned int   );
  virtual RWvostream&   operator<<(         long  );
  virtual RWvostream&   operator<<(unsigned long  );
  virtual RWvostream&   operator<<(         short );
  virtual RWvostream&   operator<<(unsigned short );
  virtual RWvostream&   flush();

  virtual RWvostream&   put(char);
  virtual RWvostream&   put(const char* p, size_t N);
#ifndef RW_NO_BOOL
  virtual RWvostream&   put(RWBoolean);
  virtual RWvostream&   put(const RWBoolean* p, size_t N);
#endif
#ifndef RW_NO_OVERLOAD_WCHAR
  virtual RWvostream&   put(wchar_t);
  virtual RWvostream&   put(const wchar_t* p, size_t N);
#endif
#ifndef RW_NO_OVERLOAD_UCHAR
  virtual RWvostream&   put(unsigned char);
  virtual RWvostream&   put(const unsigned char* p, size_t N);
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

private:

  XDR*  xdrp;
  int   streambufXDR;
};

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWXDRSTREA_H__ */
