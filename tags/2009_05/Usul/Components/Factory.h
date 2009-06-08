
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV and Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Generic Component Factory class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_COMPONENTS_FACTORY_CLASS_H_
#define _USUL_COMPONENTS_FACTORY_CLASS_H_

#include "Usul/Config/Config.h"
#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IClassFactory.h"

namespace Usul {
namespace Components {


template < class Component >
class Factory : public Usul::Base::Referenced,
                public Usul::Interfaces::IClassFactory
{
public:

  /// Typedefs.
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Usul::Base::Referenced BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( Factory );

  /// Default construction.
  Factory() : BaseClass ()
  {
  }

  /// Reference.
  void ref()
  {
    BaseClass::ref();
  }

  /// Dereference.
  void unref ( bool allowDeletion = true )
  {
    BaseClass::unref ( allowDeletion );
  }

  /// Query for an interface.
  Usul::Interfaces::IUnknown * queryInterface ( unsigned long iid );

protected:

  /// Use reference counting.
  virtual ~Factory()
  {
  }

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::IClassFactory
  //
  /////////////////////////////////////////////////////////////////////////////

  /// Create a component of the given interface type (defined by iid).
  /// Return an unknown pointer to this new component.
  virtual Unknown *       createInstance ( unsigned long iid );
};


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

template < class Component >
Usul::Interfaces::IUnknown *Factory < Component >::queryInterface ( unsigned long iid )
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

template < class Component >
Usul::Interfaces::IUnknown *Factory < Component >::createInstance ( unsigned long iid )
{
  // Require this to construct.
  typename Component::ValidAccessRefPtr component ( new Component() );

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

}
}


#ifdef _DEBUG 
#define USUL_DEBUG_MODE true
#else
#define USUL_DEBUG_MODE false
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for functions needed in Cadkit plugins.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_DECLARE_COMPONENT_FACTORY(component_name) \
extern "C" bool usul_is_debug_build() \
{ \
  return USUL_DEBUG_MODE;\
} \
extern "C" Usul::Interfaces::IClassFactory *usul_get_class_factory() \
{ \
  Usul::Interfaces::IClassFactory::ValidRefPtr factory ( new Usul::Components::Factory<component_name>() ); \
  return factory.release(); \
}


#endif // _USUL_COMPONENTS_FACTORY_CLASS_H_
