#ifndef __RWINSTMGR_H__
#define __RWINSTMGR_H__

/*
 * Declarations for RWInstanceManager --- manages multi-thread and per-process
 * instance data.
 *
 * $Id: instmgr.h 18623 2000-03-03 08:28:13Z sih $
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

#if (defined(__DLL__) && defined(__WIN16__)) || defined(RW_MULTI_THREAD)

#if defined(__DLL__) && defined(__WIN16__)
#  include "rw/rwtsd.h"

#elif defined(RW_POSIX_D10_THREADS)
#  include <pthread.h>
   typedef pthread_key_t RWTSDKEY;
   
#elif defined(RW_SOLARIS_THREADS)  // MT & Sun -- assuming Solaris 2.x/Cafe
#  include <thread.h>
   typedef thread_key_t RWTSDKEY;
   
#elif defined(RW_DCE_THREADS)
#  if defined(RW_NO_DCE_PTHREAD_H)
#    include <pthread.h>
#  else
#    include <dce/pthread.h>
#  endif
   typedef pthread_key_t RWTSDKEY;


#elif defined(__WIN32__)
#  include <windows.h>
   typedef DWORD RWTSDKEY;

#elif defined(__OS2__)
#  if defined(__IBMCPP__)
#    define RWTHREADID *_threadid
#  elif defined(__TURBOC__)
#    define RWTHREADID _threadid
#  endif
#  define INCL_BASE
#  include <os2.h>
#  define rwfar
#  define rwpascal
   typedef unsigned long RWTSDKEY;

   RWTSDKEY rwfar rwpascal 
   RWGetTaskSpecificKey();

   int rwfar rwpascal 
   RWSetTaskSpecificData(RWTSDKEY hKey, void rwfar*);

   void rwfar* rwfar rwpascal 
   RWGetTaskSpecificData(RWTSDKEY hKey);

   void rwfar* rwfar rwpascal 
   RWReleaseTaskSpecificData(RWTSDKEY hKey);
#elif defined( macintosh )	// djk
#  include <Threads.h>
#  define TID		ThreadID	// used for our __OS2__ disguise ;-)
#  define rwfar
#  define rwpascal
   typedef unsigned long RWTSDKEY;

   RWTSDKEY rwfar rwpascal 
   RWGetTaskSpecificKey();

   int rwfar rwpascal 
   RWSetTaskSpecificData(RWTSDKEY hKey, void rwfar*);

   void rwfar* rwfar rwpascal 
   RWGetTaskSpecificData(RWTSDKEY hKey);

   void rwfar* rwfar rwpascal 
   RWReleaseTaskSpecificData(RWTSDKEY hKey);

#else
#  error RW_MULTI_THREAD not supported in this environment
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWInstanceManager
{
public:
  void rwfar*           addValue();       // set instance specific data
  void rwfar*           currentValue();   // get instance specific data
  void                  freeValue();      // release key and discard data

  // Values to be provided and discarded by the specializing class:
  virtual void rwfar*   newValue() = 0;
  virtual void          deleteValue(void rwfar*) = 0;

  RWInstanceManager();
  virtual ~RWInstanceManager();

private:
  RWTSDKEY              tsd_key;
};

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#else   /* neither 16-bit Windodws DLL nor MultiThread */

//#error RWInstanceManager class not meant to be used outside DLL or MT situation
extern int rwDummyInstMgr;

#endif
#endif  /* __RWINSTMGR_H__ */
