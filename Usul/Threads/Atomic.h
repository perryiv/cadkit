
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_THREADS_ATOMIC_H__
#define __USUL_THREADS_ATOMIC_H__

#include "Usul/Config/Config.h"

#ifdef USUL_INTEL_TBB_ATOMICS
# include "Usul/Threads/AtomicTBB.h"
#else
# include "Usul/Threads/AtomicMutex.h"
#endif

#endif // __USUL_THREADS_ATOMIC_H__
