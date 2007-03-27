
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
#include "Usul/Base/InstanceManager.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Exceptions/Thrower.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Components/Exceptions.h"

using namespace Usul;
using namespace Usul::Base;

USUL_IMPLEMENT_TYPE_ID ( Referenced );


///////////////////////////////////////////////////////////////////////////////
//
//  Local typedefs.
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul
{
  namespace Base
  {
    typedef Usul::Threads::Mutex Mutex;
    typedef Usul::Threads::Guard<Mutex> Guard;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Variables with file scope.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
namespace Detail { Usul::Base::InstanceManager im; }
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Referenced::Referenced() : BaseClass(),
  _refCount ( 0 ),
  _rcMutex ( Mutex::create() )
{
#if _DEBUG
  Detail::im.add ( this );
  //USUL_ASSERT ( 0x08E52F48 != reinterpret_cast < unsigned int > ( this ) );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

Referenced::Referenced ( const Referenced &r ) : BaseClass ( r ),
  _refCount ( 0 ),
  _rcMutex ( Mutex::create() )
{
#if _DEBUG
  Detail::im.add ( this );
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
  Detail::im.remove ( this );
#endif

  // Should be true.
  USUL_ASSERT ( 0 == _refCount );
  USUL_ASSERT ( 0x0 != _rcMutex );

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
  // One thread at a time.
  Guard guard ( *_rcMutex );

#ifdef _DEBUG
  // If this is the first time, update the entry in the instange-manager. 
  // When the entry is first made in the constructor, the virtual table 
  // is not fully constructed.
  if ( 0 == _refCount )
    Detail::im.update ( this );
#endif

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
