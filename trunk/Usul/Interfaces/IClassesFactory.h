
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

#ifndef _USUL_INTERFACE_CLASSES_FACTORY_H_
#define _USUL_INTERFACE_CLASSES_FACTORY_H_

#include "Usul/Interfaces/IUnknown.h"

#include <list>

namespace Usul {
namespace Interfaces {


struct IClassesFactory : public IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IClassesFactory );

  /// Id for this interface.
  enum { IID = 3653177759 };

  typedef std::list < IUnknown * > Unknowns;

  /// Create a list components of the given interface type (defined by iid).
  /// Return a list of unknown pointers to the new components.
  virtual Unknowns createInstances ( unsigned long iid ) = 0;
};


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_CLASSES_FACTORY_H_
