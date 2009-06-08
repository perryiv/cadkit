
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
#include "Usul/Strings/Format.h"

using namespace Usul;
using namespace Usul::Base;

USUL_IMPLEMENT_TYPE_ID ( Referenced );


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
  _refCount()
{
  _refCount.fetch_and_store ( 0 );

#if _DEBUG
  Detail::im.add ( this );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

Referenced::Referenced ( const Referenced &r ) : BaseClass ( r ),
  _refCount()
{
  _refCount.fetch_and_store ( 0 );

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
#ifdef _DEBUG
  // If this is the first time, update the entry in the instance-manager. 
  // When the entry is first made in the constructor, the virtual table 
  // is not fully constructed.
  if ( 0 == _refCount )
    Detail::im.update ( this );
#endif

  _refCount.fetch_and_increment();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unreference this instance.
//
///////////////////////////////////////////////////////////////////////////////

void Referenced::unref ( bool allowDeletion )
{
  // Hopefully there is only one thread in here when we delete.
  const unsigned int count ( --_refCount );
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
        throw std::runtime_error ( Usul::Strings::format (
          "Error 1078343241: deleting this instance caused an exception.",
          "\n\tMessage: ", e.what(),
          "\n\tAddress: ", this,
          "\n\tClass:   ", name ) );
      }

      catch ( ... )
      {
        USUL_ASSERT ( 0 == 1078340966u ); // FYI
        throw std::runtime_error ( Usul::Strings::format (
          "Error 1078340800: deleting this instance caused an exception.",
          "\n\tAddress: ", this,
          "\n\tClass:   ", name ) );
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
  return _refCount;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Classes that inherit can overload and return an IUnknown.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* Referenced::asUnknown()
{
	return 0x0;
}
