
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry Miller
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "SphereFactory.h"

#include "osg/Geometry"
#include "osg/Geode"

#include "osgUtil/TriStripVisitor"

#include "Usul/Algorithms/Sphere.h"
#include "Usul/Errors/AssertPolicy.h"
#include "Usul/Containers/Vector.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SphereFactory::SphereFactory() :
  _spheres()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SphereFactory::~SphereFactory()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a sphere.
//
///////////////////////////////////////////////////////////////////////////////

osg::Geometry *SphereFactory::create ( unsigned int numDivisions )
{
  // Make sure we have room.
  if ( numDivisions >= _spheres.size() )
    _spheres.resize ( numDivisions + 1 );

  // If we have this one already...
  if ( _spheres.at(numDivisions).valid() )
    return _spheres.at(numDivisions).get();

  // Generate the points (which are both vertices and normals).
  std::vector<osg::Vec3> points;
  Usul::Algorithms::sphere<float> ( numDivisions, points );

#ifdef _DEBUG
  //points.resize ( 24 );
#endif

  // Make new vertices and normals.
  unsigned int num ( points.size() );
  osg::ref_ptr<osg::Vec3Array> vertices ( new osg::Vec3Array ( num ) );
  osg::ref_ptr<osg::Vec3Array> normals  ( new osg::Vec3Array ( num ) );

  // Copy the vertices and normals.
  std::copy ( points.begin(), points.end(), vertices->begin() );
  std::copy ( points.begin(), points.end(), normals->begin() );

  // Make a new geometry.
  osg::ref_ptr<osg::Geometry> geometry ( new osg::Geometry );
  geometry->setVertexArray ( vertices.get() );
  geometry->setNormalArray ( normals.get() );
  geometry->setNormalBinding ( osg::Geometry::BIND_PER_VERTEX );
  geometry->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::TRIANGLES, 0, vertices->size() ) );

  // Tri-strip it.
  osgUtil::TriStripVisitor visitor;
  //visitor.stripify ( *geometry );

  // Save this geometry in our collection and return it.
  _spheres.at(numDivisions) = geometry;
  return geometry.get();
}


#if 0

Need a trip-stripper that works.
1. Bag of vertices -> bag of triangles
2. Find adjacent triangles, start new strip when exhausted search
3. When done, discard excess vertices (i.e., make the strip).

Need to cache a geometry for each radii so that you can properly size the normals. 
Otherwise you will have to turn on GL_NORMALIZE.

Once you have tri-strips, see if they are faster than the shape-drawables.

#endif


#if 0

Using many osg::ShapeDrawable nodes ends up being faster than using many 
osg::Geometry nodes because the osg::Geometry hangs on to its vertices and 
you run out of memory. The ShapeDrawable simply calculates the vertices as 
needed. ShapeDrawable also does a push/pop and a glTranslate().

Perhaps the best scene would be to reuse several psg::Geometry nodes of a
unit sphere, one for each lod. Then you get vertex arrays and also do not 
use a lot of memory. However, you do introduce matrix transformations. 
Perhaps with this approach display lists can be turned back on.

The osg::Geometry could be constructed with either spheres with poles or 
subdivided spheres.

#endif


#if 0

  {
    // Shortcut.
    Vertices &verts = *(_spheres[numDivisions]);

    // Make new vertices and normals.
    unsigned int numVertices ( verts.size() );
    osg::ref_ptr<osg::Vec3Array> vertices ( new osg::Vec3Array ( numVertices ) );
    osg::ref_ptr<osg::Vec3Array> normals  ( new osg::Vec3Array ( numVertices ) );
    osg::Vec3Array &v = *vertices;
    osg::Vec3Array &n = *normals;

    // Copy the vertices. Scale and offset by radius and center, respectively.
    for ( unsigned int i = 0; i < numVertices; ++i )
    {
      const osg::Vec3 &vertex = verts.at(i);
      v.at(i) = vertex * radius + center;
      n.at(i) = vertex;
    }

    // Make a new geometry.
    osg::ref_ptr<osg::Geometry> sphere ( new osg::Geometry );
    sphere->setVertexArray ( vertices.get() );
    sphere->setNormalArray ( normals.get()  );
    sphere->setNormalBinding ( osg::Geometry::BIND_PER_VERTEX );
    sphere->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::TRIANGLES, 0, vertices->size() ) );

    // Strip it.
    //osgUtil::TriStripVisitor visitor;
    //visitor.stripify ( *sphere );

    // TODO, make this an option. Display lists crash with really big files.
    sphere->setUseDisplayList ( false );

    // Return the new sphere.
    return sphere.release();
  }


#endif
