
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Denis V. Dorozkain
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Library class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _COLLISION_PQP_LIBRARY_OBJECT_CLASS_H_
#define _COLLISION_PQP_LIBRARY_OBJECT_CLASS_H_

#include "Collision/PQP/CompileGuard.h"
#include "Collision/Interfaces/ICollider.h"
#include "Collision/Interfaces/ICollisionObject.h"

#include "Usul/Base/Referenced.h"


namespace Collision {
namespace PQP {


class Library : public Usul::Base::Referenced,
                public Collision::Interfaces::ICollider
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Library );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction.
  Library();

protected:

  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Collision::Interfaces::ICollisionObject ICollisionObject;

  /// Do not copy.
  Library ( const Library & );
  Library &operator = ( const Library & );

  /// Use reference counting.
  virtual ~Library();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Collision::Interfaces::ICollider
  //
  /////////////////////////////////////////////////////////////////////////////

  /// See if the objects collide.
  virtual bool    isColliding ( ICollisionObject *object1, ICollisionObject *object2 ) const;
};


}; // namespace PQP
}; // namespace Collision


#endif // _COLLISION_PQP_LIBRARY_OBJECT_CLASS_H_
