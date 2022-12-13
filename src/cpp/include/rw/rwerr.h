#ifndef __RWERR_H__
#define __RWERR_H__

/*
 * Error definitions
 *
 * $Id: rwerr.h 18623 2000-03-03 08:28:13Z sih $
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

#include "rw/defs.h"
#include "rw/rstream.h"
STARTWRAP
#include <stdio.h>      /* Looking for FILE */
ENDWRAP
#ifndef RW_NO_XMSG
//#  ifdef sun
#  if defined(sun) || defined(__PowerCpp__) 
#    include <exception.h>
#  else
#    include <except.h>
#  endif
#endif

class RWExport RWMessage;

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                                RWxmsg                                //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

#ifdef RW_NO_XMSG

// No host-supplied xmsg; we must supply it:
class RWExport RWxmsg
{
public:

  RWxmsg(const char*);
  RWxmsg(const RWMessage&);
  RWxmsg(const RWxmsg&);
  virtual ~RWxmsg();

  const char*    why() const    {return msg_; }

  void          raise();

protected:

  RWxmsg(const char*, RWBoolean doCopy);

private:

  void makeCopy(const char* str);
  const char* msg_;             // Message
  RWBoolean   doFree_;
};

#else

/*
 * Compiler-supplied xmsg available.
 * In this case, RWxmsg is merely an interface.
 */

class RWExport RWxmsg : public xmsg
{

public:

  RWxmsg(const char*);
  RWxmsg(const RWMessage&);
  virtual ~RWxmsg();
};

#endif

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                               RWxalloc                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifdef RW_NO_XMSG

// No host-supplied xalloc; we must supply it:
class RWExport RWxalloc : public RWxmsg
{
public:

  RWxalloc(size_t size);
  virtual ~RWxalloc();

  size_t requested() const { return size_; }

private:
  size_t size_;
};

#else


/*
 * Compiler-supplied xalloc available.
 * In this case, RWxalloc is merely an interface.
 */

class RWExport RWxalloc : public xalloc
{
public:

  RWxalloc(size_t size);
  virtual ~RWxalloc();
};

#endif

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                            RWExternalErr                             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

// For errors external to the library;
// these are errors the prudent programmer should expect,
// such as out-of-disk-space, file errors, and the like:

class RWExport RWExternalErr : public RWxmsg
{
public:

  RWExternalErr(const char*);
  RWExternalErr(const RWMessage&);
  virtual ~RWExternalErr();
};

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                              RWFileErr                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

// For errors involving a file descriptor:
class RWExport RWFileErr : public RWExternalErr
{
public:

  enum errType {unknown, seekErr, readErr, writeErr, openErr, closeErr};
  RWFileErr(const char*     , FILE*, errType type);
  RWFileErr(const RWMessage&, FILE*, errType type);
  virtual ~RWFileErr();

  FILE*   descriptor()          { return fd_;   }
  errType errorType()           { return type_; }

private:
  FILE* fd_;
  errType type_;
};

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                             RWStreamErr                              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

// For errors involving streams:
class RWExport RWStreamErr : public RWExternalErr
{
public:

  RWStreamErr(const char*     , RW_SL_IO_STD(ios)&);
  RWStreamErr(const RWMessage&, RW_SL_IO_STD(ios)&);
  virtual ~RWStreamErr();

  RW_SL_IO_STD(ios)&    errStream()          { return stream_; }

private:
  RW_SL_IO_STD(ios)&    stream_;
};

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                            RWInternalErr                             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

// Logic errors are violated invariants, bounds errors,
// and the like.  

class RWExport RWInternalErr : public RWxmsg
{
public:

  RWInternalErr(const char*);
  RWInternalErr(const RWMessage&);
  ~RWInternalErr();
};

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                             RWBoundsErr                              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

class RWExport RWBoundsErr : public RWInternalErr
{
public:

  RWBoundsErr(const char*);
  RWBoundsErr(const RWMessage&);
  virtual ~RWBoundsErr();
};

  // Compiler does not support exception handling:
  // Or cannot throw them from a shared library.
  extern void rwexport RWThrow( const RWxmsg&        );
  extern void rwexport RWThrow( const RWxalloc&      );
  extern void rwexport RWThrow( const RWExternalErr& );
  extern void rwexport RWThrow( const RWFileErr&     );
  extern void rwexport RWThrow( const RWStreamErr&   );
  extern void rwexport RWThrow( const RWInternalErr& );
  extern void rwexport RWThrow( const RWBoundsErr&   );
  typedef void (rwfar *rwErrHandler)(const RWxmsg&);
  rwErrHandler rwexport rwSetErrHandler(rwErrHandler);


// All exceptions are thrown by the library code itself so that
// if exceptions are changed from that of the library, both application
// code and library code throw exceptions identically.

// Use extra parens to skirt MS C7 bug as documented in MS Problem ID: C9301003
# define RWTHROW(a) (RWThrow(a))

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWERR_H__ */
