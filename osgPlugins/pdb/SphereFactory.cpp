
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
//  Create a sphere.
//
///////////////////////////////////////////////////////////////////////////////

SphereFactory::~SphereFactory()
{
  // Contained vectors are raw pointers. Some of them are null.
  for ( Spheres::iterator i = _spheres.begin(); i != _spheres.end(); ++i )
  {
    Vertices *v = *i;
    if ( v )
      v->clear();
  }
}


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


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

osg::Geometry *SphereFactory::create ( unsigned int numDivisions, const osg::Vec3 &center, float radius )
{
  // Make sure we have room. Pass null for default value.
  if ( numDivisions >= _spheres.size() )
    _spheres.resize ( numDivisions + 1, 0x0 );

  // If we have this one already...
  if ( _spheres[numDivisions] )
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

  // Otherwise, make the new sphere vertices.
  else
  {
    // Generate the vertices (which are also normals).
    std::auto_ptr<Vertices> verts ( new Vertices );
    Usul::Algorithms::sphere<float> ( numDivisions, *verts );

    // Put these vertices into our list.
    _spheres.at(numDivisions) = verts.release();

    // Recursively call this function to make the geometry.
    return this->create ( numDivisions, center, radius );
  }
}
