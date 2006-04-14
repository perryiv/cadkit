
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DirectX Joystick factory class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _DIRECTX_JOYSTICK_FACTORY_CLASS_H_
#define _DIRECTX_JOYSTICK_FACTORY_CLASS_H_

#include "CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IClassFactory.h"


class DXJoystickFactory : public Usul::Base::Referenced,
                          public Usul::Interfaces::IClassFactory
{
public:

  /// Typedefs.
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Usul::Base::Referenced BaseClass;

  /// Type information.
  USUL_DECLARE_TYPE_ID ( DXJoystickFactory );

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( DXJoystickFactory );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  DXJoystickFactory();

protected:

  /// Use reference counting.
  virtual ~DXJoystickFactory();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::IClassFactory
  //
  /////////////////////////////////////////////////////////////////////////////

  /// Create a component of the given interface type (defined by iid).
  /// Return an unknown pointer to this new component.
  virtual Unknown *       createInstance ( unsigned long iid );
};


#endif // _DIRECTX_JOYSTICK_FACTORY_CLASS_H_
