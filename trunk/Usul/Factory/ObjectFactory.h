
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Serialization factory class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FACTORY_OBJECT_FACTORY_CLASS_
#define _USUL_FACTORY_OBJECT_FACTORY_CLASS_

#include "Usul/Export/Export.h"
#include "Usul/Factory/BaseCreator.h"
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"

#include <map>


namespace Usul {
namespace Factory {


///////////////////////////////////////////////////////////////////////////////
//
//  Factory class.
//
///////////////////////////////////////////////////////////////////////////////

class USUL_EXPORT ObjectFactory 
{
public:

  typedef std::map < std::string, BaseCreator::ValidRefPtr > Creators;
  typedef Usul::Threads::RecursiveMutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;

  // Use as singleton or not.
  ObjectFactory();
  ~ObjectFactory();

  void                              add ( BaseCreator *c );

  void                              clear();

  Usul::Base::Referenced *          create ( const std::string &name );

  static ObjectFactory &            instance();
  static void                       instance ( ObjectFactory *f );

  Mutex &                           mutex() const;

  void                              remove ( const std::string &name );

private:

  void                              _destroy();

  static ObjectFactory *_instance;
  mutable Mutex *_mutex;
  Creators _creators;
};


} // namespace Factory
} // namespace Usul


#endif // _USUL_FACTORY_OBJECT_FACTORY_CLASS_
