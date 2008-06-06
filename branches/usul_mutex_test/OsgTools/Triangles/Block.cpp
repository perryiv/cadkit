
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

#include "OsgTools/MaterialFactory.h"

#include "Usul/Exceptions/Thrower.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Shared/Preferences.h"

#include <iostream>


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
  _normalsT  ( new osg::Vec3Array ),
  _colorsT   ( new osg::Vec4Array )
{
  // Make sure the bounding-box is valid.
  if ( false == _bbox.valid() )
    throw std::runtime_error ( "Error 2019573022: invalid bounding box given to constructor" );

  // Reserve triangles.
  this->_reserveTriangles ( reserve );

  // The geometry gets one primitive-set.
  _geometry->addPrimitiveSet ( _elements.get() );
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
#if 0
  USUL_ASSERT ( _colorsT->size()  == _triangles.size() );
#endif
  USUL_ASSERT ( _elements->size() == _triangles.size() * 3 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of triangles in this block
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Block::getNumberOfTriangles() 
{
    return _triangles.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the normal of the i'th triangle.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec3f & Block::triangleNormal ( unsigned int i ) const
{
    return _normalsT->at(i);
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
#if 0
  USUL_ASSERT ( _colorsT->size()  == _triangles.size() );
#endif
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

#if 0
  // Append the per-triangle color.
  _colorsT->push_back ( ts->color ( t ) );
#endif

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
#if 0
  USUL_ASSERT ( _colorsT->size()  == _triangles.size() );
#endif
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

#if 0
  // Remove the per-triangle color.
  _colorsT->erase ( _colorsT->begin() + index );
#endif

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

#if 0
  _colorsT->reserve  ( numTriangles );
#endif
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

  // Purge per-triangle colors.
  {
    Colors temp ( new osg::Vec4Array ( *_colorsT ) );
    _colorsT->swap ( *temp );
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
#if 0
  USUL_ASSERT ( _colorsT->size()  == _triangles.size() );
#endif
  USUL_ASSERT ( _elements->size() == _triangles.size() * 3 );
  USUL_ASSERT ( 1 == _geometry->getNumPrimitiveSets() );
  USUL_ASSERT ( _elements.get() == _geometry->getPrimitiveSet ( 0 ) );

  // If we don't have any triangles, return a null geometry.
  // This is because with Visual Studio 8, an empty vector (i.e. PrimitiveSetUInt )
  // has null pointers for it's member variables.  So when OSG trys to draw with an
  // empty primitive set, an exception is thrown.  The geode ignores null geometry,
  // so this is safe wrt the geode's internal list of geometries.
  if( _triangles.empty() )
    return 0x0;

  // Check input.
  if ( 0x0 == ts )
    throw std::runtime_error ( "Error 3206734392: null triangle-set given" );

  // Set display-list flag (just in case it has been reset somehow).
  const bool useList ( Usul::Shared::Preferences::instance().getBool ( "display_lists" ) );
  _geometry->setUseDisplayList ( useList );

  // Set the vertices.
  osg::ref_ptr<osg::Vec3Array> vertices ( ts->vertices() );
  _geometry->setVertexArray ( vertices.get() );

  // Set the correct normal-vectors.
  if ( OsgTools::Options::has ( options, "normals", "per-vertex" ) )
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

  // Set the correct colors.
  if ( OsgTools::Options::has ( options, "colors", "per-vertex" ) )
  {
    osg::ref_ptr< osg::Vec4Array > colors ( ts->colorsV() );
    if( true == colors.valid() && vertices->size() == colors->size() )
    {
      _geometry->setColorArray ( colors.get() );
      _geometry->setColorBinding ( osg::Geometry::BIND_PER_VERTEX );
    }
  }
  else if ( OsgTools::Options::has ( options, "colors", "per-triangle" ) )
  {

    if( true == _colorsT.valid () && _colorsT->size () == _triangles.size() )
    {
      _geometry->setColorArray ( _colorsT.get() );
      _geometry->setColorBinding ( osg::Geometry::BIND_PER_PRIMITIVE );
    }
  }
  else
  {
    // Use Materials
  }

#if 0
  static osg::ref_ptr < OsgTools::MaterialFactory > mf ( new OsgTools::MaterialFactory );

  osg::ref_ptr < osg::StateSet > ss ( _geometry->getOrCreateStateSet() );
  ss->setAttribute ( mf->create(), osg::StateAttribute::ON );
#endif

  // Return the geometry.
  return _geometry.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the indices of the triangles in this Block.
//
///////////////////////////////////////////////////////////////////////////////

void Block::triangleIndices ( TriangleIndices& indices ) const
{
  indices.reserve ( _elements->size() );
  for ( Elements::const_iterator iter = _elements->begin(); iter != _elements->end(); ++iter )
  {
    indices.push_back ( *iter );
  }
}
