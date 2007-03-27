
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_DB_CONNECTION_POOL_H__
#define __MINERVA_CORE_DB_CONNECTION_POOL_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/DB/Connection.h"

#include <string>
#include <map>

namespace Minerva {
namespace Core {
namespace DB {

class MINERVA_EXPORT ConnectionPool
{
public:
  static ConnectionPool& instance();

  bool                             hasConnection ( const std::string& name ) const;
  Minerva::Core::DB::Connection*   getConnection ( const std::string& name );
  void                             addConnection ( Minerva::Core::DB::Connection* connection );

protected:
  ConnectionPool();
  ~ConnectionPool();

private:
  typedef std::map< std::string, Minerva::Core::DB::Connection::RefPtr > Connections;

  Connections _connections;

  static ConnectionPool *_instance;
};

}
}
}

#endif // __MINERVA_CORE_DB_CONNECTION_POOL_H__
