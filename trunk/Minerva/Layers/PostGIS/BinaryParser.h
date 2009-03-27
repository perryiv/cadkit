
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Use this parser when using the asBinary postGIS function.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_POSTGIS_BINARY_PARSER_H__
#define __MINERVA_POSTGIS_BINARY_PARSER_H__

#include "Minerva/Layers/PostGIS/Export.h"

#include "Minerva/Core/Data/Geometry.h"
#include "Minerva/Core/Data/Point.h"
#include "Minerva/Core/Data/Line.h"
#include "Minerva/Core/Data/Polygon.h"
#include "Minerva/Core/Data/WKBParsers.h"

#include "Usul/Endian/Endian.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Types/Types.h"
#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include <vector>
#include <stack>

namespace Minerva {
namespace Layers {
namespace PostGIS {


///////////////////////////////////////////////////////////////////////////////
//
//  Pass through transform.
//
///////////////////////////////////////////////////////////////////////////////

struct MINERVA_POSTGIS_EXPORT PassThroughTransform
{
  Usul::Math::Vec3d operator() ( const Usul::Math::Vec3d& vertex ) const
  {
    return vertex;
  }
};


template<class ConstIterator,class Transform>
class BinaryParser
{
public:
  
  typedef Minerva::Core::Data::Geometry Geometry;
  typedef std::vector < Geometry::RefPtr > Geometries;
  typedef Minerva::Core::Data::Point Point;
  typedef Minerva::Core::Data::Line Line;
  typedef Minerva::Core::Data::Polygon Polygon;
  typedef Line::Vertices Vertices;
  typedef Minerva::Core::Data::BaseParser<ConstIterator,Transform> BaseParser;

  enum wkbGeometryType 
  {
    wkbPoint = BaseParser::wkbPoint,
    wkbLineString = BaseParser::wkbLineString,
    wkbPolygon = BaseParser::wkbPolygon,
    wkbMultiPoint = BaseParser::wkbMultiPoint,
    wkbMultiLineString = BaseParser::wkbMultiLineString,
    wkbMultiPolygon = BaseParser::wkbMultiPolygon,
    wkbGeometryCollection = BaseParser::wkbGeometryCollection
  };

  enum wkbEndianType
  {
    wkbBigEndian = BaseParser::wkbBigEndian,
    wkbLittleEndian = BaseParser::wkbLittleEndian
  };

  BinaryParser ( ConstIterator begin, ConstIterator end, const Transform& transform );
  ~BinaryParser();

  /// Parse and create geomtry.
  inline Geometries createGeometry();

protected:

  Geometry* _createPoint();
  Line* _createLine();
  Polygon* _createPolygon();

  inline void _createGeometry ( Geometries& );

  void _readVertices ( Vertices& vertices );
  void _readByte ( Usul::Types::Uint8& value );
  void _readAndConvert ( Usul::Types::Uint32& value );
  void _readAndConvert ( Usul::Types::Float64& value );

  void _preCheck ( unsigned int size ) const;
  void _postCheck() const;

private:

  BinaryParser();

  std::stack<Usul::Types::Uint8> _endianess;
  
  ConstIterator _begin;
  ConstIterator _end;
  ConstIterator _current;

  Transform _transform;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class ConstIterator,class Transform>
BinaryParser<ConstIterator,Transform>::BinaryParser ( ConstIterator begin, ConstIterator end, const Transform& transform ) :
  _endianess(),
  _begin ( begin ),
  _end ( end ),
  _current ( begin ),
  _transform ( transform )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class ConstIterator,class Transform>
BinaryParser<ConstIterator,Transform>::~BinaryParser()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse and create geomtry.
//
///////////////////////////////////////////////////////////////////////////////

template <class ConstIterator,class Transform>
inline typename BinaryParser<ConstIterator,Transform>::Geometries BinaryParser<ConstIterator,Transform>::createGeometry()
{
  Geometries geometries;
  this->_createGeometry ( geometries );
  return geometries;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse and create geomtry with given endian converter.
//
///////////////////////////////////////////////////////////////////////////////

template <class ConstIterator,class Transform>
inline void BinaryParser<ConstIterator,Transform>::_createGeometry ( Geometries &geometries )
{
  Usul::Types::Uint8 endian ( 0 );
  this->_readByte ( endian );
	_endianess.push ( endian );

  Usul::Types::Uint32 polygonType;
  this->_readAndConvert ( polygonType );

	switch ( polygonType )
	{
	case wkbPoint:       geometries.push_back ( this->_createPoint() ); break;
  case wkbLineString:  geometries.push_back ( this->_createLine() ); break;
  case wkbPolygon:     geometries.push_back ( this->_createPolygon() ); break;
	case wkbMultiPoint:
		{
      // How many points will we have?
      Usul::Types::Uint32 numPoints;
      this->_readAndConvert ( numPoints );

      // Loop through and add the points.
      for( Usul::Types::Uint32 i = 0; i < numPoints; ++i )
			{
        this->_createGeometry ( geometries );
      }
		}
		break;
	case wkbMultiLineString:
		{
      // Get the number of lines to expect.
      Usul::Types::Uint32 numLines;
      this->_readAndConvert ( numLines );

			for( Usul::Types::Uint32 i = 0; i < numLines; ++i )
			{
        this->_createGeometry ( geometries );
      }
		}
		break;
	case wkbMultiPolygon:
		{
      // Get the number of polygons to expect.
      Usul::Types::Uint32 numPolygons;
      this->_readAndConvert ( numPolygons );

			for( Usul::Types::Uint32 i = 0; i < numPolygons; ++i )
			{
        this->_createGeometry ( geometries );
			}
		}
		break;
	case wkbGeometryCollection:
		{
      this->_createGeometry ( geometries );
		}
		break;
  default:
    throw std::runtime_error ( "Error 4919319700: Unknown geometry type." );
	}

  _endianess.pop();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a point.
//
///////////////////////////////////////////////////////////////////////////////

template<class ConstIterator,class Transform>
typename BinaryParser<ConstIterator,Transform>::Geometry* BinaryParser<ConstIterator,Transform>::_createPoint()
{
  _current = _current - 5;

  Minerva::Core::Data::ParsePoint<ConstIterator,Transform>::RefPtr parsePoint ( new Minerva::Core::Data::ParsePoint<ConstIterator,Transform> ( _transform ) );

  Usul::Types::Uint32 size ( 0 );
  Geometry::RefPtr point ( parsePoint->createGeometry ( _current, _end, size ) );
  _current += size;

  return point.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a line.
//
///////////////////////////////////////////////////////////////////////////////

template<class ConstIterator,class Transform>
typename BinaryParser<ConstIterator,Transform>::Line* BinaryParser<ConstIterator,Transform>::_createLine()
{
  Line::RefPtr line ( new Line );

  Vertices vertices;
  this->_readVertices ( vertices );

  line->line( vertices );

  return line.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a polygon.
//
///////////////////////////////////////////////////////////////////////////////

template<class ConstIterator,class Transform>
typename BinaryParser<ConstIterator,Transform>::Polygon* BinaryParser<ConstIterator,Transform>::_createPolygon()
{
  Polygon::RefPtr polygon ( new Polygon );

  Usul::Types::Uint32 numRings;
  this->_readAndConvert ( numRings );

  Vertices vertices;
  for ( Usul::Types::Uint32 i = 0; i < numRings; ++i )
  {
    this->_readVertices ( vertices );
  }

  polygon->line ( vertices );

  return polygon.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Predicate for comparing vertices.  Should probably use CloseFloat for robustness.
//
///////////////////////////////////////////////////////////////////////////////

struct VectorCompare : std::binary_function< Usul::Math::Vec2d,
                                             Usul::Math::Vec2d,
                                             bool >
{
  template < class T >
  bool operator() ( const T& lhs, const T& rhs ) const
  {
    return lhs.equal( rhs );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Create vertices from binary string.
//
///////////////////////////////////////////////////////////////////////////////

template<class ConstIterator,class Transform>
void BinaryParser<ConstIterator,Transform>::_readVertices ( Vertices& vertices )
{
  typedef Vertices::value_type Vertex;

  Usul::Types::Uint32 numPoints;
  this->_readAndConvert ( numPoints );
  vertices.reserve ( numPoints );

  for ( Usul::Types::Uint32 i = 0; i < numPoints; ++i )
  {
    Vertex vertex ( 0.0, 0.0, 0.0 );
    this->_readAndConvert ( vertex[0] );
    this->_readAndConvert ( vertex[1] );
    
    vertices.push_back ( _transform ( vertex ) );
  }

  // Remove any duplicate vertices.
  vertices.erase ( std::unique ( vertices.begin(), vertices.end(), VectorCompare() ), vertices.end() );
}


template<class ConstIterator,class Transform>
void BinaryParser<ConstIterator,Transform>::_readByte ( Usul::Types::Uint8& value )
{
  value = *_current;
  _current += sizeof ( value );
}


template<class ConstIterator,class Transform>
void BinaryParser<ConstIterator,Transform>::_readAndConvert ( Usul::Types::Uint32& value )
{
  const unsigned int size ( sizeof ( Usul::Types::Uint32 ) );

  this->_preCheck ( size );

  const Usul::Types::Uint32 *pointer ( reinterpret_cast<const Usul::Types::Uint32 *> ( &(*_current) ) );
  value = *pointer;
  _current += size;

  this->_postCheck();

  if ( _endianess.top() == wkbBigEndian )
  {
    Usul::Endian::FromBigToSystem::convert ( value );
  }
  else
  {
    Usul::Endian::FromLittleToSystem::convert ( value );
  }
}

template<class ConstIterator,class Transform>
void BinaryParser<ConstIterator,Transform>::_readAndConvert ( Usul::Types::Float64& value )
{
  const unsigned int size ( sizeof ( Usul::Types::Float64 ) );

  this->_preCheck ( size );

  const Usul::Types::Float64 *pointer ( reinterpret_cast<const Usul::Types::Float64 *> ( &(*_current) ) );
  value = *pointer;
  _current += size;

  this->_postCheck();

  if ( _endianess.top() == wkbBigEndian )
  {
    Usul::Endian::FromBigToSystem::convert ( value );
  }
  else
  {
    Usul::Endian::FromLittleToSystem::convert ( value );
  }
}

template<class ConstIterator,class Transform>
void BinaryParser<ConstIterator,Transform>::_preCheck ( unsigned int size ) const
{
}

template<class ConstIterator,class Transform>
void BinaryParser<ConstIterator,Transform>::_postCheck() const
{
}


}
}
}


#endif // __MINERVA_POSTGIS_BINARY_PARSER_H__
