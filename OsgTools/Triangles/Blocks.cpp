
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach & Perry Miller
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Triangles/Blocks.h"
#include "OsgTools/Triangles/TriangleSet.h"

#include "Usul/Errors/Assert.h"
#include "Usul/Exceptions/Thrower.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Math/Math.h"

#include <algorithm>
#include <functional>

using namespace OsgTools::Triangles;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Blocks::Blocks ( const osg::BoundingBox &box, unsigned int times, unsigned int reserve ) : BaseClass(),
  _bbox       ( box ),
  _volume     (),
  _sequence   (),
  _geode      ( new osg::Geode )
{
  // Make sure we have a valid bounding box.
  if ( false == _bbox.valid() )
    throw std::runtime_error ( "Error 2864437610: Invalid bounding box in triangle-set" );

  // Subdivide into blocks.
  this->_subdivide ( times, reserve );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Blocks::~Blocks()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a triangle.
//
///////////////////////////////////////////////////////////////////////////////

void Blocks::addTriangle ( TriangleSet *ts, Triangle *t )
{
  // Check input.
  if ( 0x0 == ts )
    throw std::invalid_argument ( "Error 3640849883: null triangle-set given" );
  if ( 0x0 == t )
    throw std::invalid_argument ( "Error 3376676199: null triangle given" );

  // Get the appropriate block.
  Block::RefPtr b ( this->block ( ts, t ) );

  // Add the triangle to the block.
  if ( b.valid() )
    b->addTriangle ( ts, t );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the display list flag. Assume they are all set the same as the first.
//
///////////////////////////////////////////////////////////////////////////////

bool Blocks::displayList() const
{
  return ( ( _sequence.empty() ) ? false : _sequence.front()->displayList() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the display list flag.
//
///////////////////////////////////////////////////////////////////////////////

void Blocks::displayList ( bool state )
{
  const unsigned int numBlocks ( _sequence.size() );
  for ( unsigned int i = 0; i < numBlocks; ++i )
    _sequence[i]->displayList ( state );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Purge any excess memory.
//
///////////////////////////////////////////////////////////////////////////////

void Blocks::purge()
{
  std::for_each ( _sequence.begin(), _sequence.end(), std::mem_fun ( &Block::purge ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the node.
//
///////////////////////////////////////////////////////////////////////////////

osg::Geode *Blocks::buildScene ( const Options &options, TriangleSet *ts )
{
  // Check input.
  if ( 0x0 == ts )
    throw std::invalid_argument ( "Error 1098244047: null triangle-set given" );

  // Remove all existing drawables.
  _geode->removeDrawable ( 0, _geode->getNumDrawables() );

  // Loop through the blocks and add the geometry.
  const unsigned int numBlocks ( _sequence.size() );
  for ( unsigned int i = 0; i < numBlocks; ++i )
    _geode->addDrawable ( _sequence[i]->buildScene ( options, ts ) );

  // Flag the geode as dirty and return it.
  _geode->dirtyBound();
  return _geode.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to get the index for the floating-point value.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class Real > inline unsigned int index ( Real v, unsigned int d, Real mn, Real mx )
  {
    // Should be true.
    USUL_ASSERT ( d > 0 );

    // Get the range.
    const Real r ( mx - mn );

    // Get the normalized value.
    Real n ( d * ( v - mn ) / ( r ) );

    // Keep it in range.
    n = ( ( n > r ) ? ( r ) : ( ( n < 0 ) ? ( 0 ) : ( n ) ) );

    // Convert to unsigned integer.
    const unsigned int i ( static_cast < const unsigned int > ( n ) );

    // Return the index. Special-case the extrema.
    return ( ( d == i ) ? d - 1 : i );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the vertex to indices.
//
///////////////////////////////////////////////////////////////////////////////

void Blocks::_indices ( const osg::Vec3f &v, Usul::Math::Vec3ui &i ) const
{
  // Should be true.
  USUL_ASSERT ( false == _volume.empty() );
  USUL_ASSERT ( false == _volume[0].empty() );
  USUL_ASSERT ( false == _volume[0][0].empty() );
  USUL_ASSERT ( _bbox.valid() );

  // Get the sizes.
  const Usul::Math::Vec3ui size ( _volume.size(), _volume[0].size(), _volume[0][0].size() );

  // Get the indices.
  i[0] = Helper::index ( v[0], size[0], _bbox.xMin(), _bbox.xMax() );
  i[1] = Helper::index ( v[1], size[1], _bbox.yMin(), _bbox.yMax() );
  i[2] = Helper::index ( v[2], size[2], _bbox.zMin(), _bbox.zMax() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the block that contains the triangle. Use the first vertex.
//
///////////////////////////////////////////////////////////////////////////////

const Block *Blocks::block ( const TriangleSet *ts, const Triangle *t ) const
{
  // Check input.
  if ( 0x0 == ts )
    throw std::invalid_argument ( "Error 1269059241: null triangle-set given" );
  if ( 0x0 == t )
    throw std::invalid_argument ( "Error 3613986077: null triangle given" );

  // Use center of triangle.
  return this->block ( ts->triangleCenter ( t->index() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the block that contains the triangle. Use the first vertex.
//
///////////////////////////////////////////////////////////////////////////////

Block *Blocks::block ( const TriangleSet *ts, const Triangle *t )
{
  // Check input.
  if ( 0x0 == ts )
    throw std::runtime_error ( "Error 1881345218: null triangle-set given" );
  if ( 0x0 == t )
    throw std::runtime_error ( "Error 2154079742: null triangle given" );

  // Use center of triangle.
  return this->block ( ts->triangleCenter ( t->index() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the block that contains the vertex.
//
///////////////////////////////////////////////////////////////////////////////

const Block *Blocks::block ( const osg::Vec3f &v ) const
{
  // Get the indices.
  Usul::Math::Vec3ui i ( 0, 0, 0 );
  this->_indices ( v, i );

  // Return the block.
  return _volume.at(i[0]).at(i[1]).at(i[2]);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the block that contains the vertex.
//
///////////////////////////////////////////////////////////////////////////////

Block *Blocks::block ( const osg::Vec3f &v )
{
  // Get the indices.
  Usul::Math::Vec3ui i ( 0, 0, 0 );
  this->_indices ( v, i );

  // Return the block.
  return _volume.at(i[0]).at(i[1]).at(i[2]);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

void Blocks::updateBounds ( const TriangleSet *ts, const Triangle *t )
{
  Block::RefPtr b ( this->block ( ts, t ) );
  if ( b.valid() )
  {
    b->updateBounds ( ts, t );
    _bbox.expandBy ( b->boundingBox() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

void Blocks::updateBounds ( const TriangleSet *ts )
{
  const unsigned int numBlocks ( _sequence.size() );
  for ( unsigned int i = 0; i < numBlocks; ++i )
  {
    _sequence[i]->updateBounds ( ts );
    _bbox.expandBy ( _sequence[i]->boundingBox() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function for splitting.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class T > split ( const T &t1, T &t2, unsigned int &times )
  {
    if ( t1 == t2 && times > 0 )
    {
      t2 *= 0.5f;
      --times;
      return 1;
    }
    return 0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Subdivide the blocks. Needs to be called before triangles are added.
//
///////////////////////////////////////////////////////////////////////////////

void Blocks::_subdivide ( unsigned int times, unsigned int reserve )
{
  // Should be true.
  USUL_ASSERT ( _sequence.empty() );
  USUL_ASSERT ( _volume.empty() );
  USUL_ASSERT ( _bbox.valid() );

  // Initialize a vector of lengths.
  Usul::Math::Vec3f length ( _bbox.xMax() - _bbox.xMin(), 
                             _bbox.yMax() - _bbox.yMin(), 
                             _bbox.zMax() - _bbox.zMin() );

  // Initialize indices indicating the direction to split.
  Usul::Math::Vec3ui direction ( 0, 0, 0 );

  // Loop for the specified number of times.
  while ( times )
  {
    // Determine the longest edge.
    const float dm ( Usul::Math::maximum ( length[0], length[1], length[2] ) );

    // Split in the appropriate direction.
    direction[0] += Helper::split ( dm, length[0], times );
    direction[1] += Helper::split ( dm, length[1], times );
    direction[2] += Helper::split ( dm, length[2], times );
  }

  // Determine the size of the 3D array.
  Usul::Math::Vec3ui size ( Usul::Math::pow < unsigned int > ( 2, direction[0] ),
                            Usul::Math::pow < unsigned int > ( 2, direction[1] ),
                            Usul::Math::pow < unsigned int > ( 2, direction[2] ) );

  // Resize the containers.
  this->_resize ( size, reserve );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to convert loop variables to floating-point percentage.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline float convert ( unsigned int index, unsigned int size )
  {
    const double numerator ( static_cast < double > ( index ) );
    const double denominator ( static_cast < double > ( size - 1 ) );
    const float answer ( static_cast < float > ( numerator / denominator ) );
    return answer;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to convert loop variables to bounding boxes.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class SizeType > inline 
  void setBounds ( unsigned int index,          // Where we are in the loop.
                   unsigned int direction,      // 0, 1, or 2
                   const SizeType &num,         // Total number of times we are looping.
                   const osg::Vec3f &blockSize, // Size of each block.
                   const osg::BoundingBox &big, // Bounding-box containing all the blocks.
                   osg::BoundingBox &little )   // Bounding-box for one block.
  {
    // For readability.
    const unsigned int d ( direction );
    const unsigned int total ( num[d] );
    const bool last ( index + 1 == total );
    const float bigMin ( big._min[d] );
    const float bigMax ( big._max[d] );
    const float size ( blockSize[d] );

    // Set the min and max.
    little._min[d] = bigMin + size * index;
    little._max[d] = ( ( last ) ? bigMax : little._min[d] + size );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Resize the containers.
//
///////////////////////////////////////////////////////////////////////////////

void Blocks::_resize ( const Usul::Math::Vec3ui &num, unsigned int numTrianglesToReserve )
{
  USUL_ASSERT ( _bbox.valid() );

  // Handle bad size.
  const unsigned int total ( num[0] * num[1] * num[2] );
  if ( 0 == total )
  {
    Usul::Exceptions::Thrower<std::runtime_error>
      ( "Error 2040020463: blocks cannot be size (", num[0], ",", num[1], ",", num[2], ")" );
  }

  // Calculate how many triangle each block should reserve.
  numTrianglesToReserve = ( numTrianglesToReserve * 3 ) / ( total * 2 );

  // Initialize the sequence.
  _sequence.clear();
  _sequence.reserve ( total );

  // Initialize the size of the little bounding boxes.
  const osg::Vec3f blockSize ( ( _bbox.xMax() - _bbox.xMin() ) / num[0], 
                               ( _bbox.yMax() - _bbox.yMin() ) / num[1], 
                               ( _bbox.zMax() - _bbox.zMin() ) / num[2] );

  // Bounding-box for each block. We update this as we loop.
  osg::BoundingBox littleBox;

  // Loop through the dimensions and size the volume.
  _volume.resize ( num[0] );
  for ( unsigned int i = 0; i < num[0]; ++i )
  {
    // Resize and update the bounding-box minimum.
    _volume[i].resize ( num[1] );
    Helper::setBounds ( i, 0, num, blockSize, _bbox, littleBox );

    for ( unsigned int j = 0; j < num[1]; ++j )
    {
      // Clear and reserve space, and update the bounding-box minimum.
      _volume[i][j].clear();
      _volume[i][j].reserve ( num[2] );
      Helper::setBounds ( j, 1, num, blockSize, _bbox, littleBox );

      for ( unsigned int k = 0; k < num[2]; ++k )
      {
        // Update the bounding-box.
        Helper::setBounds ( k, 2, num, blockSize, _bbox, littleBox );
        USUL_ASSERT ( littleBox.valid() );

        // Make the block.
        Block::ValidRefPtr b ( new Block ( littleBox, numTrianglesToReserve ) );

        // Add it to the containers.
        _volume[i][j].push_back ( b );
        _sequence.push_back ( b );
      }
    }
  }

  // Should be true.
  USUL_ASSERT ( total == _sequence.size() );
}
