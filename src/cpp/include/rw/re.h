#ifndef __RWRE_H__
#define __RWRE_H__

/*
 * Declarations for class RWRegularExpression --- Extended Regular Expression
 *
 * $Id: re.h 18623 2000-03-03 08:28:13Z sih $
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
 *
 * $Log$
 * Revision 1.1.1.1  2000/03/02 21:40:42  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 7.35  1996/08/09 19:09:41  hart
 * HP (aCC compiler) port
 *
 * Revision 7.34  1996/07/18 04:16:54  hart
 * ObjectStore changes
 *
 * Revision 7.33  1996/05/07 22:25:16  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.32  1996/03/25 22:55:40  kevinj
 * Bug #2941: Added RWTEXPORT_RETURN and changed RWTExport to RWExport
 * in RWTRegularExpressionTrait<char> specialization for MSVC DLL's.
 *
 * Revision 7.31  1996/02/18 22:02:20  kyle
 * Millenium changed #include <rw/..> to "rw/.."
 *
 * Revision 7.30  1996/02/18 01:45:34  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.29  1996/02/05 21:41:10  griswolf
 * Add preprocessor error message per Sanders' request.
 *
 * Revision 7.28  1995/12/11 19:13:35  jims
 * Remove value from #define for __RWRE_H__
 *
 * Revision 7.27  1995/10/27 16:05:49  jims
 * move RWREBaseClass to rebase.h
 *
 * Revision 7.26  1995/09/05  19:17:30  jims
 * Use new copyright macro
 *
 * Revision 7.25  1995/08/30  00:19:35  kevinj
 * Added RW_NO_EXCEPTIONS.
 *
 * Revision 7.24  1995/08/29  16:38:19  kevinj
 * Moved RWWRExpr to ToolsPro.
 *
 * Revision 7.23  1995/07/31  23:23:38  jims
 * #include <rw/compiler.h> before testing for RW_NO_STL
 *
 * Revision 7.22  1995/07/28  22:22:04  kevinj
 * Removed redundant #include "rw/rei.h".
 *
 * Revision 7.21  1995/06/27  23:40:49  kevinj
 * #1876: Added RW_NO_STL to guard against futile compilation of classes
 * that depend on the existence of C++ Standard Library containers
 * and iterators.
 *
 * Revision 7.20  1995/01/26  17:39:40  kevinj
 * Bug #1038: Infinite loop on some pattern matching.
 *
 * Revision 7.19  1995/01/21  01:20:02  kevinj
 * Completed port to Solaris Cafe 3.0.
 *
 * Revision 7.19  1995/01/21  01:20:02  kevinj
 * Completed port to Solaris Cafe 3.0.
 *
 * Revision 7.18  1995/01/16  21:21:56  kevinj
 * Optimized for speed with RWCRegexp.
 *
 * Revision 7.18  1995/01/16  21:21:56  kevinj
 * Optimized for speed with RWCRegexp.
 *
 * Revision 7.17  1995/01/10  21:12:06  kevinj
 * Ported to Solaris Cafe.
 *
 * Revision 7.16  1994/12/30  23:56:44  kevinj
 * Added RW_NO_POSIX_RE.  Made classes MT safe.  Commented and cleaned up code.
 * Combined NFA arrays in regex_t into one array called "node".
 * Removed RWREMatch.
 *
 * Revision 7.16  1994/12/30  23:56:44  kevinj
 * Added RW_NO_POSIX_RE.  Made classes MT safe.  Commented and cleaned up code.
 * Combined NFA arrays in regex_t into one array called "node".
 * Removed RWREMatch.
 */

#include "rw/compiler.h"

#ifdef RW_MAXINT_CONFLICT 
#define _SYS_PARAM_INCLUDED
#endif

#if !defined(RW_NO_STL) && !defined(RW_NO_EXCEPTIONS)

#include "rw/rei.h"     // Regular expression implementation class.

/////////////////////////////////////////////////////////////////////////////
//
// RWTRegularExpression<charT>
//
//  POSIX.2 extended regular expression interface class.
//
/////////////////////////////////////////////////////////////////////////////

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template<class charT> class RWTExport RWTRegularExpression
  : public RWREBaseClass
{
public:
  typedef typename RWTRegularExpressionTraits<charT>::tStringT     stringT;

  RWTRegularExpression() {}
  RWTRegularExpression(const charT* reCharPtr)  : impl_(reCharPtr)  {}
  RWTRegularExpression(const stringT& reString) : impl_(reString)   {}
  RWTRegularExpression(const RWTRegularExpression<charT>&  other)
    : impl_(other.impl_) {}
  virtual ~RWTRegularExpression() {}

  RWTRegularExpression<charT>& operator=
    (const RWTRegularExpression<charT>& other)
    {impl_ = other.impl_; return *this;}

  RWTRegularExpression<charT>& operator=(const charT* reString)
    {impl_ = reString; return *this;}

  RWTRegularExpression<charT>& operator=(const stringT& string)
    {impl_ = string; return *this;}

  size_t                          index
                          (
                            const stringT&  str,
                            size_t*         len       = NULL,
                            size_t          start     = 0
                          )
                          {return impl_.index(str, len, start);}

  statusType            status() const {return impl_.status();}

private:
  RWTRegularExpressionImp<charT>  impl_;  // Regular expression implementation.

  RW_T_TYPESPEC  /* This macro usually expands to nothing */

};  // RWTRegularExpression

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#undef RWC

typedef RWTRegularExpression<char> RWCRExpr;

#if !defined(RW_NO_WSTR) && defined(RW_TOOLSPRO)
  typedef RWTRegularExpression<wchar_t> RWWRExpr;
#endif

#else
#error You must have both Standard Library and exceptions to use this class.
#endif // !defined(RW_NO_STL)

#endif /* __RWRE_H__ */
