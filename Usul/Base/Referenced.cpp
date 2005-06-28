
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Reference-counting base class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Base/Referenced.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Exceptions/Thrower.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Threads/Set.h"

#include <set>
#include <string>
#include <stdexcept>
#include <iostream>

using namespace Usul;
using namespace Usul::Base;

USUL_IMPLEMENT_TYPE_ID ( Referenced );

///////////////////////////////////////////////////////////////////////////////
//
//  I do not want these in the header file.
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul
{
  namespace Base
  {
    typedef Usul::Threads::Mutex Mutex;
    typedef Usul::Threads::Guard<Mutex> Guard;

    #ifdef _DEBUG

    struct InstanceManager
    {
      typedef std::set<Referenced*> Set;
  
      InstanceManager() : _set()
      {
      }

      ~InstanceManager()
      {
        if ( false == _set.empty() )
        {
          USUL_ASSERT ( 0 ); // FYI
          std::cout << _set.size() << " referenced items remain:" << std::endl;
          for ( Set::const_iterator i = _set.begin(); i != _set.end(); ++i )
          {
            Referenced *r ( *i );
            std::cout << "\taddress = " << r << ", name = " << ( ( r ) ? r->typeId().name() : "NULL" ) << std::endl;
          }
        }
      }

      Set &set()
      {
        return _set;
      }

    private:

      Set _set;
    };

    InstanceManager im;

    #endif
  };
};


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Referenced::Referenced() : Typed(),
  _refCount ( 0 ),
  _rcMutex ( Mutex::create() )
{
#if _DEBUG
  // Make sure this address is not already in our set, and insert it.
  std::pair<InstanceManager::Set::iterator,bool> result = im.set().insert ( this );
  USUL_ASSERT ( true == result.second );
  USUL_ASSERT ( this == *(result.first) );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

Referenced::Referenced ( const Referenced &r ) : Typed ( r ),
  _refCount ( 0 ),
  _rcMutex ( Mutex::create() )
{
#if _DEBUG
  // Make sure this address is not already in our set, and insert it.
  std::pair<InstanceManager::Set::iterator,bool> result = im.set().insert ( this );
  USUL_ASSERT ( true == result.second );
  USUL_ASSERT ( this == *(result.first) );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Referenced::~Referenced()
{
#if _DEBUG
  // Remove this address from the set. Should be one occurance.
  USUL_ASSERT ( 1 == im.set().erase ( this ) );
#endif

  // Should be true.
  USUL_ASSERT ( 0 == _refCount );
  USUL_ASSERT ( _rcMutex );

  delete _rcMutex;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Assignment.
//
///////////////////////////////////////////////////////////////////////////////

Referenced &Referenced::operator = ( const Referenced &r )
{
  // We define this function to make sure the compiler does not. A naive 
  // assignment operator would assign the reference count and mutex, which 
  // does not make any sense.
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reference this instance.
//
///////////////////////////////////////////////////////////////////////////////

void Referenced::ref()
{
  Guard guard ( *_rcMutex );
  ++_refCount;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Decrement the reference count.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  inline unsigned long threadSafeDecrement ( unsigned long &num, Usul::Threads::Mutex &mutex )
  {
    Guard guard ( mutex );
    USUL_ASSERT ( num >= 1 );
    return ( --num );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Unreference this instance.
//
///////////////////////////////////////////////////////////////////////////////

void Referenced::unref ( bool allowDeletion )
{
  // We decrement in a separate function because we cannot delete with the 
  // mutex locked. Hopefully there is only one thread in here when we delete.
  unsigned long count ( Detail::threadSafeDecrement ( _refCount, *_rcMutex ) );
  if ( 0 == count && allowDeletion )
  {
    #ifdef _DEBUG

      const std::string name ( this->typeId().name() );

      try
      {
        delete this;
      }

      catch ( std::exception &e )
      {
        USUL_ASSERT ( 0 == 1078343250u ); // FYI
        Usul::Exceptions::Thrower<std::runtime_error>
          ( "Error 1078343241: deleting this instance caused an exception.",
            "\n\tMessage: ", e.what(),
            "\n\tAddress: ", this,
            "\n\tClass:   ", name );
      }

      catch ( ... )
      {
        USUL_ASSERT ( 0 == 1078340966u ); // FYI
        Usul::Exceptions::Thrower<std::runtime_error>
          ( "Error 1078340800: deleting this instance caused an exception.",
            "\n\tAddress: ",this,
            "\n\tClass:   ", name );
      }

    #else

      delete this;

    #endif
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the reference count.
//
///////////////////////////////////////////////////////////////////////////////

unsigned long Referenced::refCount() const
{
  Guard guard ( *_rcMutex );
  return _refCount;
}
