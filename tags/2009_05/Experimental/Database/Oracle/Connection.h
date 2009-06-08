
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Encapsulates all database connectivity.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _ORACLE_LITE_WRAP_CONNECTION_H_
#define _ORACLE_LITE_WRAP_CONNECTION_H_

#include "Database/Oracle/Export.h"

#include "Usul/Base/Object.h"
#include "Usul/Pointers/Pointers.h"

#include <string>

namespace oracle { namespace occi { class Connection; class Statement; } }

namespace CadKit { namespace Database { namespace Oracle { class Result; } } }


namespace CadKit {
namespace Database {
namespace Oracle {


class ORACLE_WRAP_EXPORT Connection : public Usul::Base::Object
{
public:

  // Typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef CadKit::Database::Oracle::Result Result;

  // Type information.
  USUL_DECLARE_TYPE_ID ( Connection );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Connection );

  // Constructor
  Connection ( const std::string &database, const std::string &user, const std::string &password );

  // Cancel a running statement. This has no effect if there is no running 
  // statement. This is meant to be safe to call from any thread.
  void                    cancelRunningStatement();

  // Commit changes made since previous commit.
  void                    commit();

  // Execute the sql string and return the result, which may be empty.
  // Can also specify the number of row to fetch at a time.
  Result *                execute ( const std::string &, unsigned int numRowsToPreFetch = 0 );

  // Cancel any running queries for this connection on the server.
  void                    sendCancelRequestToServer();

  // Roll back any changes made since previous commit.
  void                    rollback();

protected:

  friend class Result;

  // Destructor
  virtual ~Connection();

  Result *                _execute ( const std::string &, unsigned int numRowsToPreFetch );
  Result *                _execute ( oracle::occi::Connection *, oracle::occi::Statement *, bool );

  void                    _setExecutingStatement ( oracle::occi::Statement *s );

  void                    _terminateStatement ( oracle::occi::Statement *s );

private:

  // Can not copy or assign.
  Connection ( const Connection & );
  Connection &operator = ( const Connection & );

  void                    _destroy();

  oracle::occi::Connection *_connection;
  oracle::occi::Statement *_executing;
  unsigned int _numPreFetchRows;
};


} // namespace Oracle
} // namespace Database
} // namespace CadKit


#endif // _ORACLE_LITE_WRAP_CONNECTION_H_
