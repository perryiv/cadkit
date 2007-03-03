
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_GEOMETRY_H__
#define __MINERVA_CORE_GEOMETRY_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/DB/Connection.h"
#include "Minerva/Core/postGIS/BinaryParser.h"

#include "Usul/Base/Referenced.h"

#include "osg/Array"
namespace osg { class Geometry; class Node; }

#include "pqxx/pqxx"

#include "ossim/base/ossimRefPtr.h"
#include "ossim/projection/ossimProjection.h"

#include <vector>

namespace Minerva {
namespace Core {
namespace postGIS {

class MINERVA_EXPORT Geometry : public Usul::Base::Referenced
{
public:
  typedef Usul::Base::Referenced    BaseClass;
  typedef BinaryParser::Vertices    Vertices;
  typedef BinaryParser::VertexList  VertexList;

  USUL_DECLARE_REF_POINTERS( Geometry );

  Geometry( Minerva::Core::DB::Connection *connection, const std::string &tableName, int id, int srid, const pqxx::result::field &F );
  Geometry( Minerva::Core::DB::Connection *connection, const std::string &tableName, int id );
  Geometry( Minerva::Core::DB::Connection *connection, const pqxx::result::field &F );

  /// TODO: These should probably be in interfaces.
  virtual osg::Node*      buildScene() = 0;
  virtual osg::Geometry*  buildGeometry() = 0;

  // Get center of geometry.
  virtual osg::Vec3       getCenter ( );
  virtual osg::Vec3       getCenter ( float xOffset, float yOffset, float zOffset );

  /// Get/Set the x offset.
  void                    xOffset( float f );
  float                   xOffset( ) const;

  /// Get/Set the y offset.
  void                    yOffset( float f );
  float                   yOffset( ) const;

  /// Get/Set the z offset.
  void                    zOffset( float f );
  float                   zOffset( ) const;

  // Is it valid?
  bool                    valid() const;

  /// Get/Set the dirty flag
  void                    dirty ( bool b );
  bool                    dirty () const;

protected:
  virtual ~Geometry();

  void _convertToLatLong ( const Vertices& vertices, std::vector< ossimGpt >& latLongPoints );
  bool _isSridSphereical( int id );

  typedef std::pair< std::string, int > CacheKey;
  typedef std::map< CacheKey, VertexList > VerticesCache;
  typedef std::map < CacheKey, osg::ref_ptr< osg::Node > > TriangleCache;

  static TriangleCache _triangleCache;
  static VerticesCache _verticesCache;

  Minerva::Core::DB::Connection::RefPtr _connection;
  Minerva::Core::postGIS::BinaryParser::RefPtr _parser;
  std::string _tableName;
  int _id;
  int _srid;
  ossimRefPtr < ossimProjection > _projection;
  float _xOffset;
  float _yOffset;
  float _zOffset;
  bool _dirty;
};

}
}
}

#endif // __MINERVA_CORE_GEOMETRY_H__
