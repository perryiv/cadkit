
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///
///  Class factory interface.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_CLASS_FACTORY_H_
#define _USUL_INTERFACE_CLASS_FACTORY_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {


struct IClassFactory : public IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IClassFactory );

  /// Id for this interface.
  enum { IID = 1075271048 };

  /// Create a component of the given interface type (defined by iid).
  /// Return an unknown pointer to this new component.
  virtual IUnknown *      createInstance ( unsigned long iid ) = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_CLASS_FACTORY_H_
