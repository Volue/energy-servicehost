#ifndef __RWAUDIT_STREAMBUFFER_H__
#define __RWAUDIT_STREAMBUFFER_H__
/*
 * A streambuf that counts and allows you to "audit" each byte as it
 * passes through the buffer.
 *
 * $Id: auditbuf.h 18623 2000-03-03 08:28:13Z sih $
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
 ***************************************************************************
 */

#include "rw/rstream.h"
//#include "rw/defs.h"
#if !defined(RW_NO_IOSTD)
#  if defined(RW_MSVC_STDLIB) || defined(RW_NO_IOS_HEADER)
#    include <iosfwd>     // typedef for streamsize the MSVC way
#  else
#    include <ios>        // typedef for streamsize the standard way
#  endif
#else
#  ifndef __64BIT__
typedef int streamsize; // pre-standard, streamsize was an int
#  else
typedef long streamsize;  // 64 bit IBM needs this
#  endif /*__64BIT__*/
#endif

typedef void (*RWauditFunction)(unsigned char, void*);

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWAuditStreamBuffer : public RW_SL_IO_STD(streambuf) {
  unsigned long     seenBytes_;
  RWauditFunction   funcP_;
  void*             funcV_;
  RW_SL_IO_STD(streambuf)*        realBuf_;
  char              myBuf[1]; // Required for underflow and sputbackc.

public:
  RWAuditStreamBuffer(RWauditFunction cb=0, void* x=0);

  RWAuditStreamBuffer(RW_SL_IO_STD(istream)& is, RWauditFunction cb=0, void* x=0);

#ifndef RW_NO_IOSSTREAM		// jvd
  RWAuditStreamBuffer(RW_SL_IO_STD(iostream)& os, RWauditFunction cb=0, void* x=0);
#endif

  RWAuditStreamBuffer(RW_SL_IO_STD(ostream)& os, RWauditFunction cb=0, void* x=0);

  RWAuditStreamBuffer(RW_SL_IO_STD(streambuf)* sb, RWauditFunction cb=0, void* x=0);

  virtual ~RWAuditStreamBuffer();

  operator unsigned long() { return seenBytes_; }

  unsigned long reset(unsigned long val = 0ul) 
  { unsigned long ret = seenBytes_; seenBytes_ = val; return ret; }

  RW_SL_IO_STD(streambuf)* setbuf(char*, RW_SL_IO_STD(streamsize)) {
    /* does nothing: You may NOT set a buffer for this streambuf! */
    return 0;
  }

protected:
// input
  virtual int underflow();
  virtual RW_SL_IO_STD(streamsize) xsgetn(char* s, RW_SL_IO_STD(streamsize) n);
// output
  virtual int overflow(int i);
  RW_SL_IO_STD(streamsize) xsputn(const char* s, RW_SL_IO_STD(streamsize) n);
};

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWAUDIT_STREAMBUFFER_H__ */
