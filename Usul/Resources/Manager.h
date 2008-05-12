
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_RESOURCES_MANAGER_H__
#define __USUL_RESOURCES_MANAGER_H__

#include "Usul/Export/Export.h"

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/Guard.h"

#include <string>
#include <map>

namespace Usul {
namespace Resources {

 
class USUL_EXPORT Manager
{
public:
  typedef Usul::Threads::Mutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;
  typedef Usul::Interfaces::IUnknown IUnknown;
  
  /// Get the instance.
  static Manager& instance();

  /// Add an unknown.
  void            add ( const std::string &name, IUnknown *unknown );

  /// Clear.
  void            clear();

  /// Create.
  IUnknown *      find ( const std::string &name ) const;

  /// Remove an unknown.
  void            remove ( const std::string &name );

private:

  Manager();
  ~Manager();

  typedef std::map < std::string, IUnknown::RefPtr > Unknowns;

  static Manager *_instance;
  mutable Mutex *_mutex;
  Unknowns _unknowns;
};


}
}


#endif // __USUL_RESOURCES_MANAGER_H__
