#ifndef __RWMODEL_H__
#define __RWMODEL_H__

/*
 * RWModel --- maintains a list of dependent clients
 *
 * $Id: model.h 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 7.5  1996/07/18 04:20:22  hart
 * ObjectStore changes
 *
 * Revision 7.4  1996/05/07 23:26:00  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.3  1996/02/18 01:45:21  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.2  1995/09/05 19:11:18  jims
 * Use new copyright macro
 *
 * Revision 7.1  1994/10/16  02:53:22  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.4  1994/07/18  19:04:25  jims
 * Add copyright
 *
 * Revision 6.3  1994/06/16  04:20:24  vriezen
 * Added outlined virtual dtors.
 *
 * Revision 6.2  1994/04/15  21:12:29  vriezen
 * Backed out (until next release) RW_DEFINE_HOME and RW_DECLARE_HOME support.
 *
 * Revision 6.1  1994/04/15  19:10:31  vriezen
 * Move all files to 6.1
 *
 * Revision 2.4  1994/03/31  23:16:08  vriezen
 * Add RW_DECLARE_HOME and RW_DEFINE_HOME for class RWModelClient
 *
 * Revision 2.3  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 2.1  1992/11/16  04:24:16  keffer
 * Broke out RWModel functionality into RWModel and RWModelClient
 *
 * Revision 2.0  1992/10/23  03:27:32  keffer
 * RCS Baseline version
 *
 * 
 *    Rev 1.1   18 Feb 1992 09:55:34   KEFFER
 * 
 *    Rev 1.0   12 Nov 1991 13:13:18   keffer
 * Initial revision.
 */

#include "rw/ordcltn.h"

class RWExport RWModelClient;

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWModel RWBehavior 
{
public:
  virtual ~RWModel();
  RWModel();
  void                  addDependent(RWModelClient*);
  void                  removeDependent(RWModelClient*);
  // Allows peek at dependent list:
  const RWOrdered*      dependents() {return &dependList;}
  virtual void          changed(void* pData = 0);      // Notify all dependents
private:
  RWOrdered             dependList;
  
  RW_TYPESPEC  /* This macro usually expands to nothing */
};


class RWExport RWModelClient : public RWCollectable
{
public:
  virtual ~RWModelClient();
  virtual void          updateFrom(RWModel* model, void* pData) = 0;

  RW_TYPESPEC  /* This macro usually expands to nothing */
};

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif  /* __RWMODEL_H__ */
