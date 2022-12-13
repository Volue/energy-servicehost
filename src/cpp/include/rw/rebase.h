#ifndef __RWREBASE_H__
#define __RWREBASE_H__

/*
 * Declaration for class RWREBaseClass
 *
 * $Id: rebase.h 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 7.3  1996/02/18 01:45:35  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.2  1995/10/24 06:35:19  jims
 * Add guard against multiple inclusion
 *
 * Revision 7.1  1995/10/24  06:27:36  jims
 * Move to revision 7.1
 */

/////////////////////////////////////////////////////////////////////////////
//
// RWREBaseClass
//
//  Stores enums shared by both RWTRegularExpression
//  and RWTRegularExpresssionImp.
//
//  This is inherited by both RWTRegularExpression and RWTRegularExpressionImp
//  because RWTRegularExpressionImp uses some enums that are declared in
//  RWTRegularExpression but RWTRegularExpression has a member of type
//  RWTRegularExpressionImp. This can lead to a "chicken and egg" situation
//  that can confuse some compilers; hence this class.
//
/////////////////////////////////////////////////////////////////////////////

class RWREBaseClass
{
public:

  enum statusType {
    OK=0,
    NOT_SUPPORTED = 0x100,
    NO_MATCH, BAD_PATTERN, BAD_COLLATING_ELEMENT, BAD_CHAR_CLASS_TYPE,
    TRAILING_BACKSLASH, UNMATCHED_BRACKET, UNMATCHED_PARENTHESIS,
    UNMATCHED_BRACE, BAD_BRACE, BAD_CHAR_RANGE, OUT_OF_MEMORY, BAD_REPEAT
  };

  enum {MAX_NO_OF_SUBEXPRESSIONS = 1}; // Includes subexpr 0, the whole string
};

#endif  /* __RWREBASE_H__ */
