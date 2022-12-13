#ifndef __RWBENCH_H__
#define __RWBENCH_H__

/*
 * RWBench:  A class to help in the running of benchmarks
 *
 * $Id: bench.h 18623 2000-03-03 08:28:13Z sih $
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
 * To use, derive a class from RWBench including a doLoop(unsigned long) 
 * function, and a what() function (if you plan to use report()).  
 * The doLoop(unsigned long N) function should perform N operations of
 * the type you are trying to benchmark.  RWBench will call this function
 * over and over again until "duration" time has elapsed.  Then it will sum
 * the total number of operations performed.
 *
 * To run, construct an object then call go().  Then call report() to get
 * a summary.  You can call ops() outerLoops(), etc. for more detail.
 *
 * If you wish to correct for overhead, then provide an idleLoop() function
 * which does non-benchmark related calculations.
 *
 ***************************************************************************
 */

#include "rw/timer.h"
#include "rw/rstream.h"

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWBench RWBehavior {

public:

  // Default: 5 second test, one inner loop op, no machine name.
  RWBench(double duration = 5, unsigned long ILO=1000, const char* machine = 0);

  // Deletes memory allocated for machine_
  ~RWBench();            
   
  void           parse(const char *args);   // A 'parse' for Windows users - calls parse(int, char*[])
  virtual void   parse(int argc, char* argv[]);
  virtual void   go();                      // Run the benchmark
  virtual void   doLoop(unsigned long n)=0;       // User supplied: should execute the inner loop n times
  virtual void   idleLoop(unsigned long n);       // To calculate looping overhead
  virtual void   what(RW_SL_IO_STD(ostream)&) const;      // Print out what we're doing --- used by report()
  void           where(RW_SL_IO_STD(ostream)&) const;     // Print out the machine type and the compiler
  virtual void   report(RW_SL_IO_STD(ostream)&) const;    // Print out #reps and speed
  double         setDuration(double);       // Set test duration to something else
  unsigned long  setInnerLoops(unsigned long);    // Set # inner loops to something else
  double         duration() const           {return timeToTest_;}
  const char *   machine() const            {return machine_;   }
  unsigned long  innerLoops() const         {return innerLoops_;}
  double         time() const;              // time to execute in seconds
  unsigned long  outerLoops() const;        // Number of times the inner loop was executed
  double         ops() const;               // Number of operations performed
  double         opsRate() const;           // Number of operations per second
  double         kiloOpsRate() const;       // Number of thousands of ops per second
  double         megaOpsRate() const;       // Number of millions of ops per second

private:

  char*          machine_;       // What machine we're running on
  double         timeToTest_;    // How long should the test take
  unsigned long  innerLoops_;    // Number of inner loop operations to be done
  unsigned long  outerLoops_;    // Number of outer loops actually executed
  double         delta_;         // Actual time (corrected for overhead)
  
  RW_TYPESPEC  /* This macro usually expands to nothing */
};

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif  /* __RWBENCH_H__ */
