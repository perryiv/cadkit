
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV and Denis V. Dorozkain
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for working with many collision objects.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _COLLISION_INTERFACE_COLLIDER_OBJECT_H_
#define _COLLISION_INTERFACE_COLLIDER_OBJECT_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Collision {
namespace Interfaces {

struct ICollisionObject;


struct ICollider : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ICollider );

  /// Id for this interface.
  enum { IID = 2908694449u };

  /// See if the objects collide.
  virtual bool    isColliding ( ICollisionObject *object1, ICollisionObject *object2 ) const = 0;
};


}; // namespace Interfaces
}; // namespace Collision


#endif // _COLLISION_INTERFACE_COLLIDER_OBJECT_H_
