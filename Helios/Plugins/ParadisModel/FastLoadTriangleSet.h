
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __FAST_LOAD_TRIANGLE_SET_H__
#define __FAST_LOAD_TRIANGLE_SET_H__

#include "Group.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Interfaces/IUnknown.h"

#include "osg/ref_ptr"
#include "osg/Vec3f"
#include "osg/BoundingBox"
#include "osg/Array"
#include "osg/PrimitiveSet"
#include "osg/Geode"

#include <vector>
#include <map>
#include <string>

namespace Usul 
{ 
  namespace Interfaces 
  { 
    struct IDecimateTriangles; 
    struct ISmoothTriangles;
    struct ISubdivideTriangles;
  } 
}

namespace osg { class Node; }

/// I think that we need a Triangle Set interface...

class FastLoadTriangleSet : public Usul::Base::Referenced
{
public:
  // Useful typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef osg::ref_ptr < osg::Vec3Array > VerticesPtr;
  typedef osg::ref_ptr < osg::Vec3Array > NormalsPtr;
  typedef std::map < std::string,std::string > Options;
  typedef Usul::Interfaces::IUnknown Unknown;
  
  // Type information.
  USUL_IMPLEMENT_INLINE_TYPE_ID ( FastLoadTriangleSet );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( FastLoadTriangleSet );

  /// Constructor.
  FastLoadTriangleSet();

  /// Add a group
  void                    addGroup ( osg::Vec3Array *vertices, 
                                     osg::Vec3Array *normalsT, 
                                     osg::Vec3Array *normalsV, 
                                     osg::DrawElementsUInt *indices );

  unsigned int            numberOfGroups () const;
  Unknown*                getPrimitiveGroup ( unsigned int i );
 
  /// Get the bounding box.
  osg::BoundingBox        boundingBox() const;

  /// Build the scene
  osg::Node*              buildScene ( const Options &opt, Unknown *caller );

  /// Decimate
  void                    decimate ( Usul::Interfaces::IDecimateTriangles* decimate, float reduction );

  /// Is the triangle set empty?
  bool                    empty() const;

  /// Update the bounding box.
  void                    updateBounds ( const osg::Vec3& vertex );

  /// Smooth
  void                    smooth ( Usul::Interfaces::ISmoothTriangles *smooth, unsigned int numIterations );

  /// Subdivide
  void                    subdivide ( Usul::Interfaces::ISubdivideTriangles *subdivide, unsigned int numIterations );

protected:

  /// Use reference counting.
  virtual ~FastLoadTriangleSet();

private:

  typedef std::vector< Group::RefPtr > Groups;

  osg::BoundingBox _bb;
  Groups _groups;
};

#endif // __FAST_LOAD_TRIANGLE_SET_H__
