
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/PostGIS/BinaryParser.h"

#include "Minerva/Core/Data/Point.h"
#include "Minerva/Core/Data/Line.h"
#include "Minerva/Core/Data/Polygon.h"

#include "Usul/Endian/Endian.h"

#include <algorithm>
#include <functional>

using namespace Minerva::Layers::PostGIS;


///////////////////////////////////////////////////////////////////////////////
//
//  Typedefs.
//
///////////////////////////////////////////////////////////////////////////////

typedef Minerva::Core::Data::Point Point;
typedef Minerva::Core::Data::Line Line;
typedef Minerva::Core::Data::Polygon Polygon;

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

BinaryParser::BinaryParser()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

BinaryParser::~BinaryParser()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Predicate for comparing vertices.  Should probably use CloseFloat for robustness.
//
///////////////////////////////////////////////////////////////////////////////

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


///////////////////////////////////////////////////////////////////////////////
//
//  Get data and convert.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < typename Type, class Convert >
  Type convert ( const unsigned char *& buffer )
  {
    Type t;
    const unsigned int size ( sizeof ( t ) );
    ::memcpy( &t, buffer, size );
    buffer += size;

		Convert::convert( t );
    return t;
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Create vertices from binary string.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{

  template < class Convert, class Vertices >
  void createVertices( const unsigned char*& buffer, Vertices& vertices )
  {
    typedef typename Vertices::value_type Vertex;

    Usul::Types::Uint32 numPoints ( Detail::convert < Usul::Types::Uint32, Convert > ( buffer ) );
    vertices.reserve( numPoints );

	  for ( Usul::Types::Uint32 i = 0; i < numPoints; ++i )
	  {
      Usul::Types::Float64 x ( convert < Usul::Types::Float64, Convert > ( buffer ) );
      Usul::Types::Float64 y ( convert < Usul::Types::Float64, Convert > ( buffer ) );
      
      vertices.push_back( Vertex ( x, y ) );
	  }

    // Remove any duplicate vertices.
    vertices.erase ( std::unique ( vertices.begin(), vertices.end(), Detail::VectorCompare() ), vertices.end() );
  }

  template < class Convert, class Vertices >
  void createVertices3D( const unsigned char*& buffer, Vertices& vertices )
  {
    typedef typename Vertices::value_type Vertex;

    Usul::Types::Uint32 numPoints ( Detail::convert < Usul::Types::Uint32, Convert > ( buffer ) );
    vertices.reserve( numPoints );

	  for ( Usul::Types::Uint32 i = 0; i < numPoints; ++i )
	  {
      Usul::Types::Float64 x ( convert < Usul::Types::Float64, Convert > ( buffer ) );
      Usul::Types::Float64 y ( convert < Usul::Types::Float64, Convert > ( buffer ) );
      
      vertices.push_back( Vertex ( x, y, 0.0 ) );
	  }

    // Remove any duplicate vertices.
    vertices.erase ( std::unique ( vertices.begin(), vertices.end(), Detail::VectorCompare() ), vertices.end() );
  }

}


namespace Detail
{
  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Create a point.
  //
  ///////////////////////////////////////////////////////////////////////////////

  template < class Convert >
  Point* createPoint ( const unsigned char *& buffer )
  {
    Point::RefPtr point ( new Point );

    Usul::Types::Float64 x ( convert < Usul::Types::Float64, Convert > ( buffer ) );
    Usul::Types::Float64 y ( convert < Usul::Types::Float64, Convert > ( buffer ) );

    point->point ( Usul::Math::Vec3d ( x, y, 0.0 ) );

    return point.release();
  }


  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Create a line.
  //
  ///////////////////////////////////////////////////////////////////////////////

  template < class Convert >
  Line* createLine ( const unsigned char *& buffer )
  {
    Line::RefPtr line ( new Line );

    typedef Line::Vertices Vertices;

    Vertices vertices;
	  Detail::createVertices3D < Convert, Vertices > ( buffer, vertices );

    line->line( vertices );

    return line.release();
  }


  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Create a polygon.
  //
  ///////////////////////////////////////////////////////////////////////////////

  template < class Convert >
  Polygon* createPolygon ( const unsigned char *& buffer )
  {
    Polygon::RefPtr polygon ( new Polygon );

    Usul::Types::Uint32 numRings ( Detail::convert < Usul::Types::Uint32, Convert > ( buffer ) );

    typedef Line::Vertices Vertices;
    Vertices vertices;

    for ( Usul::Types::Uint32 i = 0; i < numRings; ++i )
    {
      Detail::createVertices3D < Convert, Vertices > ( buffer, vertices );
    }

    polygon->line( vertices );

    return polygon.release();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse and create geomtry with given endian converter.
//
///////////////////////////////////////////////////////////////////////////////

template < class Convert >
void BinaryParser::_createGeometryEndian ( const unsigned char*& buffer, Geometries &geometries )
{
  

  Usul::Types::Uint32 polygonType ( Detail::convert < Usul::Types::Uint32, Convert > ( buffer ) );

	switch ( polygonType )
	{
	case wkbPoint:       geometries.push_back ( Detail::createPoint   < Convert > ( buffer ) ); break;
  case wkbLineString:  geometries.push_back ( Detail::createLine    < Convert > ( buffer ) ); break;
  case wkbPolygon:     geometries.push_back ( Detail::createPolygon < Convert > ( buffer ) ); break;
	case wkbMultiPoint:
		{
      // How many points will we have?
      Usul::Types::Uint32 numPoints ( Detail::convert < Usul::Types::Uint32, Convert > ( buffer ) );

      // Loop through and add the points.
      for( Usul::Types::Uint32 i = 0; i < numPoints; ++i )
			{
        this->_createGeometry( buffer, geometries );
      }
		}
		break;
	case wkbMultiLineString:
		{
      // Get the number of lines to expect.
      Usul::Types::Uint32 numLines ( Detail::convert < Usul::Types::Uint32, Convert > ( buffer ) );

			for( Usul::Types::Uint32 i = 0; i < numLines; ++i )
			{
        this->_createGeometry ( buffer, geometries );
      }
		}
		break;
	case wkbMultiPolygon:
		{
      // Get the number of polygons to expect.
      Usul::Types::Uint32 numPolygons ( Detail::convert < Usul::Types::Uint32, Convert > ( buffer ) );

			for( Usul::Types::Uint32 i = 0; i < numPolygons; ++i )
			{
        this->_createGeometry ( buffer, geometries );
			}
		}
		break;
	case wkbGeometryCollection:
		{
      this->_createGeometry ( buffer, geometries );
		}
		break;
  default:
    throw std::runtime_error ("Error 4919319700: Unknown geometry type." );
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse and create geomtry.
//
///////////////////////////////////////////////////////////////////////////////

void BinaryParser::_createGeometry ( const unsigned char*& buffer, Geometries &geometries )
{
  Usul::Types::Uint8 endian ( 0 );
	::memcpy( &endian, buffer, sizeof( endian ) );
  buffer += sizeof(endian);

  switch ( endian )
  {
	// Big endian.
  case wkbBigEndian:
		return _createGeometryEndian < Usul::Endian::FromBigToSystem > ( buffer, geometries );
	// Little endian.
  case wkbLittleEndian:
		return _createGeometryEndian < Usul::Endian::FromLittleToSystem > ( buffer, geometries );    
	}

  throw std::runtime_error ( "Error 1713426630: Unknown endian type." );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Parse and create geomtry.
//
///////////////////////////////////////////////////////////////////////////////

BinaryParser::Geometries BinaryParser::operator() ( const unsigned char* buffer )
{
  Geometries geometries;
  this->_createGeometry( buffer, geometries );
  return geometries;
}
