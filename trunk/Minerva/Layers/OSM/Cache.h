
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

  USUL_DECLARE_REF_POINTERS ( Cache );

  Cache ( CadKit::Database::SQLite::Connection *connection );

  void addNodeData ( const std::string& key, const Extents& extents, const Nodes& nodes );
  void getNodeData ( const std::string& key, const Extents& extents, Nodes& nodes ) const;
  bool hasNodeData ( const std::string& key, const Extents& extents ) const;

  void addWayData ( const std::string& key, const Extents& extents, const Ways& ways );
  void getWayData ( const std::string& key, const Extents& extents, Ways& ways ) const;
  bool hasWayData ( const std::string& key, const Extents& extents ) const;

protected:

  virtual ~Cache();

  void _addNodeData ( const std::string& key, const Extents& extents, const Nodes& nodes );
  Node* _createNode ( CadKit::Database::SQLite::Result& result ) const;

  void _addWay ( const std::string& key, const Extents& extents, const Way& way );
  Way* _createWay ( Way::IdType id ) const;

  void _createTagsTable ( const std::string& tableName );
  void _addTags ( const std::string& tableName, OSMObject::IdType id, const OSMObject::Tags& tags );
  void _getTags ( const std::string& tableName, OSMObject::IdType id, OSMObject::Tags& tags ) const;

  static std::string _createPointText ( const Node::Location& location );
  static std::string _createMBRText ( const Extents& extents );

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
