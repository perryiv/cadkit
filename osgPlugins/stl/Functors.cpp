///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Functors.h"

#include "osg/Geode"
#include "osg/Geometry"
#include "osg/Vec3"

///////////////////////////////////////////////////////////////////////////////
//
//  Add the number of facets in this geometry to the running total
//
///////////////////////////////////////////////////////////////////////////////
void FacetCounter::countFacets( osg::Geode *geode )
{
  unsigned int numDrawables ( geode->getNumDrawables() );
  for ( unsigned int i = 0; i < numDrawables; ++i )
  {
    // Get the drawable.
    const osg::Drawable *drawable = geode->getDrawable ( i );

    // See if the drawable is a geometry.
    const osg::Geometry *geometry = drawable->asGeometry();
    if(geometry)
    {
      const osg::Vec3Array *normals = geometry->getNormalArray();

      _numFacets += normals->size();
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Write out a facet in Ascii
//
///////////////////////////////////////////////////////////////////////////////
void AsciiWriter::operator () ( const osg::Vec3& normal, const osg::Vec3& v1, const osg::Vec3& v2, const osg::Vec3& v3)
{
  _out << "facet normal " << normal[0] << " " << normal[1] << " " << normal[2] << " " << std::endl;
  _out << "  outer loop " << std::endl;
  _out << "    vertex "   << v1[0] << " " << v1[1] << " " << v1[2] << " " << std::endl;
  _out << "    vertex "   << v2[0] << " " << v2[1] << " " << v2[2] << " " << std::endl;
  _out << "    vertex "   << v3[0] << " " << v3[1] << " " << v3[2] << " " << std::endl;
  _out << "  endloop"     << std::endl;
  _out << "endfacet"      << std::endl;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Write out a facet in binary
//
///////////////////////////////////////////////////////////////////////////////
void BinaryWriter::operator () ( const osg::Vec3& normal, const osg::Vec3& v1, const osg::Vec3& v2, const osg::Vec3& v3)
{
  char buf[50]; //50 bytes to define a facet
  ::memset( buf, 0, 50 );

  float f1, f2, f3;

  //write normal
  f1 = normal[0];
  f2 = normal[1];
  f3 = normal[2];
  ::memcpy( buf    , &f1, 4);
  ::memcpy( buf + 4, &f2, 4);
  ::memcpy( buf + 8, &f3, 4);

  //write first vertex
  f1 = v1[0];
  f2 = v1[1];
  f3 = v1[2];
  ::memcpy( buf + 12, &f1, 4);
  ::memcpy( buf + 16, &f2, 4);
  ::memcpy( buf + 20, &f3, 4);

  //write second vertex
  f1 = v2[0];
  f2 = v2[1];
  f3 = v2[2];
  ::memcpy( buf + 24, &f1, 4);
  ::memcpy( buf + 28, &f2, 4);
  ::memcpy( buf + 32, &f3, 4);

  //write third vertex
  f1 = v3[0];
  f2 = v3[1];
  f3 = v3[2];
  ::memcpy( buf + 36, &f1, 4);
  ::memcpy( buf + 40, &f2, 4);
  ::memcpy( buf + 44, &f3, 4);

  //write spacers
  ::memcpy( buf + 48, "\0", 1);
  ::memcpy( buf + 49, "\0", 1);

  _out.write( buf, 50 );
}