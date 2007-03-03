
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/postGIS/BinaryParser.h"

#include "Usul/Endian/Endian.h"

#include <algorithm>


namespace Detail
{
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
}


using namespace Minerva::Core::postGIS;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

BinaryParser::BinaryParser( const pqxx::result::field &F ) : 
BaseClass(),
_buffer ( new pqxx::binarystring( F ) )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

BinaryParser::~BinaryParser()
{
  delete _buffer;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create vertices from binary string.
//
///////////////////////////////////////////////////////////////////////////////

template < class Convert >
void BinaryParser::_createVertices( Usul::Types::Uint32 numPoints, Usul::Types::Uint64& bytesReadSoFar, Vertices& vertices )
{
  vertices.reserve( numPoints );

	Usul::Types::Uint32 sizeOfFloat ( sizeof ( Usul::Types::Float64 ) );

	for ( Usul::Types::Uint32 i = 0; i < numPoints; ++i )
	{
		Usul::Types::Float64 x ( 0 );
		::memcpy( &x, &(_buffer->front()) + bytesReadSoFar, sizeOfFloat );
		bytesReadSoFar += sizeOfFloat;

		Convert::convert(x);

		Usul::Types::Float64 y ( 0 );
		::memcpy( &y, &(_buffer->front()) + bytesReadSoFar, sizeOfFloat );
		bytesReadSoFar += sizeOfFloat;

		Convert::convert(y);  
    
    vertices.push_back( Vertex ( x, y ) );
	}

  // Remove any duplicate vertices.
  vertices.erase ( std::unique ( vertices.begin(), vertices.end(), Detail::VectorCompare() ), vertices.end() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the line string.
//
///////////////////////////////////////////////////////////////////////////////

template < class Convert >
void BinaryParser::_parseLineString( Usul::Types::Uint64& bytesReadSoFar, Vertices& vertices )
{
	Usul::Types::Uint32 numPoints ( 0 );
	::memcpy( &numPoints, &(_buffer->front()) + bytesReadSoFar, sizeof( numPoints ) );
	bytesReadSoFar += sizeof(numPoints);

	Convert::convert(numPoints);

	return _createVertices< Convert > ( numPoints, bytesReadSoFar, vertices );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the geometry.
//
///////////////////////////////////////////////////////////////////////////////

template < class Convert >
void BinaryParser::_createGeometryEndian ( VertexList &vertexList, Usul::Types::Uint64& bytesReadSoFar )
{
	Usul::Types::Uint32 polygonType ( 0 );
	::memcpy( &polygonType, &(_buffer->front()) + bytesReadSoFar, sizeof( polygonType ) );
	bytesReadSoFar += sizeof(polygonType);

	Convert::convert(polygonType);

	switch ( polygonType )
	{
	case wkbPoint:
		{
      Vertices vertices;
			_createVertices < Usul::Endian::FromLittleToSystem > ( 1, bytesReadSoFar, vertices );
			vertexList.push_back ( vertices );
		}
		break;
	case wkbLineString:
		{
      Vertices vertices;
			_parseLineString < Usul::Endian::FromLittleToSystem > ( bytesReadSoFar, vertices );
			vertexList.push_back ( vertices );
		}
		break;
	case wkbPolygon:
		{
			Usul::Types::Uint32 numRings ( 0 );
			::memcpy( &numRings, &(_buffer->front()) + bytesReadSoFar, sizeof( Usul::Types::Uint32 ) );
			bytesReadSoFar += sizeof(numRings);
			Convert::convert(numRings);

      for ( Usul::Types::Uint32 i = 0; i < numRings; ++i )
      {
         Vertices vertices;
        _parseLineString < Usul::Endian::FromLittleToSystem > ( bytesReadSoFar, vertices );
			  vertexList.push_back ( vertices );
      }
		}
		break;
	case wkbMultiPoint:
		{
			Usul::Types::Uint32 numPoints ( 0 );
			::memcpy( &numPoints, &(_buffer->front()) + bytesReadSoFar, sizeof( numPoints ) );
			bytesReadSoFar += sizeof(numPoints);
			Convert::convert(numPoints);
		}
		break;
	case wkbMultiLineString:
		{
			Usul::Types::Uint32 numLineStrings ( 0 );
			::memcpy( &numLineStrings, &(_buffer->front()) + bytesReadSoFar, sizeof( numLineStrings ) );
			bytesReadSoFar += sizeof(numLineStrings);
			Convert::convert(numLineStrings);

			for( Usul::Types::Uint32 i = 0; i < numLineStrings; ++i )
			{
        this->_createGeometry( vertexList, bytesReadSoFar );
      }
		}
		break;
	case wkbMultiPolygon:
		{
			Usul::Types::Uint32 numPolygons ( 0 );
			::memcpy( &numPolygons, &(_buffer->front()) + bytesReadSoFar, sizeof( numPolygons ) );
			bytesReadSoFar += sizeof(numPolygons);
			Convert::convert(numPolygons);

			for( Usul::Types::Uint32 i = 0; i < numPolygons; ++i )
			{
		    this->_createGeometry( vertexList, bytesReadSoFar );
			}
		}
		break;
	case wkbGeometryCollection:
		{
			Usul::Types::Uint32 numGeometries ( 0 );
			::memcpy( &numGeometries, &(_buffer->front()) + bytesReadSoFar, sizeof( numGeometries ) );
			bytesReadSoFar += sizeof(numGeometries);
			Convert::convert(numGeometries);
		}
		break;
	default: // Unknown
		throw std::runtime_error ("Error 1293201202: Unknown geometry type." );
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the geometry.
//
///////////////////////////////////////////////////////////////////////////////

void BinaryParser::_createGeometry( VertexList &vertexList, Usul::Types::Uint64& bytesReadSoFar )
{
	Usul::Types::Uint8 endian ( 0 );
	::memcpy( &endian, &(_buffer->front()), sizeof( endian ) );
	bytesReadSoFar += sizeof(endian);

	// Big endian.
	if( endian == 0 )
	{
		_createGeometryEndian < Usul::Endian::FromBigToSystem > ( vertexList, bytesReadSoFar );
	}
	// Little endian.
	else if ( endian == 1 )
	{
		_createGeometryEndian < Usul::Endian::FromLittleToSystem > ( vertexList, bytesReadSoFar );
	}
	// Error!
	else
	{
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get vertex data for the given table and id.
//
///////////////////////////////////////////////////////////////////////////////

BinaryParser::VertexList BinaryParser::getVertices ()
{
  Usul::Types::Uint64 bytesReadSoFar ( 0 );

  VertexList vertexList;
  this->_createGeometry( vertexList, bytesReadSoFar );
 
  return vertexList;
}
