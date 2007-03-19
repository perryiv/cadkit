
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_DB_INFO_H__
#define __MINERVA_DB_INFO_H__

#include "Minerva/Core/Export.h"

#include "Minerva/Core/DB/Connection.h"

#include <vector>
#include <string>

namespace Minerva {
namespace Core {
namespace DB {

class MINERVA_EXPORT Info : public Usul::Base::Referenced
{
public:
  typedef Usul::Base::Referenced BaseClass;
  typedef std::vector < std::string > Strings;

  USUL_DECLARE_REF_POINTERS( Info );

  Info( Connection* connection );

  /// Get all public tables.
  Strings                         tables();

  /// Get all polygon tables.
  Strings                         polygonTables();

  /// Get all line tables.
  Strings                         lineTables();

  /// Get all point tables.
  Strings                         pointTables();

  /// Get all point time tables.
  Strings                         pointTimeTables();

  /// Get all geometry tables.
  Strings                         geometryTables();

  /// Check the type of given table.
  bool                            isPointTable   ( const std::string& table ) const;
  bool                            isLineTable    ( const std::string& table ) const;
  bool                            isPolygonTable ( const std::string& table ) const;

  /// Does the table have a column of the given type.
  bool                            hasColumnType ( const std::string& table, const std::string& type ) const;

  Strings                         getColumnNames ( const std::string& table ) const;

  void                            getMinMaxValue( const std::string& query, const std::string& columnName, double& min, double& max ) const;
protected:
  virtual ~Info();

  std::string                     _getGeometryType ( const std::string& table );
  Strings                         _fillStringsFromQuery( const std::string& query ) const;

private:
  Info();

  Minerva::Core::DB::Connection::RefPtr _connection;
};

}
}
}


#endif //__MINERVA_DB_INFO_H__
