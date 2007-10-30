
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

namespace OsgTools { namespace Triangles { class SharedVertex; class Triangle; } }
namespace osg { class Vec3f; }


namespace Usul {
namespace Interfaces {


template 
< 
  class VertexType, 
  class NormalType, 
  bool DefaultLook,
  unsigned long iid 
>
struct IAddTriangle : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IAddTriangle );

  /// Id for this interface.
  enum { IID = iid };

  // Add a single triangle. Pass false for "look" if you know the vertex is 
  // unique. This is a "hint", and can be ignored by the implementation.
  virtual OsgTools::Triangles::Triangle *   addTriangle ( VertexType v0, VertexType v1, VertexType v2, NormalType n, bool update, bool look ) = 0;
};

typedef IAddTriangle < OsgTools::Triangles::SharedVertex *, const osg::Vec3f &, false, 2793935616u > IAddTriangleWithSharedVertex;
typedef IAddTriangle < const osg::Vec3f &,                  const osg::Vec3f &, true,  2793935617u > IAddTrangleWithOsgVec3f;

}
}

#endif // __USUL_INTERFACES_ADD_TRIANGLE_H__
