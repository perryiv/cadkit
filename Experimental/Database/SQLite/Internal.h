
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
//  Internal helper functions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SQL_LITE_WRAP_INTERNAL_HELPER_FUNCTIONS_H_
#define _SQL_LITE_WRAP_INTERNAL_HELPER_FUNCTIONS_H_

#ifndef _COMPILING_SQL_LITE_WRAP
# error "This header file is only used for compiling the library."
# error "It should not be included in client code."
#endif

#ifdef _MSC_VER
# define NOMINMAX
# define WIN32_LEAN_AND_MEAN
#endif

#include "Usul/Strings/Case.h"
#include "Usul/Strings/Format.h"

#include "sqlite3.h"


namespace CadKit {
namespace Database {
namespace SQLite {
namespace Helper {


///////////////////////////////////////////////////////////////////////////////
//
//  Small helper class to guard an sqlite mutex.
//
///////////////////////////////////////////////////////////////////////////////

struct Guard
{
  Guard ( ::sqlite3 *db ) : _db ( db )
  {
    if ( 0x0 != _db )
    {
      ::sqlite3_mutex_enter ( ::sqlite3_db_mutex ( _db ) );
    }
  }
  ~Guard()
  {
    if ( 0x0 != _db )
    {
      ::sqlite3_mutex_leave ( ::sqlite3_db_mutex ( _db ) );
    }
  }
private:
  ::sqlite3 *_db;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to return the error string.
//
///////////////////////////////////////////////////////////////////////////////

inline std::string errorMessage ( ::sqlite3 *db )
{
  std::string error;
  if ( 0x0 != db )
  {
    error = sqlite3_errmsg ( db );
  }
  return error;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the string a select statement?
//
///////////////////////////////////////////////////////////////////////////////

inline bool isSelectStatement ( const std::string &sql )
{
  const std::string s ( Usul::Strings::lowerCase ( sql ) );
  if ( s.size() >= 6 )
  {
    return ( ( 's' == s[0] ) && 
             ( 'e' == s[1] ) && 
             ( 'l' == s[2] ) && 
             ( 'e' == s[3] ) && 
             ( 'c' == s[4] ) && 
             ( 't' == s[5] ) );
  }
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to get the value.
//
///////////////////////////////////////////////////////////////////////////////

template < class Function, class OutputType > 
inline void getValue ( ::sqlite3_stmt *statement, Function fun, OutputType &value, unsigned int &column )
{
  if ( 0x0 == statement )
    throw std::runtime_error ( "Error 5398252670: Null statement pointer" );

  value = fun ( statement, column++ );
}


} // namespace Helper
} // namespace SQLite
} // namespace Database
} // namespace CadKit


#endif // _SQL_LITE_WRAP_INTERNAL_HELPER_FUNCTIONS_H_
