
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 3003, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A pool of 3D vectors.
//
///////////////////////////////////////////////////////////////////////////////

#include "GSG/Core/Precompiled.h"
#include "GSG/Core/Vec3Pool.h"
#include "GSG/Core/Functors.h"
#include "GSG/Core/Container.h"

#include "boost/mpl/assert_is_same.hpp"

#include <algorithm>


using namespace GSG;

GSG_IMPLEMENT_CLONE ( Vec3Pool );


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

Vec3Pool::Vec3Pool() : Referenced(), 
  _s(), 
  _m()
{
  // Empty.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
/////////////////////////////////////////////////////////////////////////////

Vec3Pool::Vec3Pool ( const Vec3Pool &p ) : Referenced ( p ), 
  _s ( p._s ), 
  _m ( p._m )
{
  // Empty.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
/////////////////////////////////////////////////////////////////////////////

Vec3Pool::~Vec3Pool()
{
  // Empty.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Access to the elements.
//
///////////////////////////////////////////////////////////////////////////////

const Vec3Pool::value_type &Vec3Pool::operator [] ( Vec3Pool::size_type i ) const
{
  Lock lock ( this );
  BoundsChecker ( _s.size(), i );
  return GSG::constReference ( _s, i );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Access to the elements.
//
///////////////////////////////////////////////////////////////////////////////

Vec3Pool::value_type &Vec3Pool::operator [] ( Vec3Pool::size_type i )
{
  Lock lock ( this );
  BoundsChecker ( _s.size(), i );
  return GSG::reference ( _s, i );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Look for (and maybe insert) the given vertex. Return the index.
//
///////////////////////////////////////////////////////////////////////////////

SeqVec3::size_type Vec3Pool::operator [] ( const Vec3Pool::value_type &v )
{
  Lock lock ( this );
  typedef MapVec3::iterator MapItr;
  typedef MapVec3::value_type MapValue;

  // Look for the vector.
  MapItr i = _m.find ( v );

  // If we did not find it...
  if ( _m.end() == i )
  {
    // Get the current size of the sequence.
    size_type index ( _s.size() );

    // Insert it into the map. The value is the index in the sequence.
    _m.insert ( MapValue ( v, index ) );

    // Insert it into the sequence.
    _s.insert ( _s.end(), v );

    // Return the index.
    return index;
  }

  else
  {
    return i->second;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Resize the pool.
//
///////////////////////////////////////////////////////////////////////////////

void Vec3Pool::resize ( Vec3Pool::size_type s )
{
  BOOST_MPL_ASSERT_IS_SAME ( SeqVec3::size_type,  MapVec3::size_type );
  BOOST_MPL_ASSERT_IS_SAME ( SeqVec3::value_type, MapVec3::key_type );
  BOOST_MPL_ASSERT_IS_SAME ( Indices::size_type,  MapVec3::value_type::second_type );

  // Thread lock and error check.
  Lock lock ( this );
  ErrorChecker ( _s.size() == _m.size() );

  // Resize the vector.
  _s.resize ( s );

  // Clear the map.
  _m.clear();

  // Load the map. Vec3Pool::value_type is actually the key for the map.
  std::for_each ( _s.begin(), _s.end(), Detail::MapInserter < MapVec3 > ( _m ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the size.
//
///////////////////////////////////////////////////////////////////////////////

Vec3Pool::size_type Vec3Pool::size() const
{
  BOOST_MPL_ASSERT_IS_SAME ( SeqVec3::size_type, MapVec3::size_type );
  Lock lock ( this );
  ErrorChecker ( _s.size() == _m.size() );
  return _s.size();
}
