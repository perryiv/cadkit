
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

#include "Minerva/Core/Geometry/Geometry.h"

#include "Usul/Math/Vector2.h"
#include "Usul/Types/Types.h"
#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include <vector>

namespace Minerva {
namespace Layers {
namespace PostGIS {

class MINERVA_POSTGIS_EXPORT BinaryParser
{
public:
  BinaryParser();
  ~BinaryParser();

  typedef Minerva::Core::Geometry::Geometry Geometry;
  typedef std::vector < Geometry::RefPtr > Geometries;

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

  enum wkbEndianType
  {
    wkbBigEndian = 0,
    wkbLittleEndian = 1
  };

  Geometries              operator() ( const unsigned char* buffer );

protected:

  template < class Convert >
	void _createGeometryEndian ( const unsigned char*& buffer, Geometries& );
  void _createGeometry       ( const unsigned char*& buffer, Geometries& );

};

}
}
}


#endif // __MINERVA_POSTGIS_BINARY_PARSER_H__
