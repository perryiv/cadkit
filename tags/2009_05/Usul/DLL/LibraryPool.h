
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Authors: Adam Kubach.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_DLL_LIBRARY_POOL_H__
#define __USUL_DLL_LIBRARY_POOL_H__

#include "Usul/Export/Export.h"
#include "Usul/DLL/Library.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/Guard.h"

#include <set>
#include <iosfwd>

namespace Usul {
namespace DLL {

class USUL_EXPORT LibraryPool
{
public:
  /// Typedefs.
  typedef Usul::DLL::Library Library;
  typedef Library::ValidRefPtr LibPtr;
  typedef std::set < LibPtr > LibrarySet;
  typedef Usul::Threads::Mutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;
  
  /// Get the instance.
  static LibraryPool& instance();
  
  /// Add a library.
  void add ( Library * );
  
  /// Clear.
  void clear( std::ostream *out );
  
private:
  LibraryPool();
  ~LibraryPool();
  
  static LibraryPool *_instance;
  LibrarySet _pool;
  mutable Mutex* _mutex;
};
  
}
}


#endif // __USUL_DLL_LIBRARY_POOL_H__
