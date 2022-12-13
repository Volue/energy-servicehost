#ifndef __RWMESSAGE_H__
#define __RWMESSAGE_H__

/*
 * Message string facility
 *
 * $Id: message.h 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 7.4  1996/05/07 22:49:52  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.3  1996/02/18 01:45:19  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.2  1995/09/05 19:10:54  jims
 * Use new copyright macro
 *
 * Revision 7.1  1994/10/16  02:52:11  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.3  1994/07/12  19:58:19  vriezen
 * Update Copyright notice
 *
 * Revision 6.2  1994/05/16  18:12:19  jims
 * Port to Win32 DLL
 *
 * Revision 6.1  1994/04/15  19:10:30  vriezen
 * Move all files to 6.1
 *
 * Revision 1.3  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 1.2  1993/05/31  21:45:26  keffer
 * New messaging architecture for localization
 *
 * Revision 1.1  1993/05/18  00:41:12  keffer
 * Initial revision
 */

#include "rw/defs.h"

struct RWCatMsg
{
  const char* domainName_;      // Called "Catalog Name" by catgets()
  int         msgNumber_;       // Message number
  const char* msg_;             // Default string (catgets())
                                //   or lookup key (gettext())
};

typedef const RWCatMsg* RWMsgId;

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWMessage
{
public:
  RWMessage(const char*);
  RWMessage(RWMsgId ...);
  RWMessage(const RWMessage&);
  ~RWMessage();
  RWMessage& operator=(const RWMessage&);
  const char* str() const {return msg_;}
private:
  char* msg_;
};

#ifndef RW_TRAILING_RWEXPORT
rwexport const char* rwMsgLookup(RWMsgId msgId);
#else
const char* rwexport rwMsgLookup(RWMsgId msgId);
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWMESSAGE_H__ */
