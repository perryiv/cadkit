
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach & Perry Miller
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Triangles/Block.h"
#include "OsgTools/Triangles/TriangleSet.h"
#include "OsgTools/HasOption.h"

#include "Usul/Exceptions/Thrower.h"
#include "Usul/Errors/Assert.h"

#define USE_RANDOM_COLORS
#ifdef USE_RANDOM_COLORS
#include "Usul/Adaptors/Random.h"
#include "osg/Material"
#endif

using namespace OsgTools::Triangles;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Block::Block ( const osg::BoundingBox &box, unsigned int reserve ) : BaseClass(),
  _bbox      ( box ),
  _elements  ( new osg::DrawElementsUInt ( osg::PrimitiveSet::TRIANGLES ) ),
  _geometry  ( new osg::Geometry ),
  _triangles (),
  _normalsT  ( new osg::Vec3Array )
{
  // Make sure the bounding-box is valid.
  if ( false == _bbox.valid() )
    throw std::runtime_error ( "Error 2019573022: invalid bounding box given to constructor" );

  // Reserve triangles.
  this->_reserveTriangles ( reserve );

  // The geometry gets one primitive-set.
  _geometry->addPrimitiveSet ( _elements.get() );

#ifdef USE_RANDOM_COLORS

  // Give the geometry a random material.
  Usul::Adaptors::Random < float > random ( 0.2f, 0.8f );
  osg::ref_ptr < osg::Material > mat ( new osg::Material );
  mat->setDiffuse ( osg::Material::FRONT, osg::Vec4f ( random(), random(), random(), 1.0f ) );
  mat->setAmbient ( osg::Material::FRONT, mat->getDiffuse ( osg::Material::FRONT ) );
  _geometry->getOrCreateStateSet()->setAttribute ( mat.get(), osg::StateAttribute::ON );

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Block::~Block()
{
  // Should be true.
  USUL_ASSERT ( _normalsT->size() == _triangles.size() );
  USUL_ASSERT ( _elements->size() == _triangles.size() * 3 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a triangle.
//
///////////////////////////////////////////////////////////////////////////////

void Block::addTriangle ( TriangleSet *ts, Triangle *t )
{
  // Should be true.
  USUL_ASSERT ( _normalsT->size() == _triangles.size() );
  USUL_ASSERT ( _elements->size() == _triangles.size() * 3 );

  // Check input.
  if ( 0x0 == ts )
    throw std::runtime_error ( "Error 3791231357: null triangle-set given" );
  if ( 0x0 == t )
    throw std::runtime_error ( "Error 2575308850: null triangle given" );

  // Get the indices.
  const unsigned int i0 ( t->vertex0()->index() );
  const unsigned int i1 ( t->vertex1()->index() );
  const unsigned int i2 ( t->vertex2()->index() );

  // Append the appropriate vertices to the draw-elements primitive.
  _elements->push_back ( i0 );
  _elements->push_back ( i1 );
  _elements->push_back ( i2 );

  // Append the per-triangle normal vector.
  _normalsT->push_back ( ts->triangleNormal ( t->index() ) );

  // Append the triangle to our list.
  _triangles.push_back ( t );

  // Flag the geometry as dirty.
  _geometry->dirtyBound();
  _geometry->dirtyDisplayList();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the triangle.
//
///////////////////////////////////////////////////////////////////////////////

void Block::removeTriangle ( unsigned int index )
{
  // Should be true.
  USUL_ASSERT ( _normalsT->size() == _triangles.size() );
  USUL_ASSERT ( _elements->size() == _triangles.size() * 3 );

  // Make sure the index is in range.
  if ( index > _triangles.size() )
  {
    Usul::Exceptions::Thrower<std::range_error> 
      ( "Error 3838651841: given index ", index, " is out of range; ",
        "there are ", _triangles.size(), " triangles in this block" );
  }

  // Remove the indices from the draw-elements.
  osg::DrawElementsUInt::iterator first ( _elements->begin() + ( index * 3 ) );
  osg::DrawElementsUInt::iterator last  ( first + 3 );
  _elements->erase ( first, last );

  // Remove the per-triangle normal vector.
  _normalsT->erase ( _normalsT->begin() + index );

  // Remove the triangle.
  _triangles.erase ( _triangles.begin() + index );

  // Flag the geometry as dirty.
  _geometry->dirtyBound();
  _geometry->dirtyDisplayList();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reserve space for this many triangles.
//
///////////////////////////////////////////////////////////////////////////////

void Block::_reserveTriangles ( unsigned int numTriangles )
{
  _elements->reserve ( numTriangles * 3 );
  _triangles.reserve ( numTriangles );
  _normalsT->reserve ( numTriangles );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the display list flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Block::displayList() const
{
  return _geometry->getUseDisplayList();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the display list flag.
//
///////////////////////////////////////////////////////////////////////////////

void Block::displayList ( bool state )
{
  _geometry->setUseDisplayList ( state );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Purge any excess memory.
//
///////////////////////////////////////////////////////////////////////////////

void Block::purge()
{
  // Purge draw-elements.
  {
    osg::ref_ptr < osg::DrawElementsUInt > temp ( new osg::DrawElementsUInt ( *_elements, osg::CopyOp::DEEP_COPY_ALL ) );
    _elements->swap ( *temp );
  }

  // Purge triangles.
  {
    Triangles temp ( _triangles );
    _triangles.swap ( temp );
  }

  // Purge per-triangle normal vectors.
  {
    Normals temp ( new osg::Vec3Array ( *_normalsT ) );
    _normalsT->swap ( *temp );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the node.
//
///////////////////////////////////////////////////////////////////////////////

osg::Geometry *Block::buildScene ( const Options &options, TriangleSet *ts )
{
  // Should be true.
  USUL_ASSERT ( _normalsT->size() == _triangles.size() );
  USUL_ASSERT ( _elements->size() == _triangles.size() * 3 );
  USUL_ASSERT ( 1 == _geometry->getNumPrimitiveSets() );
  USUL_ASSERT ( _elements.get() == _geometry->getPrimitiveSet ( 0 ) );

  // Check input.
  if ( 0x0 == ts )
    throw std::runtime_error ( "Error 3206734392: null triangle-set given" );

  // Set the vertices.
  osg::ref_ptr<osg::Vec3Array> vertices ( ts->vertices() );
  _geometry->setVertexArray ( vertices.get() );

  // Set the correct normal-vectors.
  if ( OsgTools::Options::has ( options, "normals", "average" ) )
  {
    osg::ref_ptr<osg::Vec3Array> normals ( ts->normalsV() );
    USUL_ASSERT ( vertices->size() == normals->size() );
    _geometry->setNormalArray ( normals.get() );
    _geometry->setNormalBinding ( osg::Geometry::BIND_PER_VERTEX );
  }
  else
  {
    _geometry->setNormalArray ( _normalsT.get() );
    _geometry->setNormalBinding ( osg::Geometry::BIND_PER_PRIMITIVE );
  }

  // Return the geometry.
  return _geometry.get();
}
