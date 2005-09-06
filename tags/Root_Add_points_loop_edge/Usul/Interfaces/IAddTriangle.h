
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_ADD_TRIANGLE_H__
#define __USUL_INTERFACES_ADD_TRIANGLE_H__

#include "Usul/Interfaces/IUnknown.h"

namespace osg { class Vec3f; }

namespace OsgTools { namespace Triangles { class SharedVertex; } }

namespace Usul {
namespace Interfaces {

template 
< 
  class VertexType, 
  class NormalType, 
  unsigned long iid 
>
struct IAddTriangle : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IAddTriangle );

  /// Id for this interface.
  enum { IID = iid };

  virtual void addTriangle ( const VertexType& v0, 
                             const VertexType& v1, 
                             const VertexType& v2, 
                             const NormalType & normal, 
                             bool correctNormal = false,
                             bool rebuild = false ) = 0;
};

typedef IAddTriangle< OsgTools::Triangles::SharedVertex, osg::Vec3f, 2793935616u > IAddTriangleSharedVertex;
typedef IAddTriangle< osg::Vec3f,                        osg::Vec3f, 2793935617u > IAddTrangleOsgVec3f;

}
}

#endif // __USUL_INTERFACES_ADD_TRIANGLE_H__

