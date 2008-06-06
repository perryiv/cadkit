
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV and Denis V. Dorozkain
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Factory class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _COLLISION_PQP_FACTORY_CLASS_H_
#define _COLLISION_PQP_FACTORY_CLASS_H_

#include "Collision/PQP/CompileGuard.h"

#include "Usul/Base/Referenced.h"

#include "Usul/Interfaces/IClassFactory.h"


namespace Collision {
namespace PQP {


class Factory : public Usul::Base::Referenced,
                public Usul::Interfaces::IClassFactory
{
public:

  /// Typedefs.
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Usul::Base::Referenced BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( Factory );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  Factory();

protected:

  /// Use reference counting.
  virtual ~Factory();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::IClassFactory
  //
  /////////////////////////////////////////////////////////////////////////////

  /// Create a component of the given interface type (defined by iid).
  /// Return an unknown pointer to this new component.
  virtual Unknown *       createInstance ( unsigned long iid );
};


}; // namespace PQP
}; // namespace Collision


#endif // _COLLISION_PQP_FACTORY_CLASS_H_
