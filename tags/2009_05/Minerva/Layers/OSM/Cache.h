
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_OPEN_STREET_MAP_CACHE_H__
#define __MINERVA_LAYERS_OPEN_STREET_MAP_CACHE_H__

#include "Minerva/Core/Extents.h"

#include "Minerva/Layers/OSM/Export.h"
#include "Minerva/Layers/OSM/Common.h"
#include "Minerva/Layers/OSM/Node.h"
#include "Minerva/Layers/OSM/Way.h"
#include "Minerva/Layers/OSM/LineString.h"

#include "Database/SQLite/Connection.h"

#include "Usul/Base/Object.h"

namespace Minerva {
namespace Layers {
namespace OSM {


class MINERVA_OSM_EXPORT Cache : public Usul::Base::Object
{
public:

  typedef Usul::Base::Object BaseClass;
  typedef Minerva::Layers::OSM::Object OSMObject;
  typedef std::vector<LineString::RefPtr> Lines;

  USUL_DECLARE_REF_POINTERS ( Cache );

  Cache ( CadKit::Database::SQLite::Connection *connection );

  void initialize();
  bool isInitialized();

  void addNodeData ( const std::string& key, const Extents& extents, const Nodes& nodes );
  void getNodeData ( const std::string& key, const Extents& extents, Nodes& nodes ) const;
  bool hasNodeData ( const std::string& key, const Extents& extents ) const;

  void addLineData ( const std::string& key, const Extents& extents, const Lines& line );
  void getLineData ( const std::string& key, const Extents& extents, Lines& lines ) const;
  bool hasLineData ( const std::string& key, const Extents& extents ) const;

protected:

  virtual ~Cache();

  void _addGeometryColumn ( const std::string& table, const std::string& column, const std::string& type );
  void _addSpatialIndex ( const std::string& table, const std::string& column );

  void _addNodeData ( const std::string& key, const Extents& extents, const Nodes& nodes );
  Node* _createNode ( CadKit::Database::SQLite::Result& result ) const;

  int _addLineEntry ( const std::string& key, const Extents& extents );

  void _createTagsTable ( const std::string& tableName );
  void _addTags ( const std::string& tableName, OSMObject::IdType id, const OSMObject::Tags& tags );
  void _getTags ( const std::string& tableName, OSMObject::IdType id, OSMObject::Tags& tags ) const;

  static std::string _createLineText ( const LineString::Vertices& vertices );
  static std::string _createPointText ( const Node::Location& location );
  static std::string _createIndexQuery ( const std::string& tableName, const std::string& columnName, const Extents& extents );

  static void _translate ( Node::Location& location );
  static void _unTranslate ( Node::Location& location );

private:

  Cache();
  Cache ( const Cache& rhs );
  Cache& operator= ( const Cache& rhs );

  mutable CadKit::Database::SQLite::Connection::RefPtr _connection;

};


}
}
}


#endif // __MINERVA_LAYERS_OPEN_STREET_MAP_CACHE_H__
