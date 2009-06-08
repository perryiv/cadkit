
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for a bounding sphere.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SAL_INTERFACE_BOUNDING_SPHERE_H_
#define _SAL_INTERFACE_BOUNDING_SPHERE_H_

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Math/Vector3.h"


namespace SAL {
namespace Interfaces {


template
<
  unsigned long iid,
  class RealType
>
struct IBoundingSphere : public Usul::Interfaces::IUnknown
{
  typedef Usul::Math::Vector3<RealType> Vec3;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IBoundingSphere );

  /// Id for this interface.
  enum { IID = iid };

  // Get the center.
  virtual void            boundingSphereCenter ( Vec3 &center ) const = 0;

  // Get the radius.
  virtual void            boundingSphereRadius ( RealType &radius ) const = 0;
};


typedef IBoundingSphere<2222872204u,float>  IBoundingSphereFloat;
typedef IBoundingSphere<3462467182u,double> IBoundingSphereDouble;


}; // namespace Interfaces
}; // namespace SAL


#endif // _SAL_INTERFACE_BOUNDING_SPHERE_H_
