#ifndef __RWTIMER_H__
#define __RWTIMER_H__

/*
 * RWTimer: measures elapsed CPU or user time.
 *
 * $Id: timer.h 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 1.1.1.1  2000/03/02 21:40:43  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 7.5  1996/07/18 04:13:33  hart
 * ObjectStore changes
 *
 * Revision 7.4  1996/05/07 21:19:26  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.3  1996/02/18 01:46:59  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.2  1995/09/05 20:07:06  jims
 * Use new copyright macro
 *
 * Revision 7.1  1994/10/16  02:58:04  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.2  1994/07/12  19:58:19  vriezen
 * Update Copyright notice
 *
 * Revision 6.1  1994/04/15  19:11:11  vriezen
 * Move all files to 6.1
 *
 * Revision 2.3  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 2.1  1992/10/31  02:20:23  keffer
 * Clarified header comments.
 *
 * Revision 2.0  1992/10/23  03:27:32  keffer
 * RCS Baseline version
 *
 *    Rev 1.0   11 Mar 1992 14:10:46   KEFFER
 * Initial revision.
 */

#ifndef __RWDEFS_H__
#  include "rw/defs.h"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWTimer {

  double        startTime_;
  double        stopTime_;
  RWBoolean     isStopped_;

  static double absoluteTime();

public:
  RWTimer();

  double        elapsedTime() const;
  void          reset();
  void          start();
  void          stop();
#ifdef RW_OBJECTSTORE_HETERO
  char          _os_pad_0[4];
#endif
 
   RW_TYPESPEC  /* This macro usually expands to nothing */
};

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif  /* __RWTIMER_H__ */
