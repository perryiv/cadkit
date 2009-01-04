
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
//  Result for SQLite database.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SQL_LITE_WRAP_RESULTS_CALLBACK_H_
#define _SQL_LITE_WRAP_RESULTS_CALLBACK_H_

#include "Database/SQLite/Export.h"

#include "Usul/Base/Object.h"
#include "Usul/Types/Types.h"

#include <string>

struct sqlite3;
struct sqlite3_stmt;


namespace CadKit {
namespace Database {
namespace SQLite {


class SQL_LITE_WRAP_EXPORT Result : public Usul::Base::Object
{
public:

  /// Typedefs.
  typedef Usul::Base::Object BaseClass;

  // Type information.
  USUL_DECLARE_TYPE_ID ( Result );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Result );

  // Return the column name or empty string if index is out of range.
  // Valid range is [0,numColumns()-1].
  std::string             columnName ( unsigned int index ) const;

  // Prepare the next row for data retrieval. 
  // Returns false if there are no more rows.
  bool                    prepareNextRow();

  // Get the number of columns.
  unsigned int            numColumns() const;

  // Input operator for copying the results.
  Result &                operator >> ( std::string & );
  Result &                operator >> ( double & );
  Result &                operator >> ( float & );
  Result &                operator >> ( int & );
  Result &                operator >> ( unsigned int & );
  Result &                operator >> ( Usul::Types::Int64 & );
  Result &                operator >> ( Usul::Types::Uint64 & );

protected:

  friend class Connection;

  // Constructor
  Result ( ::sqlite3 *, ::sqlite3_stmt * );

  // Use reference counting.
  virtual ~Result();

private:

  // No default construction.
  Result();

  // Can not copy or assign.
  Result ( const Result & );
  Result &operator = ( const Result & );

  void                    _destroy();

  ::sqlite3 *_database;
  ::sqlite3_stmt *_statement;
  unsigned int _currentColumn;
};


} // namespace SQLite
} // namespace Database
} // namespace CadKit


#endif // _SQL_LITE_WRAP_RESULTS_CALLBACK_H_
