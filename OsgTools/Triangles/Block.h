
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach & Perry Miller
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_TRIANGLES_PARITION_SINGLE_BLOCK_H__
#define __OSG_TOOLS_TRIANGLES_PARITION_SINGLE_BLOCK_H__

#include "OsgTools/Export.h"
#include "OsgTools/Triangles/Triangle.h"

#include "Usul/Types/Types.h"
#include "Usul/Base/Referenced.h"

#include "osg/BoundingBox"
#include "osg/Geometry"
#include "osg/ref_ptr"

#include <vector>
#include <map>
#include <string>

using namespace Usul::Types;

namespace OsgTools {
namespace Triangles {

class TriangleSet;

class OSG_TOOLS_EXPORT Block : public Usul::Base::Referenced
{
public:

  // Useful typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef std::map < std::string, std::string > Options;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Block );

  // Construction
  Block ( const osg::BoundingBox &box, unsigned int numTrianglesToReserve );

  // Add a triangle.
  void                      addTriangle ( TriangleSet *, Triangle * );

  // Return the bounding box.
  const osg::BoundingBox &  boundingBox() const { return _bbox; }

  // Build the scene.
  osg::Geometry *           buildScene ( const Options &options, TriangleSet *ts );

  // Get/set display list state.
  bool                      displayList() const;
  void                      displayList ( bool );

  // Purge any excess memory.
  void                      purge();

  // Remove a triangle.
  void                      removeTriangle ( unsigned int );

  // Return the i'th triangle in the block.
  const Triangle *          triangle ( unsigned int i ) const { return _triangles.at ( i ); }
  Triangle *                triangle ( unsigned int i )       { return _triangles.at ( i ); }
  
  // Get the normal of the i'th triangle.
  const osg::Vec3f &        triangleNormal ( unsigned int ) const;
  
  /// Get the Number of Triangles in this block
  Uint32                    getNumberOfTriangles();
  
protected:

  // Do not copy.
  Block ( const Block & );
  Block &operator = ( const Block & );

  // Use reference counting.
  virtual ~Block();

  void                      _reserveTriangles ( unsigned int numTriangles );

private:

  typedef osg::DrawElementsUInt Elements;
  typedef osg::ref_ptr < Elements > ElementsPtr;
  typedef osg::ref_ptr < osg::Geometry > GeometryPtr;
  typedef Triangle::ValidAccessRefPtr TrianglePtr;
  typedef std::vector < TrianglePtr > Triangles;
  typedef osg::ref_ptr < osg::Vec3Array > Normals;
  typedef osg::ref_ptr < osg::Vec4Array > Colors;

  osg::BoundingBox _bbox;
  ElementsPtr _elements;
  GeometryPtr _geometry;
  Triangles _triangles;
  Normals _normalsT;
  Colors _colorsT;
};


} // namespace Triangles
} // namespace OsgTools


#endif // __OSG_TOOLS_TRIANGLES_PARITION_SINGLE_BLOCK_H__
