
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for transforming vertices.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_COORDINATE_TRANSFORM_H__
#define __USUL_INTERFACES_COORDINATE_TRANSFORM_H__

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Math/Vector3.h"

#include "osg/Array"

namespace osg { class Vec3f; }

namespace Usul {
namespace Interfaces {


template
<
  unsigned long iid,
  class VertexType,
  class VertexArrayType
>
struct ICoordinateTransform : public Usul::Interfaces::IUnknown
{
  typedef VertexType Vertex;
  typedef VertexArrayType VertexArray;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ICoordinateTransform );

  /// Id for this interface.
  enum { IID = iid };

  // Transform coordinates.
  virtual void                                    transformCoordinate  ( Vertex& ) const = 0;
  virtual void                                    transformCoordinates ( VertexArray& ) const = 0;

}; // ICoordinateTransform

typedef ICoordinateTransform<1238102520u,osg::Vec3,osg::Vec3Array>  ICoordinateTransformOSG;
typedef ICoordinateTransform<3318846281u,Usul::Math::Vec3d,std::vector<Usul::Math::Vec3d> >  ICoordinateTransformUsul;

}
}


#endif // __USUL_INTERFACES_COORDINATE_TRANSFORM_H__
