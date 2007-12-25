
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __STAR_SYSTEM_EXTENTS_H__
#define __STAR_SYSTEM_EXTENTS_H__

#include "Usul/Math/MinMax.h"

#include <stdexcept>


namespace StarSystem {


template < class VertexType > class Extents
{
public:

  typedef VertexType Vertex;
  typedef typename Vertex::value_type value_type;
  typedef value_type ValueType;

  /// Construction/Destruction.
  Extents();
  Extents ( ValueType minLon, ValueType minLat, ValueType maxLon, ValueType maxLat );
  Extents ( const Vertex& min, const Vertex& max );
  Extents ( const Extents& rhs );
  ~Extents();

  /// Assignment.
  Extents& operator = ( const Extents& rhs );

  /// Expand by the extents.
  void                  expand ( const Extents& extents );

  /// Does the extent intersect this extent.
  bool                  intersects ( const Extents& extents ) const;

  /// Get the min.
  const Vertex &        minimum() const;
  ValueType             minLon() const;
  ValueType             minLat() const;

  /// Get the max.
  const Vertex &        maximum() const;
  ValueType             maxLon() const;
  ValueType             maxLat() const;

  /// Bracket operator, largely for serialization.
  ValueType             operator [] ( unsigned int ) const;
  ValueType &           operator [] ( unsigned int );

private:

  Vertex _min;
  Vertex _max;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Macro to help shorten the lines.
//
///////////////////////////////////////////////////////////////////////////////

#define ThisClass Extents<VertexType>


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template < class VertexType > inline ThisClass::Extents() :
  _min ( 0.0, 0.0 ),
  _max ( 0.0, 0.0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template < class VertexType > inline ThisClass::Extents ( const Vertex& min, const Vertex& max ) :
  _min ( min ),
  _max ( max )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template < class VertexType > inline ThisClass::Extents ( ValueType minLon, ValueType minLat, ValueType maxLon, ValueType maxLat ) :
  _min ( minLon, minLat ),
  _max ( maxLon, maxLat )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template < class VertexType > inline ThisClass::Extents ( const Extents& rhs ) :
  _min ( rhs._min ),
  _max ( rhs._max )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Assignment.
//
///////////////////////////////////////////////////////////////////////////////

template < class VertexType > inline ThisClass& ThisClass::operator = ( const ThisClass& rhs )
{
  _min = rhs._min;
  _max = rhs._max;
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

template < class VertexType > inline ThisClass::~Extents()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the min.
//
///////////////////////////////////////////////////////////////////////////////

template < class VertexType > inline const typename ThisClass::Vertex &ThisClass::minimum() const
{
  return _min;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the max.
//
///////////////////////////////////////////////////////////////////////////////

template < class VertexType > inline const typename ThisClass::Vertex &ThisClass::maximum() const
{
  return _max;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the min latitude.
//
///////////////////////////////////////////////////////////////////////////////

template < class VertexType > inline typename ThisClass::ValueType ThisClass::minLat() const
{
  return _min[1];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the max latitude.
//
///////////////////////////////////////////////////////////////////////////////

template < class VertexType > inline typename ThisClass::ValueType ThisClass::maxLat() const
{
  return _max[1];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the min longitide.
//
///////////////////////////////////////////////////////////////////////////////

template < class VertexType > inline typename ThisClass::ValueType ThisClass::minLon() const
{
  return _min[0];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the max longitide.
//
///////////////////////////////////////////////////////////////////////////////

template < class VertexType > inline typename ThisClass::ValueType ThisClass::maxLon() const
{
  return _max[0];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Bracket operator to help with serialization.
//
///////////////////////////////////////////////////////////////////////////////

template < class VertexType > inline typename ThisClass::ValueType ThisClass::operator [] ( unsigned int i ) const
{
  if ( i > 3 )
  {
    throw std::runtime_error ( "Error 2037139798: Index out of range" );
  }
  return ( ( i < 2 ) ? _min[i] : _max[i-2] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Bracket operator to help with serialization.
//
///////////////////////////////////////////////////////////////////////////////

template < class VertexType > inline typename ThisClass::ValueType &ThisClass::operator [] ( unsigned int i )
{
  if ( i > 3 )
  {
    throw std::runtime_error ( "Error 8676398430: Index out of range" );
  }
  return ( ( i < 2 ) ? _min[i] : _max[i-2] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Expand by the extents.
//
///////////////////////////////////////////////////////////////////////////////

template < class VertexType > inline void ThisClass::expand ( const Extents& extents )
{
  Vertex zero ( 0.0, 0.0 );

  // If we are invalid.
  if ( zero == _min && zero == _max )
  {
    _min = extents.minimum();
    _max = extents.maximum();
  }
  else
  {
    const Vertex mn ( extents.minimum() );
    const Vertex mx ( extents.maximum() );

    _min[0] = Usul::Math::minimum ( _min[0], mn[0] );
    _min[1] = Usul::Math::minimum ( _min[1], mn[1] );

    _max[0] = Usul::Math::maximum ( _max[0], mx[0] );
    _max[1] = Usul::Math::maximum ( _max[1], mx[1] );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Does the extent intersect this extent.
//
///////////////////////////////////////////////////////////////////////////////

template < class VertexType > inline bool ThisClass::intersects ( const Extents& extents ) const
{
  const Vertex mn ( extents.minimum() );
  const Vertex mx ( extents.maximum() );
  
  return ( Usul::Math::maximum ( _min[0], mn[0] ) <= Usul::Math::minimum ( _max[0], mx[0] ) &&
           Usul::Math::maximum ( _min[1], mn[1] ) <= Usul::Math::minimum ( _max[1], mx[1] ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clean up macro definition.
//
///////////////////////////////////////////////////////////////////////////////

#undef ThisClass


} // namespace StarSystem


#endif // __STAR_SYSTEM_EXTENTS_H__
