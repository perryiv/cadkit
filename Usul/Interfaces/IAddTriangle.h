
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

namespace OsgTools { namespace Triangles { class SharedVertex; } }
namespace osg { class Vec3f; }


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

  // Add a single triangle.
  virtual void addTriangle ( VertexType v0, VertexType v1, VertexType v2, NormalType n, bool update ) = 0;
};

typedef IAddTriangle < OsgTools::Triangles::SharedVertex *, const osg::Vec3f &, 2793935616u > IAddTriangleWithSharedVertex;
typedef IAddTriangle < const osg::Vec3f &,                  const osg::Vec3f &, 2793935617u > IAddTrangleWithOsgVec3f;

}
}

#endif // __USUL_INTERFACES_ADD_TRIANGLE_H__
