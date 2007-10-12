
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV and Denis V. Dorozkain
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for working with a PQP object.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _COLLISION_INTERFACE_PQP_COLLISION_OBJECT_H_
#define _COLLISION_INTERFACE_PQP_COLLISION_OBJECT_H_

#include "Usul/Interfaces/IUnknown.h"

class PQP_Model;


namespace Collision {
namespace Interfaces {


struct IPQPCollisionObject : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IPQPCollisionObject );

  /// Id for this interface.
  enum { IID = 2125682792u };

  /// Get the PQP model.
  const PQP_Model *         getPQPModel() const = 0;
  PQP_Model *               getPQPModel() = 0;
};


}; // namespace Interfaces
}; // namespace Collision


#endif // _COLLISION_INTERFACE_PQP_COLLISION_OBJECT_H_
