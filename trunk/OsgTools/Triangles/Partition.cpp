
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Triangles/Partition.h"
#include "OsgTools/Triangles/SharedVertex.h"

#include "OsgTools/GlassBoundingBox.h"
#include "OsgTools/MaterialFactory.h"

#include "osg/Group"
#include "osg/Geode"

using namespace OsgTools::Triangles;

#define DRAW_CUBES 0
#define DRAW_MATERIALS 0

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Partition::Partition () :
_cubes(),
_primitiveSets(),
_geometries(),
_triangles()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the primitive sets.
//
///////////////////////////////////////////////////////////////////////////////

void Partition::clear()
{
  _cubes.clear();
  _geometries.clear();
  _primitiveSets.clear();
  _triangles.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a triangle.
//
///////////////////////////////////////////////////////////////////////////////

void Partition::add ( Triangle *t, const osg::Vec3Array &vertices, const osg::Vec3& n )
{
  unsigned int i0 ( t->vertex0()->index() );
  unsigned int i1 ( t->vertex1()->index() );
  unsigned int i2 ( t->vertex2()->index() );

  const osg::Vec3& v0 ( vertices.at( i0 ) );
  const osg::Vec3& v1 ( vertices.at( i1 ) );
  const osg::Vec3& v2 ( vertices.at( i2 ) );

  unsigned int index ( this->_index( v0, v1, v2 ) );

  PrimitiveSetPtr primitiveSet ( _primitiveSets.at( index ) );

  // Add the indices to the primitive set
  primitiveSet->push_back ( i0 );
  primitiveSet->push_back ( i1 );
  primitiveSet->push_back ( i2 );

  // Add the triangle.
  _triangles.at( index ).push_back ( t );

  if ( _geometries.at ( index )->getNormalBinding() == osg::Geometry::BIND_PER_PRIMITIVE )
  {
    osg::ref_ptr < osg::Vec3Array > normals ( _geometries.at ( index )->getNormalArray() );

    if ( normals.valid() )
      normals->push_back ( n );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a triangle.
//
///////////////////////////////////////////////////////////////////////////////

void Partition::remove ( const osg::Drawable *d, unsigned int index )
{
  for( unsigned int i = 0; i < _geometries.size(); ++i )
  {
    if( _geometries.at( i ).get() == d )
    {
      PrimitiveSetPtr primitiveSet ( _primitiveSets.at( i ) );

      // Positions of the indices of doomed
      osg::DrawElementsUInt::iterator first ( primitiveSet->begin() + ( index * 3 ) );
      osg::DrawElementsUInt::iterator last  ( primitiveSet->begin() + ( index * 3 ) + 3 );

      //Remove the indices from the primitive set
      primitiveSet->erase( first, last );
      
      //Display lists need to be recalculated
      _geometries.at( i )->dirtyDisplayList();

      _triangles.at( i ).erase ( _triangles.at( i ).begin() + index );

    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reserve enough room.
//
///////////////////////////////////////////////////////////////////////////////

void Partition::reserve ( unsigned int s )
{
  //unsigned int size ( s / _primitiveSets.size() );

  //for( PrimitiveSets::iterator i = _primitiveSets.begin(); i != _primitiveSets.end(); ++ i )
  //  (*i)->reserve ( size );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Call back to properly set the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

struct BoundingBoxCallback : public osg::Drawable::ComputeBoundingBoxCallback
{
  BoundingBoxCallback ( const osg::BoundingBox &bb ) :
  _bb ( bb )
  {
  }

  virtual osg::BoundingBox computeBound(const osg::Drawable&) const
  {
    return _bb;
  }

private:
  osg::BoundingBox _bb;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Build the node.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Partition::operator () ( osg::Vec3Array *vertices, osg::Vec3Array *normals, bool average )
{
  // A single geode.
  osg::ref_ptr<osg::Geode> geode ( new osg::Geode );

#if DRAW_CUBES
  osg::ref_ptr< osg::Group > root ( new osg::Group );
  root->addChild ( geode.get() );
#endif

#if DRAW_MATERIALS
  osg::ref_ptr< OsgTools::MaterialFactory > mf ( new OsgTools::MaterialFactory );
#else
  // Give the geode the default material.
  osg::ref_ptr< osg::StateSet > ss ( geode->getOrCreateStateSet() );
  ss->setAttribute ( new osg::Material, osg::StateAttribute::ON );
#endif

  for ( unsigned int i = 0; i < _cubes.size(); ++i )
  {
    GeometryPtr geometry ( _geometries.at( i ) );

#if DRAW_CUBES
    OsgTools::GlassBoundingBox gbb ( _cubes.at( i ) );
    gbb( root.get(), true, false, false );
#endif

#if DRAW_MATERIALS
    osg::ref_ptr< osg::StateSet > ss ( geometry->getOrCreateStateSet() );
    ss->setAttribute ( mf->create(), osg::StateAttribute::ON );
#endif

    // Make geometry and add to geode.
    geode->addDrawable ( geometry.get() );

    // Callback to set the bounding box.
    geometry->setComputeBoundingBoxCallback( new BoundingBoxCallback ( _cubes.at( i ) ) );

    // Add the vertices
    geometry->setVertexArray( vertices );
    
    //Set the right number of normals
    if( average )
    {
      //Set the normals
      geometry->setNormalArray ( normals );

      // Need per-vertex normals for osg::Geometry's "fast path".
      geometry->setNormalBinding ( osg::Geometry::BIND_PER_VERTEX );
    }
    else
    {
      // Per Primitive normals
      osg::ref_ptr < osg::Vec3Array > n ( new osg::Vec3Array );

      // Get the triangle list for this geometry
      const TriangleList &triangles ( _triangles.at( i ) );

      // Reserve enough room
      n->reserve ( triangles.size() );

      // Add the normals.
      for( TriangleList::const_iterator iter = triangles.begin(); iter != triangles.end(); ++ iter )
        n->push_back ( normals->at ( (*iter)->index() ) );

      //Set the normals
      geometry->setNormalArray ( n.get() );

      //Set the normal binding
      geometry->setNormalBinding ( osg::Geometry::BIND_PER_PRIMITIVE );
    }

    geometry->dirtyBound();
    geometry->dirtyDisplayList();
  }

#if DRAW_CUBES
  return root.release();
#else
  return geode.release();
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get primitive set index for given vertices.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Partition::_index ( const osg::Vec3& v0, const osg::Vec3& v1, const osg::Vec3& v2 )
{
  for( unsigned int i = 0; i < _cubes.size(); ++i )
  {
    if( _cubes.at(i).contains ( v0 ) )
      return i;
  }

  return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Subdive the partition.
//
///////////////////////////////////////////////////////////////////////////////

void Partition::subdivide ( const osg::BoundingBox &bb, unsigned int times )
{
  _subdivide ( bb, times );

  _triangles.resize( _cubes.size() );
}

void Partition::_subdivide ( const osg::BoundingBox &bb, unsigned int times )
{
  if( times == 0 )
  {
    _cubes.push_back( bb );

    _primitiveSets.push_back ( new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 0 ) );
    _geometries.push_back ( new osg::Geometry );
  
    // Add the PrimitiveSet
    _geometries.back()->addPrimitiveSet( _primitiveSets.back().get() );
  }
  else
  {
    osg::BoundingBox bb1, bb2;

    this->_divide ( bb, bb1, bb2 );

    this->_subdivide ( bb1, times - 1 );
    this->_subdivide ( bb2, times - 1 );
  }
}

void Partition::_divide ( const osg::BoundingBox &bb, osg::BoundingBox &bb1, osg::BoundingBox &bb2 )
{
  float x ( bb.xMax() - bb.xMin() );
  float y ( bb.yMax() - bb.yMin() );
  float z ( bb.zMax() - bb.zMin() );

  osg::Vec3 min1 ( bb.corner ( 0 ) );
  osg::Vec3 max1 ( bb.corner ( 7 ) );

  osg::Vec3 min2 ( bb.corner ( 0 ) );
  osg::Vec3 max2 ( bb.corner ( 7 ) );

  osg::Vec3 min ( bb.corner ( 0 ) );
  osg::Vec3 max ( bb.corner ( 7 ) );

  if ( x > y )
  {
    // Divide along x-axis
    if ( x > z )
    {
      float halfX ( ( max.x() - min.x() ) / 2 );

      max1.x() -= halfX;
      min2.x() += halfX;
    }
    // Divide along z-axis
    else
    {
      float halfZ ( ( max.z() - min.z() ) / 2 );

      max1.z() -= halfZ;
      min2.z() += halfZ;
    }
  }
  else
  {
    // Divide along y-axis
    if ( y > z )
    {
      float halfY ( ( max.y() - min.y() ) / 2 );

      max1.y() -= halfY;
      min2.y() += halfY;
    }
    // Divide along z-axis
    else
    {
      float halfZ ( ( max.z() - min.z() ) / 2 );

      max1.z() -= halfZ;
      min2.z() += halfZ;
    }
  }

  bb1.set ( min1, max1 );
  bb2.set ( min2, max2 );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the first index value of the triangle in the drawables primitive set.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Partition::index0 ( const osg::Drawable *d, unsigned int index ) const
{
  return this->_indexn ( d, index, 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the second index value of the triangle in the drawables primitive set.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Partition::index1 ( const osg::Drawable *d, unsigned int index ) const
{
  return this->_indexn ( d, index, 1 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the third index value of the triangle in the drawables primitive set.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Partition::index2 ( const osg::Drawable *d, unsigned int index ) const
{
  return this->_indexn ( d, index, 2 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the n'th index value of the triangle in the drawables primitive set.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int  Partition::_indexn ( const osg::Drawable *d, unsigned int index, unsigned int n ) const
{
  for( unsigned int i = 0; i < _geometries.size(); ++i )
  {
    if( _geometries.at( i ).get() == d )
    {
      return _primitiveSets.at( i )->at ( index * 3 + n );
    }
  }

  throw std::runtime_error ( "Drawable not in this partition." );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the first shared vertex of the triangle in the drawables primitive set.
//
///////////////////////////////////////////////////////////////////////////////

SharedVertex* Partition::sharedVertex0 ( const osg::Drawable *d, unsigned int i )
{
  return this->_triangle( d, i )->vertex0();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the second shared vertex of the triangle in the drawables primitive set.
//
///////////////////////////////////////////////////////////////////////////////

SharedVertex* Partition::sharedVertex1 ( const osg::Drawable *d, unsigned int i )
{
  return this->_triangle( d, i )->vertex1();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the third shared vertex of the triangle in the drawables primitive set.
//
///////////////////////////////////////////////////////////////////////////////

SharedVertex* Partition::sharedVertex2 ( const osg::Drawable *d, unsigned int i )
{
  return this->_triangle( d, i )->vertex2();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the first shared vertex of the triangle in the drawables primitive set.
//
///////////////////////////////////////////////////////////////////////////////

const SharedVertex* Partition::sharedVertex0 ( const osg::Drawable *d, unsigned int i ) const
{
  return this->_triangle( d, i )->vertex0();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the second shared vertex of the triangle in the drawables primitive set.
//
///////////////////////////////////////////////////////////////////////////////

const SharedVertex* Partition::sharedVertex1 ( const osg::Drawable *d, unsigned int i ) const
{
  return this->_triangle( d, i )->vertex1();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the third shared vertex of the triangle in the drawables primitive set.
//
///////////////////////////////////////////////////////////////////////////////

const SharedVertex* Partition::sharedVertex2 ( const osg::Drawable *d, unsigned int i ) const
{
  return this->_triangle( d, i )->vertex2();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the i'th triangle in the drawables primitive set.
//
///////////////////////////////////////////////////////////////////////////////

Triangle* Partition::_triangle( const osg::Drawable *d, unsigned int index )
{
  for( unsigned int i = 0; i < _geometries.size(); ++i )
  {
    if( _geometries.at( i ).get() == d )
    {
      return _triangles.at( i ).at( index );
    }
  }

  throw std::runtime_error ( "Drawable not in this partition." );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the i'th triangle in the drawables primitive set.
//
///////////////////////////////////////////////////////////////////////////////

const Triangle* Partition::_triangle( const osg::Drawable *d, unsigned int index ) const
{
  for( unsigned int i = 0; i < _geometries.size(); ++i )
  {
    if( _geometries.at( i ).get() == d )
    {
      return _triangles.at( i ).at( index );
    }
  }

  throw std::runtime_error ( "Drawable not in this partition." );
}

