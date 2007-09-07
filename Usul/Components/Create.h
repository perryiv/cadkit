
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Template classes and functions that help construct objects.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_COMPONENT_CREATE_HELPER_CLASSES_H_
#define _USUL_COMPONENT_CREATE_HELPER_CLASSES_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Components {


///////////////////////////////////////////////////////////////////////////////
//
//  Helper struct to create a new component and then have it query for 
//  the requested interface.
//
///////////////////////////////////////////////////////////////////////////////

template < class ComponentType > class Create
{
  static Usul::Interfaces::IUnknown *_create ( unsigned long iid, ComponentType *c )
  {
    // Require the pointer to be valid.
    typedef typename ComponentType::ValidRefPtr ValidComponent;
    ValidComponent component ( c );

    // Do not require this to work.
    typedef typename Usul::Interfaces::IUnknown::RefPtr UnknownPtr;
    UnknownPtr unknown ( component->queryInterface ( iid ) );

    // Release this now so that we control the order of unreferencing.
    component.release();

    // Return the released pointer to the component (which may be null).
    return unknown.release();
  }

public:

  template < class Arg > static Usul::Interfaces::IUnknown *create ( unsigned long iid, Arg arg )
  {
    return _create ( iid, new ComponentType ( arg ) );
  }

  static Usul::Interfaces::IUnknown *create ( unsigned long iid )
  {
    return _create ( iid, new ComponentType() );
  }
};


} // namespace Components
} // namespace Usul


#endif // _USUL_COMPONENT_CREATE_HELPER_CLASSES_H_
