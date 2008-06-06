
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Hash table with buckets. The API should be similar to std::map.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_CONTAINERS_HASH_CLASS_H_
#define _USUL_CONTAINERS_HASH_CLASS_H_

#include "Usul/Math/Functions.h"

#include <vector>
#include <algorithm>
#include <functional>
#include <sstream>


namespace Usul {
namespace Containers {


///////////////////////////////////////////////////////////////////////////////
//
//  Start of hash-function namespace.
//
///////////////////////////////////////////////////////////////////////////////

namespace HashFunctions {


///////////////////////////////////////////////////////////////////////////////
//
//  Generic definition of hash function for vectors of known size.
//
///////////////////////////////////////////////////////////////////////////////

template < class IndexType, class ScalarType, unsigned int Dimension > struct Vector;


///////////////////////////////////////////////////////////////////////////////
//
//  Specialization of above for 3D vectors.
//
///////////////////////////////////////////////////////////////////////////////

template < class IndexType, class ScalarType > struct Vector < IndexType, ScalarType, 3 >
{
  Vector() : _minDist ( 0 ), _maxDist ( 1 )
  {
  }
  template < class KeyType, class BucketContainer > IndexType operator () ( const KeyType &key, const BucketContainer &b )
  {
    typename BucketContainer::size_type s ( b.size() );
    const ScalarType x  ( key[0] );
    const ScalarType y  ( key[1] );
    const ScalarType z  ( key[2] );
    const ScalarType d2 ( x * x + y * y + z * z );
    const ScalarType d  ( Usul::Math::sqrt ( d2 ) );
    const ScalarType mn ( _minDist );
    const ScalarType mx ( _maxDist );
    const ScalarType nd ( ( ( d - mn ) / ( mx - mn ) ) * s );
    const IndexType index ( static_cast < const IndexType > ( nd ) );
    return ( ( index < s ) ? index : s - 1 );
  }
  void range ( const ScalarType &minX, const ScalarType &minY, const ScalarType &minZ,
               const ScalarType &maxX, const ScalarType &maxY, const ScalarType &maxZ )
  {
    _minDist = Usul::Math::sqrt ( minX * minX + minY * minY + minZ * minZ );
    _maxDist = Usul::Math::sqrt ( maxX * maxX + maxY * maxY + maxZ * maxZ );
  }
private:
  ScalarType _minDist, _maxDist;
};


///////////////////////////////////////////////////////////////////////////////
//
//  End of hash-function namespace.
//
///////////////////////////////////////////////////////////////////////////////

}


///////////////////////////////////////////////////////////////////////////////
//
//  Hash table with buckets.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class KeyType_, 
  class ValueType_, 
  class FunctionType_,
  class EqualPred_ = std::less < KeyType_ >
>
class Hash
{
public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Typedefs
  //
  /////////////////////////////////////////////////////////////////////////////

  typedef KeyType_                                                KeyType;
  typedef ValueType_                                              ValueType;
  typedef FunctionType_                                           FunctionType;
  typedef EqualPred_                                              EqualPred;
  typedef std::pair < KeyType, ValueType >                        PairType;
  typedef std::vector < PairType >                                Bucket;
  typedef std::vector < Bucket >                                  Buckets;
  typedef typename Buckets::size_type                             SizeType;
  typedef Hash < KeyType, ValueType, FunctionType, EqualPred >    ThisType;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Default construction.
  //
  /////////////////////////////////////////////////////////////////////////////

  explicit Hash() : 
    _buckets(), 
    _function(),
    _equality(),
    _autoResize ( false )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Construction that sets all members.
  //
  /////////////////////////////////////////////////////////////////////////////

  explicit Hash ( SizeType n, FunctionType f, EqualPred e, bool r ) : 
    _buckets    ( n ), 
    _function   ( f ),
    _equality   ( e ),
    _autoResize ( r )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Copy construction.
  //
  /////////////////////////////////////////////////////////////////////////////

  Hash ( const ThisType &h ) : 
    _buckets    ( h._buckets    ), 
    _function   ( h._function   ),
    _equality   ( h._equality   ),
    _autoResize ( h._autoResize )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Assignment.
  //
  /////////////////////////////////////////////////////////////////////////////

  ThisType &operator = ( const ThisType &h )
  {
    _buckets    = h._buckets;
    _function   = h._function;
    _equality   = h._equality;
    _autoResize = h._autoResize;
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set/get the auto-resize flag.
  //
  /////////////////////////////////////////////////////////////////////////////

  void autoResize ( bool state )
  {
    _autoResize = state;
  }
  bool autoResize() const
  {
    return _autoResize;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set/get number of buckets.
  //
  /////////////////////////////////////////////////////////////////////////////

  void buckets ( SizeType s )
  {
    _buckets.resize ( s );
  }
  SizeType buckets() const
  {
    return _buckets.size();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set/get hash function.
  //
  /////////////////////////////////////////////////////////////////////////////

  FunctionType &function()
  {
    return _function;
  }
  const FunctionType &function() const
  {
    return _function;
  }
  void function ( const FunctionType &f )
  {
    _function = f;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set/get equality predicate.
  //
  /////////////////////////////////////////////////////////////////////////////

  EqualPred &equality()
  {
    return _equality;
  }
  const EqualPred &equality() const
  {
    return _equality;
  }
  void equality ( const EqualPred &f )
  {
    _equality = f;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Clear the elements.
  //
  /////////////////////////////////////////////////////////////////////////////

  void clear()
  {
    _buckets.clear();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Access the value at the given key.
  //
  /////////////////////////////////////////////////////////////////////////////

  ValueType &operator [] ( const KeyType &key )
  {
    // Calculate index from hash function.
    const SizeType index ( _function ( key, _buckets ) );

    // Resize if we need to, and if we are allowed.
    if ( index >= _buckets.size() )
    {
      if ( this->autoResize() )
      {
        _buckets.resize ( index + 1 );
      }
      else
      {
        std::ostringstream out;
        out << "Error 1759653286: given key hashes to an index of " 
            << index 
            << ", but the hash table is configured with a maximum size of " 
            << _buckets.size();
        throw std::range_error ( out.str() );
      }
    }

    // Get the bucket.
    Bucket &bucket ( _buckets.at ( index ) );

    // Depending on the number of buckets...
    switch ( bucket.size() )
    {
    case 0:

      // Insert the first element and return the value.
      bucket.push_back ( PairType ( key, ValueType() ) );
      return bucket.back().second;

    case 1:

      // Check the keys and return the value if they match.
      if ( _equality ( bucket.back().first, key ) )
        return bucket.back().second;
      break;

    default:

      // Look for the correct value by comparing the keys.
      for ( typename Bucket::iterator i = bucket.begin(); i != bucket.end(); ++i )
      {
        PairType &p ( *i );
        KeyType  &k ( p.first );
        if ( _equality ( k, key ) )
          return p.second;
      }
    }

    // If we get to here then append a new pair and return the value.
    bucket.push_back ( PairType ( key, ValueType() ) );
    return bucket.back().second;
  }

private:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Data members.
  //
  /////////////////////////////////////////////////////////////////////////////

  Buckets _buckets;
  FunctionType _function;
  EqualPred _equality;
  bool _autoResize;
};


} // namespace Containers
} // namespace Usul


#endif // _USUL_CONTAINERS_HASH_CLASS_H_
