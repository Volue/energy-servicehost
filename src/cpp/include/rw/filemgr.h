#ifndef __RWFILEMGR_H__
#define __RWFILEMGR_H__

/*
 * RWFileManager --- manages free space in a file
 *
 * $Id: filemgr.h 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 1.1.1.1  2000/03/02 21:40:41  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 7.4  1996/05/07 23:24:31  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.3  1996/02/18 01:41:47  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.2  1995/09/05 18:31:56  jims
 * Use new copyright macro
 *
 * Revision 7.1  1994/10/16  02:49:02  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.3  1994/07/12  18:46:56  vriezen
 * Update Copyright notice
 *
 * Revision 6.2  1994/05/16  18:10:10  jims
 * Add optional mode paramter to constructor to allow use of
 * read-only files
 *
 * Revision 6.1  1994/04/15  19:09:57  vriezen
 * Move all files to 6.1
 *
 * Revision 2.6  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 2.5  1993/07/19  20:45:26  keffer
 * friend classes now use elaborated-type-specifier (ARM Sec. 11.4)
 *
 * Revision 2.4  1993/05/18  00:41:46  keffer
 * Now uses new exception handling facility
 *
 * Revision 2.3  1993/04/09  23:09:56  keffer
 * Extensive rewrite.  Allocations now done in units of RWspace.
 *
 * Revision 2.2  1993/03/01  23:28:38  keffer
 * Renamed variables.
 *
 * Revision 2.0  1992/10/23  03:27:32  keffer
 * RCS Baseline version
 *
 *    Rev 1.2   18 Feb 1992 09:54:20   KEFFER
 * 
 *    Rev 1.1   28 Oct 1991 09:08:14   keffer
 * Changed inclusions to <rw/xxx.h>
 * 
 *    Rev 1.0   28 Jul 1991 08:15:00   keffer
 * Tools.h++ V4.0.5 PVCS baseline version
 *
 */

#include "rw/rwfile.h"
#include "rw/tooldefs.h"
#include "rw/rwerr.h"

class RWListManager;

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWFileManager : public RWFile
{

public:

  RWFileManager(const char* fname, const char* mode = rwnil);

  ~RWFileManager();

  RWoffset              allocate(RWspace);      // Allocate storage 
  void                  deallocate(RWoffset);   // Deallocate storage.
  RWoffset              endData() const  {return endOfData_;} 
  RWoffset              start()   const  {return startOfData_;}

#ifdef RDEBUG
  RWoffset              walkFreeList(RWoffset&, int&, RWspace&);
  void                  summarize();
#endif

protected:

  RWoffset              allocateAtEnd(RWspace);
  RWBoolean             deallocateFromEnd(RWoffset, RWspace);
  void                  readErr();
  void                  seekErr();
  void                  writeErr();
  RWoffset              rootOffset() const;

private:

  // Cannot have 2 managers for the same file.
  RWFileManager(const RWFileManager&);
  void                  operator=(const RWFileManager&);

private:

  RWListManager*        filemgr_;       // Implementation
  RWoffset              startOfData_;   // Offset to first data in file.
  RWoffset              endOfData_;     // Offset to last data in file.

friend class RWListManager;
};

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWFILEMGR_H__ */
