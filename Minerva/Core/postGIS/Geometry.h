
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

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IGeometryCenter.h"
#include "Usul/Interfaces/IOffset.h"

#include "osg/Array"
namespace osg { class Geometry; class Node; }

#include "pqxx/pqxx"

#include <vector>

#include "ossim/base/ossimGpt.h"

namespace Minerva {
namespace Core {
namespace postGIS {

class MINERVA_EXPORT Geometry : public Usul::Base::Referenced,
                                public Usul::Interfaces::IGeometryCenter,
                                public Usul::Interfaces::IOffset
{
public:
  typedef Usul::Base::Referenced        BaseClass;
  typedef Minerva::Core::DB::Connection Connection;

  USUL_DECLARE_QUERY_POINTERS( Geometry );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  Geometry();

  // Is it valid?
  bool                    valid() const;

  /// Get/Set the dirty flag
  void                    dirty ( bool b );
  bool                    dirty () const;

  /// Get/Set the srid.
  unsigned int            srid () const { return _srid; }
  void                    srid ( unsigned int srid ) { _srid = srid; }

  /// Set the database info.
  void                    databaseInfo ( Connection* connection, int id, const std::string& table );
protected:
  virtual ~Geometry();

  /// Usul::Interfaces::IGeometryCenter
  virtual osg::Vec3f geometryCenter ( unsigned int& srid );
  virtual osg::Vec3f geometryCenter ( const osg::Vec3f& offset, unsigned int& srid );

  /// Usul::Interfaces::IOffset
  virtual const osg::Vec3f&   spatialOffset () const;
  virtual void                spatialOffset ( const osg::Vec3f& );

  Minerva::Core::DB::Connection::RefPtr _connection;
  std::string _tableName;
  int _id;
  int _srid;
  osg::Vec3f _offset;
  bool _dirty;

};

}
}
}

#endif // __MINERVA_CORE_GEOMETRY_H__
