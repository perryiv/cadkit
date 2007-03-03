
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_POSTGIS_BINARY_PARSER_H__
#define __MINERVA_POSTGIS_BINARY_PARSER_H__

#include "Minerva/Core/Export.h"

#include "Usul/Math/Vector2.h"
#include "Usul/Types/Types.h"
#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include "pqxx/pqxx"

#include <vector>

namespace Minerva {
namespace Core {
namespace postGIS {

class MINERVA_EXPORT BinaryParser : public Usul::Base::Referenced
{
public:
  typedef Usul::Base::Referenced BaseClass;

  USUL_DECLARE_REF_POINTERS( BinaryParser );

  BinaryParser( const pqxx::result::field &F );

  typedef Usul::Math::Vec2d                Vertex;
  typedef std::vector < Vertex >           Vertices;
  typedef std::vector < Vertices >         VertexList;

  enum wkbGeometryType 
  {
    wkbPoint = 1,
    wkbLineString = 2,
    wkbPolygon = 3,
    wkbMultiPoint = 4,
    wkbMultiLineString = 5,
    wkbMultiPolygon = 6,
    wkbGeometryCollection = 7
  };

  VertexList              getVertices();

protected:
  virtual ~BinaryParser();

  template < class Convert >
  void   _createVertices( Usul::Types::Uint32 numPoints, Usul::Types::Uint64& bytesReadSoFar, Vertices& vertices );

  template < class Convert >
	void   _parseLineString( Usul::Types::Uint64& bytesReadSoFar, Vertices& vertices );

  template < class Convert >
	void _createGeometryEndian ( VertexList &vertexList, Usul::Types::Uint64& bytesReadSoFar );

  void _createGeometry( VertexList &vertexList, Usul::Types::Uint64& bytesReadSoFar );

private:
  pqxx::binarystring *_buffer;
};

}
}
}


#endif // __MINERVA_POSTGIS_BINARY_PARSER_H__
