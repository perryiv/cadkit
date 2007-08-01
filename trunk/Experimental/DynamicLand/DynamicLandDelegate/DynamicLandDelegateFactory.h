
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Scene Factory class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _DYNAMIC_LAND_DELEGATE_DELEGATE_FACTORY_CLASS_H_
#define _DYNAMIC_LAND_DELEGATE_DELEGATE_FACTORY_CLASS_H_

#include "CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IClassFactory.h"


class DynamicLandDelegateFactory : public Usul::Base::Referenced,
                             public Usul::Interfaces::IClassFactory
{
public:

  /// Typedefs.
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Usul::Base::Referenced BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( DynamicLandDelegateFactory );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  DynamicLandDelegateFactory();

protected:

  /// Use reference counting.
  virtual ~DynamicLandDelegateFactory();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::IClassFactory
  //
  /////////////////////////////////////////////////////////////////////////////

  /// Create a component of the given interface type (defined by iid).
  /// Return an unknown pointer to this new component.
  virtual Unknown *       createInstance ( unsigned long iid );
};


#endif // _DYNAMIC_LAND_DELEGATE_DELEGATE_FACTORY_CLASS_H_
