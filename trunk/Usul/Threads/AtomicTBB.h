
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_THREADS_ATOMIC_TBB_H__
#define __USUL_THREADS_ATOMIC_TBB_H__

#ifndef NOMINMAX
# define NOMINMAX
#endif

#include "tbb/atomic.h"


namespace Usul {
namespace Threads {


template<class T>
class Atomic : public tbb::atomic<T>
{
};

}
}


#endif // __USUL_THREADS_ATOMIC_TBB_H__
