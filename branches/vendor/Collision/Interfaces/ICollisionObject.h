
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV and Denis V. Dorozkain
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for working with an object that handles collision.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _COLLISION_INTERFACE_COLLISION_OBJECT_H_
#define _COLLISION_INTERFACE_COLLISION_OBJECT_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Collision {
namespace Interfaces {


struct ICollisionObject : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ICollisionObject );

  /// Id for this interface.
  enum { IID = 3395652342u };
};


}; // namespace Interfaces
}; // namespace Collision


#endif // _COLLISION_INTERFACE_COLLISION_OBJECT_H_
