
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
//  Base factory class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FACTORY_BASE_FACTORY_CLASS_
#define _USUL_FACTORY_BASE_FACTORY_CLASS_

#include "Usul/Factory/BaseCreator.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/Guard.h"

#include <map>


namespace Usul {
namespace Factory {


///////////////////////////////////////////////////////////////////////////////
//
//  Base factory class.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > class BaseFactory
{
public:

  typedef T BaseType;
  typedef BaseCreator<BaseType> BaseCreatorType;
  typedef typename BaseCreatorType::ValidRefPtr BaseCreatorPtr;
  typedef std::map < std::string, BaseCreatorPtr > Creators;
  typedef Usul::Threads::Mutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;

  //
  // Constructor
  //
  BaseFactory() :
    _mutex(),
    _creators()
  {
  }

  //
  // Destructor
  //
  ~BaseFactory()
  {
  }

  //
  // Add a creator.
  //
  void add ( BaseCreatorType *c )
  {
    if ( 0x0 != c )
    {
      Guard guard ( _mutex );
      _creators.insert ( typename Creators::value_type ( c->name(), BaseCreatorPtr ( c ) ) );
    }
  }

  //
  // Clear the map.
  //
  void clear()
  {
    Guard guard ( _mutex );
    _creators.clear();
  }

  //
  // Return a newly created object, or null.
  //
  BaseType *create ( const std::string &name )
  {
    Guard guard ( _mutex );

    typename Creators::iterator i = _creators.find ( name );
    if ( _creators.end() == i )
    {
      return 0x0;
    }

    BaseCreatorType &creator = *(i->second);
    return creator();
  }

  //
  // Remove a creator.
  //
  void remove ( const std::string &name )
  {
    Guard guard ( _mutex );
    _creators.erase ( name );
  }

private:

  mutable Mutex _mutex;
  Creators _creators;
};


} // namespace Factory
} // namespace Usul


#endif // _USUL_FACTORY_BASE_FACTORY_CLASS_
