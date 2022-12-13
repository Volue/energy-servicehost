#ifndef __RWMUTEX_H__
#define __RWMUTEX_H__

/*
 * Declarations for class RWMutex
 *
 * $Id: mutex.h 18623 2000-03-03 08:28:13Z sih $
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
 * This class is a portable implementation of a simple mutex lock
 * to be used for synchronizing multiple threads within a single process.
 * It is not suitable for use among threads of different processes.
 * 
 ***************************************************************************
 */

#ifndef __RWDEFS_H__
# include "rw/defs.h"
#endif

#ifdef RW_MULTI_THREAD          /* This class only relevant in MT situation */

#include "rw/coreerr.h"
#include "rw/rwerr.h"
#if defined(RW_SOLARIS_THREADS)
#  include <synch.h>
#  include <thread.h>
   typedef mutex_t RWMutexType;

#elif defined(RW_POSIX_D10_THREADS)
#  include <pthread.h>
   typedef pthread_mutex_t RWMutexType;

#elif defined(RW_DCE_THREADS)
#  if defined(RW_NO_DCE_PTHREAD_H)
#    include <pthread.h>
#  else
#    include <dce/pthread.h>
#  endif
   typedef pthread_mutex_t RWMutexType;

#elif defined(__WIN32__)
#  include <windows.h>
   typedef CRITICAL_SECTION RWMutexType;

#elif defined(__OS2__)
#  define INCL_BASE            //Turns on all Dos??? definitions
#  include <os2.h>
   typedef HMTX RWMutexType;
#elif defined( macintosh )
#  include "toolsrc/semaphor.h"
   typedef TSemaphore* RWMutexType;  

// defines for other OSs go here
#else
#  error Class RWMutex is not yet supported in this environment
#endif

class RWMutex {
  RWMutexType mutex;
  int initFlag;
public:
  enum StaticCtor {staticCtor};
  enum NoInitialization {noInitialization};
  RWMutex(); 
  RWMutex(StaticCtor);   // some statics need special handling
  RWMutex(NoInitialization);
  ~RWMutex();            // destroy the mutex
  void init();
  void acquire(); // block until mutex released, acquire and continue
  void release(); // release the mutex
};

class RWGuard {
  RWMutex& rwmutex;
public:
  RWGuard(RWMutex& m);
  ~RWGuard();
};

/*
 * =========================
 * RWMutex member functions
 * -------------------------
 */

/*
 * For those OSs that require a non-zero mutex, we must treat static 
 * mutexes specially: They might not be initialized when we need them.
 */

inline RWMutex::RWMutex(RWMutex::StaticCtor)
{
  // empty, because acquire() may already have been used.
}

inline RWMutex::RWMutex(RWMutex::NoInitialization) : initFlag(0) { }

inline RWMutex::~RWMutex() 
{ 
  if (initFlag != 0) {
    initFlag = 0;
# if defined(RW_SOLARIS_THREADS)
    mutex_destroy(&mutex); 
# elif defined(RW_POSIX_D10_THREADS) || defined(RW_DCE_THREADS)
    pthread_mutex_destroy(&mutex); 
# elif defined(__WIN32__)
    DeleteCriticalSection(&mutex);
# elif defined(__OS2__)
    APIRET rv;
    if(0 != (rv = DosCloseMutexSem(mutex)))
      RWTHROW(RWExternalErr(RWMessage(RWCORE_GENERIC,rv)));
# elif defined( macintosh )
	mutex->Dispose();
#else
#  error Class RWMutex is not yet supported in this environment
# endif
  }
}

void
inline RWMutex::init()  
{ 
# if defined(RW_SOLARIS_THREADS)
  mutex_init(&mutex, USYNC_THREAD, NULL); 
# elif defined(RW_POSIX_D10_THREADS) || (defined(RW_DCE_THREADS) && defined(__IBMCPP__) && defined(_AIX)) 
//AIX 4.3 doesn't have pthread_mutexattr_default with DCE threads.
    pthread_mutex_init(&mutex, 0);
# elif defined(RW_DCE_THREADS)
    pthread_mutex_init(&mutex, pthread_mutexattr_default);
# elif defined(__WIN32__)
  InitializeCriticalSection(&mutex);
# elif defined(__OS2__)
  APIRET rv;
  if(0 != (rv = DosCreateMutexSem(0,&mutex,DC_SEM_SHARED,FALSE)))// some problem
     RWTHROW(RWExternalErr(RWMessage(RWCORE_GENERIC, rv)));
# elif defined( macintosh )		// djk
  mutex = new TSemaphore();
#else
#  error Class RWMutex is not yet supported in this environment
# endif

  initFlag = 1;
}

inline RWMutex::RWMutex() 
{ 
  init();   // Initialize the mutex
}  

inline void RWMutex::acquire() 
{ 
  if(0 == initFlag) init();

# if defined(RW_SOLARIS_THREADS)
  mutex_lock(&mutex);    
# elif defined(RW_POSIX_D10_THREADS) || defined(RW_DCE_THREADS)
  pthread_mutex_lock(&mutex);    
# elif defined(__WIN32__)
  EnterCriticalSection(&mutex);
# elif defined(__OS2__)
  APIRET rv;
  if(0 != (rv = DosRequestMutexSem(mutex, SEM_INDEFINITE_WAIT)))
    RWTHROW(RWExternalErr(RWMessage(RWCORE_GENERIC,rv)));
# elif defined( macintosh )		// djk
  mutex->Grab();
#else
#  error Class RWMutex is not yet supported in this environment
# endif
}

inline void RWMutex::release()
{ 
# if defined(RW_SOLARIS_THREADS)
  mutex_unlock(&mutex);  
# elif defined(RW_POSIX_D10_THREADS) || defined(RW_DCE_THREADS)
  pthread_mutex_unlock(&mutex);  
# elif defined(__WIN32__)
  LeaveCriticalSection(&mutex);
# elif defined(__OS2__)
  APIRET rv;
  if(0 != (rv = DosReleaseMutexSem(mutex)))
    RWTHROW(RWExternalErr(RWMessage(RWCORE_GENERIC,rv)));
# elif defined( macintosh )		// djk
  mutex->Release();
#else
#  error Class RWMutex is not yet supported in this environment
# endif
}

/*
 * ================================
 * RWGuard inline member functions
 * --------------------------------
 */

inline RWGuard::RWGuard(RWMutex& m) : rwmutex(m) { rwmutex.acquire(); }
inline RWGuard::~RWGuard()                       { rwmutex.release(); }

#endif  /* RW_MULTI_THREAD */
#endif  /* __RWMUTEX_H__ */

