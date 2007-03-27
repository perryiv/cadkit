
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
//  Encapsulates all exposed SQLite database functionality.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SQL_LITE_WRAP_DATABASE_H_
#define _SQL_LITE_WRAP_DATABASE_H_

#include "SQLiteWrap/Callback.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/Guard.h"

#include <string>
#include <sstream>

typedef struct sqlite3 sqlite3;


namespace SQLiteWrap {


class SQL_LITE_WRAP_EXPORT Database : public Usul::Base::Referenced
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Threads::Mutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Database );

  // Constructor
  Database ( const std::string &file );

  // Set the callback.
  void                    callback ( Callback *cb );

  // Execute the sql string.
  void                    execute ( const std::string & );
  void                    execute();

  // For use with the output stream operator.
  static Database &       flush ( Database & );

  // Get the file name.
  std::string             file() const;

  // Get the mutex.
  Mutex &                 mutex() const;

  // Get/set the sql string.
  std::string             sql() const;
  void                    sql ( const std::string & );

protected:

  // Destructor
  virtual ~Database();

  static int              _callback ( void *user, int argc, char **argv, char **columnName );
  int                     _callback ( int argc, char **argv, char **columnName );
  void                    _clean();
  void                    _close();

  void                    _finish();

  void                    _start();

private:

  // Can not copy or assign.
  Database ( const Database & );
  Database &operator = ( const Database & );

  // Data members.
  std::string _file;
  ::sqlite3 *_db;
  mutable Mutex *_mutex;
  std::string _sql;
  Callback::RefPtr _cb;
};


} // namespace SQLiteWrap


///////////////////////////////////////////////////////////////////////////////
//
//  Output operator.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > SQLiteWrap::Database &operator << ( SQLiteWrap::Database &db, const T &t )
{
  std::ostringstream sql;
  sql << db.sql() << t;
  db.sql ( sql.str() );
  return db;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Output operator.
//
///////////////////////////////////////////////////////////////////////////////

template < class PointerConfig, class T > SQLiteWrap::Database &operator << ( Usul::Pointers::SmartPointer<SQLiteWrap::Database,PointerConfig> &ptr, const T &t )
{
  SQLiteWrap::Database &db ( *(ptr.get()) );
  db << t;
  return db;
}


#endif // _SQL_LITE_WRAP_DATABASE_H_
