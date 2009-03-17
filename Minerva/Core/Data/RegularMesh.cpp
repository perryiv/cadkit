
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Represents an N x M mesh.
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Data/RegularMesh.h"

#include "OsgTools/State/StateSet.h"
#include "OsgTools/Convert.h"

#include "Usul/Algorithms/TriStrip.h"
#include "Usul/Interfaces/IElevationDatabase.h"
#include "Usul/Interfaces/IPlanetCoordinates.h"
#include "Usul/Strings/Format.h"

#include "osg/Geometry"
#include "osg/Geode"
#include "osg/Material"
#include "osg/MatrixTransform"

#include <stdexcept>

using namespace Minerva::Core::Data;

USUL_IMPLEMENT_TYPE_ID ( RegularMesh );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RegularMesh::RegularMesh() : BaseClass(),
  _v(),
  _n(),
  _c(),
  _m(),
  _size ( 0, 0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

RegularMesh::~RegularMesh()
{
  _v.clear();
  _n.clear();
  _c.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the size.
//
///////////////////////////////////////////////////////////////////////////////

void RegularMesh::size ( unsigned int numRows, unsigned int numColumns )
{
  Guard guard ( this );
  _size[0] = numRows;
  _size[1] = numColumns;
  this->dirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the vertices.
//
///////////////////////////////////////////////////////////////////////////////

void RegularMesh::vertices ( const Vertices &v )
{
  Guard guard ( this );
  _v = v;
  this->dirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the normals.
//
///////////////////////////////////////////////////////////////////////////////

void RegularMesh::normals ( const Normals &n )
{
  Guard guard ( this );
  _n = n;
  this->dirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the colors.
//
///////////////////////////////////////////////////////////////////////////////

void RegularMesh::colors ( const Colors &c )
{
  Guard guard ( this );
  _c = c;
  this->dirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the material.
//
///////////////////////////////////////////////////////////////////////////////

void RegularMesh::material ( const Color &ambient, const Color &diffuse, const Color &specular, const Color &emissive, double shininess )
{
  Guard guard ( this );
  _m.first[0] = ambient;
  _m.first[1] = diffuse;
  _m.first[2] = specular;
  _m.first[3] = emissive;
  _m.second   = shininess;
  this->dirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene branch for the data object.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* RegularMesh::_buildScene ( Usul::Interfaces::IUnknown *caller )
{
  Guard guard ( this );

  // If we do not have enough vertices then punt.
  if ( _v.size() < 4 )
  {
    throw std::runtime_error ( "Error 1954173844: Need at least 4 vertices in a regular mesh" );
  }

  // Need the same number of normals as vertices.
  if ( _n.size() != _v.size() )
  {
    throw std::runtime_error ( Usul::Strings::format
      ( "Error 2446056880: Number of vertices = ", _v.size(), " and number of normals = ", _n.size(), '\n' ) );
  }

  // Either need the same number of colors as vertices, just one, or none.
  if ( ( _c.size() != _v.size() ) && ( 1 != _c.size() ) && ( 0 != _c.size() ) )
  {
    throw std::runtime_error ( Usul::Strings::format
      ( "Error 2446056880: Number of vertices = ", _v.size(), " and number of colors = ", _c.size(), '\n' ) );
  }

  // Make sure given size will work.
  if ( _v.size() != ( _size[0] * _size[1] ) )
  {
    throw std::runtime_error ( Usul::Strings::format
      ( "Error 2491480094: Number of vertices is ", _v.size(), " and given mesh size is (", _size[0], ',', _size[1], ')', '\n' ) );
  }

  // Query for needed interfaces.
  Usul::Interfaces::IElevationDatabase::QueryPtr elevation ( caller );
  Usul::Interfaces::IPlanetCoordinates::QueryPtr planet ( caller );

  // Store the converted values here.
  Vertices v;
  osg::ref_ptr<osg::Vec3Array> normals ( new osg::Vec3Array );
  v.reserve ( _v.size() );
  normals->reserve ( _n.size() );

  // Make new extents.
  Extents e;

  if ( true == planet.valid() )
  {
    for ( unsigned int i = 0; i < _v.size(); ++i )
    {
      // Get the original vertex in lon-lat-elev space.
      Vertices::value_type vlle ( _v[i] );

      // Expand the extents.
      e.expand ( Extents::Vertex ( vlle[0], vlle[1] ) );

      // Get the height.
      vlle[2] = this->_elevation ( vlle, elevation.get() );

      // Convert the vertex to xyz.
      Vertices::value_type vxyz;
      planet->convertToPlanet ( vlle, vxyz );
      v.push_back ( vxyz );

      // Make a point representing the tip of the normal vector.
      Normals::value_type nlle ( vlle + _n[i] );

      // Convert this point to xyz.
      Vertices::value_type nxyz;
      planet->convertToPlanet ( nlle, nxyz );

      // The new normal vector is the difference.
      nxyz = nxyz - vxyz;
      nxyz.normalize();
      normals->push_back ( osg::Vec3Array::value_type ( nxyz[0], nxyz[1], nxyz[2] ) );
    }
  }
  else
  {
    v = _v;
    for ( unsigned int i = 0; i < _n.size(); ++i )
    {
      const Normals::value_type &nxyz ( _n[i] );
      normals->push_back ( osg::Vec3Array::value_type ( nxyz[0], nxyz[1], nxyz[2] ) );
    }
  }
  
  // Set our new extents.
  this->extents ( e );
  
  // Make the vertices.
  osg::ref_ptr < osg::Vec3Array > vertices ( new osg::Vec3Array );
  vertices->reserve ( v.size() );
  
  // Move all the vertices so that the first point starts at (0,0,0).
  Vertices::value_type offset ( v.at ( 0 ) );
  {
    for ( unsigned int i = 0; i < v.size(); ++i )
    {
      const Vertices::value_type xyz ( v[i] - offset );
      vertices->push_back ( osg::Vec3Array::value_type ( xyz[0], xyz[1], xyz[2] ) );
    }
  }

  // Make the geomtry and set vertices.
  osg::ref_ptr < osg::Geometry > geometry ( new osg::Geometry );
  geometry->setVertexArray ( vertices.get() );

  // Make the geode.
  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );
  geode->addDrawable ( geometry.get() );

  // Make the matrix-transform to hold the geode.
  osg::ref_ptr<osg::MatrixTransform> mt ( new osg::MatrixTransform );
  mt->setMatrix ( osg::Matrixd::translate ( offset[0], offset[1], offset[2] ) );
  mt->addChild ( geode.get() );

  // Set normals.
  geometry->setNormalArray ( normals.get() );
  geometry->setNormalBinding ( osg::Geometry::BIND_PER_VERTEX );

  // Make the colors if we have them.
  if ( false == _c.empty() )
  {
    osg::ref_ptr < osg::Vec4Array > colors ( new osg::Vec4Array );
    colors->reserve ( _c.size() );
    {
      typedef Colors::value_type FromColor;
      typedef osg::Vec4Array::value_type ToColor;
      typedef Usul::Convert::Type<FromColor,ToColor> ColorConverter;
      for ( unsigned int i = 0; i < _c.size(); ++i )
      {
        const ToColor color ( ColorConverter::convert ( _c[i] ) );
        colors->push_back ( color );
      }
    }
    geometry->setColorArray ( colors.get() );
    geometry->setColorBinding ( ( colors->size() == vertices->size() ) ? osg::Geometry::BIND_PER_VERTEX : osg::Geometry::BIND_OVERALL );

    // Do our best to make sure lighting is off.
    OsgTools::State::StateSet::setLighting ( geometry->getOrCreateStateSet(), false );
    OsgTools::State::StateSet::setLighting ( geode->getOrCreateStateSet(), false );
    OsgTools::State::StateSet::setLighting ( mt->getOrCreateStateSet(), false );
  }

  // Otherwise, make a material.
  else
  {
    osg::ref_ptr<osg::Material> material ( new osg::Material );
    material->setAmbient   ( osg::Material::FRONT_AND_BACK, osg::Vec4 ( _m.first[0][0], _m.first[0][1], _m.first[0][2], _m.first[0][3] ) );
    material->setDiffuse   ( osg::Material::FRONT_AND_BACK, osg::Vec4 ( _m.first[1][0], _m.first[1][1], _m.first[1][2], _m.first[1][3] ) );
    material->setSpecular  ( osg::Material::FRONT_AND_BACK, osg::Vec4 ( _m.first[2][0], _m.first[2][1], _m.first[2][2], _m.first[2][3] ) );
    material->setEmission  ( osg::Material::FRONT_AND_BACK, osg::Vec4 ( _m.first[3][0], _m.first[3][1], _m.first[3][2], _m.first[3][3] ) );
    material->setShininess ( osg::Material::FRONT_AND_BACK, static_cast < float > ( _m.second ) );
    OsgTools::State::StateSet::setMaterial ( mt.get(), material.get() );
  }

  // If we have more than one row then use indices.
  if ( _size[0] > 2 )
  {
    typedef unsigned short IndexType;
    typedef std::vector<IndexType> Indices;
    typedef std::vector<Indices> Primitives;

    Primitives primitives;
    Usul::Algorithms::triStripIndices ( _size[0], _size[1], primitives );

    // Loop through the primitives.
    for ( unsigned int i = 0; i < primitives.size(); ++i )
    {
      const Indices &strip ( primitives.at ( i ) );
      geometry->addPrimitiveSet ( new osg::DrawElementsUShort ( osg::PrimitiveSet::TRIANGLE_STRIP, strip.size(), &strip[0] ) );
    }
  }

  // Otherwise, just add a single tri-strip.
  else
  {
    geometry->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::TRIANGLE_STRIP, 0, vertices->size() ) );
  }

  // Set the render bin.
  mt->getOrCreateStateSet()->setRenderBinDetails ( this->renderBin(), "RenderBin" );

  // Return the matrix-transform.
  return mt.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is this geometry transparent?
//
///////////////////////////////////////////////////////////////////////////////

bool RegularMesh::isSemiTransparent() const
{
  Guard guard ( this );
  if ( 0 == _c.size() )
  {
    return ( _m.first[1][3] < 1 );
  }
  else
  {
    return ( _c.front()[3] < 1 );
  }
}
