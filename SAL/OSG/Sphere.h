
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Sphere class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_SAL_SPHERE_CLASS_H_
#define _OSG_SAL_SPHERE_CLASS_H_

#include "Usul/Base/Referenced.h"

#include "SAL/Interfaces/ISphere.h"
#include "SAL/Interfaces/IColorRGBA.h"
#include "SAL/Interfaces/IBuilder.h"
#include "SAL/Interfaces/INode.h"


namespace SAL {
namespace OSG {


class Sphere : public Usul::Base::Referenced,
               public SAL::Interfaces::ISphereFloat,
               public SAL::Interfaces::IBuilder
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Sphere );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction.
  Sphere();

protected:

  typedef SAL::Interfaces::ISphereFloat::Vec3 Vec3;
  typedef SAL::Interfaces::INode INode;

  /// Do not copy.
  Sphere ( const Sphere & );
  Sphere &operator = ( const Sphere & );

  /// Use reference counting.
  virtual ~Sphere();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  SAL::Interfaces::ISphereFloat
  //
  /////////////////////////////////////////////////////////////////////////////

  // Get/set the center.
  virtual void            getSphereCenter ( Vec3 &center ) const;
  virtual void            setSphereCenter ( const Vec3 &center );

  // Get/set the radius.
  virtual void            getSphereRadius ( float &radius ) const;
  virtual void            setSphereRadius ( float radius );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  SAL::Interfaces::IBuilder
  //
  /////////////////////////////////////////////////////////////////////////////

  // Build the scene.
  virtual INode *         buildScene() const;

private:

  Vec3  _center;
  float _radius;
};


}; // namespace OSG
}; // namespace SAL


#endif // _OSG_SAL_SPHERE_CLASS_H_
