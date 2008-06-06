
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for working with a sphere.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SAL_INTERFACE_SPHERE_H_
#define _SAL_INTERFACE_SPHERE_H_

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Math/Vector3.h"


namespace SAL {
namespace Interfaces {


template
<
  unsigned long iid,
  class RealType
>
struct ISphere : public Usul::Interfaces::IUnknown
{
  typedef Usul::Math::Vector3<RealType> Vec3;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ISphere );

  /// Id for this interface.
  enum { IID = iid };

  // Get/set the center.
  virtual void            getSphereCenter ( Vec3 &center ) const = 0;
  virtual void            setSphereCenter ( const Vec3 &center ) = 0;

  // Get/set the radius.
  virtual void            getSphereRadius ( RealType &radius ) const = 0;
  virtual void            setSphereRadius ( RealType radius ) = 0;
};


typedef ISphere<1734189293u,float>  ISphereFloat;
typedef ISphere<2747391966u,double> ISphereDouble;


}; // namespace Interfaces
}; // namespace SAL


#endif // _SAL_INTERFACE_SPHERE_H_
