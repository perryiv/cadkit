
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Internal class used for debugging.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_REFERENCED_INSTANCE_MANAGER_CLASS_H_
#define _USUL_REFERENCED_INSTANCE_MANAGER_CLASS_H_

#include "Usul/Threads/Mutex.h"

#include <map>
#include <string>


namespace Usul {
namespace Base {

class Referenced;

class InstanceManager
{
public:

  typedef unsigned long CountType;
  typedef std::pair < CountType, std::string > ObjectInfo;
  typedef std::map < const Referenced *, ObjectInfo > ObjectMap;
  typedef std::pair < ObjectMap::iterator, bool > Result;

  explicit InstanceManager();
  ~InstanceManager();

  void      add    ( const Referenced * );
  void      remove ( const Referenced * );
  void      update ( const Referenced * );

protected:

  Usul::Threads::Mutex&   _mutex();

  InstanceManager ( const InstanceManager &r );
  InstanceManager &operator = ( const InstanceManager &r );

private:

  Usul::Threads::Mutex *_imMutex;
  ObjectMap _objects;
  CountType _count;
};


} // namespace Base
} // namespace Usul


#endif // _USUL_REFERENCED_INSTANCE_MANAGER_CLASS_H_
