
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Encapsulates all exposed SQLite database functionality.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SQL_LITE_WRAP_CONNECTION_H_
#define _SQL_LITE_WRAP_CONNECTION_H_

#include "Database/SQLite/Binder.h"
#include "Database/SQLite/Result.h"

#include "Usul/Base/Object.h"

#include <string>
#include <vector>

struct sqlite3;


namespace CadKit {
namespace Database {
namespace SQLite {


class SQL_LITE_WRAP_EXPORT Connection : public Usul::Base::Object
{
public:

  /// Typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef std::vector<Binder::RefPtr> Binders;

  // Type information.
  USUL_DECLARE_TYPE_ID ( Connection );

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Connection );

  // Constructor
  Connection ( const std::string &file );

  // Execute the sql string.
  Result::RefPtr          execute ( const std::string &sql );

  // Overloads of the execute function to make inserting blob and 
  // text data easier. Add more as needed.
  template < class T > 
  Result::RefPtr          execute ( const std::string &sql, const T & );
  template < class T1, class T2 > 
  Result::RefPtr          execute ( const std::string &sql, const T1 &, const T2 & );
  template < class T1, class T2, class T3 > 
  Result::RefPtr          execute ( const std::string &sql, const T1 &, const T2 &, const T3 & );
  template < class T1, class T2, class T3, class T4 > 
  Result::RefPtr          execute ( const std::string &sql, const T1 &, const T2 &, const T3 &, const T4 & );
  template < class T1, class T2, class T3, class T4, class T5 > 
  Result::RefPtr          execute ( const std::string &sql, const T1 &, const T2 &, const T3 &, const T4 &, const T5 & );

  // Get the file name.
  std::string             file() const;

protected:

  // Destructor
  virtual ~Connection();

  static int              _busyHandler ( void *, int );

  void                    _destroy();

  Result::RefPtr          _execute ( const std::string &sql, const Binders &binders = Binders() );

private:

  // Can not copy or assign.
  Connection ( const Connection & );
  Connection &operator = ( const Connection & );

  // Data members.
  std::string _file;
  ::sqlite3 *_db;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Execute the given SQL string.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > 
inline Result::RefPtr Connection::execute ( const std::string &sql, const T &t )
{
  Binders binders;
  binders.push_back ( CadKit::Database::SQLite::BinderTraits<T>::makeBinder ( t ) );
  return this->_execute ( sql, binders );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute the given SQL string.
//
///////////////////////////////////////////////////////////////////////////////

template < class T1, class T2 > 
inline Result::RefPtr Connection::execute ( const std::string &sql, const T1 &t1, const T2 &t2 )
{
  Binders binders;
  binders.push_back ( CadKit::Database::SQLite::BinderTraits<T1>::makeBinder ( t1 ) );
  binders.push_back ( CadKit::Database::SQLite::BinderTraits<T2>::makeBinder ( t2 ) );
  return this->_execute ( sql, binders );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute the given SQL string.
//
///////////////////////////////////////////////////////////////////////////////

template < class T1, class T2, class T3 > 
inline Result::RefPtr Connection::execute ( const std::string &sql, const T1 &t1, const T2 &t2, const T3 &t3 )
{
  Binders binders;
  binders.push_back ( CadKit::Database::SQLite::BinderTraits<T1>::makeBinder ( t1 ) );
  binders.push_back ( CadKit::Database::SQLite::BinderTraits<T2>::makeBinder ( t2 ) );
  binders.push_back ( CadKit::Database::SQLite::BinderTraits<T3>::makeBinder ( t3 ) );
  return this->_execute ( sql, binders );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute the given SQL string.
//
///////////////////////////////////////////////////////////////////////////////

template < class T1, class T2, class T3, class T4 > 
inline Result::RefPtr Connection::execute ( const std::string &sql, const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4 )
{
  Binders binders;
  binders.push_back ( CadKit::Database::SQLite::BinderTraits<T1>::makeBinder ( t1 ) );
  binders.push_back ( CadKit::Database::SQLite::BinderTraits<T2>::makeBinder ( t2 ) );
  binders.push_back ( CadKit::Database::SQLite::BinderTraits<T3>::makeBinder ( t3 ) );
  binders.push_back ( CadKit::Database::SQLite::BinderTraits<T4>::makeBinder ( t4 ) );
  return this->_execute ( sql, binders );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute the given SQL string.
//
///////////////////////////////////////////////////////////////////////////////

template < class T1, class T2, class T3, class T4, class T5 > 
inline Result::RefPtr Connection::execute ( const std::string &sql, const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5 )
{
  Binders binders;
  binders.push_back ( CadKit::Database::SQLite::BinderTraits<T1>::makeBinder ( t1 ) );
  binders.push_back ( CadKit::Database::SQLite::BinderTraits<T2>::makeBinder ( t2 ) );
  binders.push_back ( CadKit::Database::SQLite::BinderTraits<T3>::makeBinder ( t3 ) );
  binders.push_back ( CadKit::Database::SQLite::BinderTraits<T4>::makeBinder ( t4 ) );
  binders.push_back ( CadKit::Database::SQLite::BinderTraits<T5>::makeBinder ( t5 ) );
  return this->_execute ( sql, binders );
}


} // namespace SQLite
} // namespace Database
} // namespace CadKit


#endif // _SQL_LITE_WRAP_CONNECTION_H_
