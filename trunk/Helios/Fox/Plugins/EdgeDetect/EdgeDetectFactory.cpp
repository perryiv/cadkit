
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  ConvolveFactory class.
//
///////////////////////////////////////////////////////////////////////////////

#include "EdgeDetectFactory.h"
#include "EdgeDetectComponent.h"

#include "Usul/Components/Create.h"


USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( EdgeDetectFactory, EdgeDetectFactory::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this library was compiled in debug mode.
//
///////////////////////////////////////////////////////////////////////////////

extern "C" bool usul_is_debug_build()
{
  #ifdef _DEBUG
    return true;
  #else
    return false;
  #endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return a class ConvolveFactory.
//
///////////////////////////////////////////////////////////////////////////////

extern "C" Usul::Interfaces::IClassFactory *usul_get_class_factory()
{
  Usul::Interfaces::IClassFactory::ValidRefPtr edgeDetectFactory ( new EdgeDetectFactory() );
  return edgeDetectFactory.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

EdgeDetectFactory::EdgeDetectFactory() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

EdgeDetectFactory::~EdgeDetectFactory()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *EdgeDetectFactory::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IClassFactory::IID:
    return static_cast<Usul::Interfaces::IClassFactory*>(this);
  case Usul::Interfaces::IUnknown::IID:
    return static_cast<Usul::Interfaces::IUnknown*>(this);
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a component of the given interface type (defined by iid).
//  Return an unknown pointer to this new component.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *EdgeDetectFactory::createInstance ( unsigned long iid )
{
  // Require this to construct.
  EdgeDetectComponent::ValidAccessRefPtr component ( new EdgeDetectComponent() );

  // Do not require this to work.
  Usul::Interfaces::IUnknown::RefPtr unknown ( component->queryInterface ( iid ) );

  // Decrement this now so that we control the order of unreferencing.
  // Reference count should go from 2 -> 1. If query failed above then 
  // it's deleted here.
  component = 0x0;

  // Return the released pointer to the component.
  // Reference count should go from 1 -> 0, but it should not delete.
  return unknown.release();
}
