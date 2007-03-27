
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
//  Results callback for SQLite database.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SQL_LITE_WRAP_RESULTS_CALLBACK_H_
#define _SQL_LITE_WRAP_RESULTS_CALLBACK_H_

#include "SQLiteWrap/Callback.h"

#include <string>
#include <vector>
#include <map>


namespace SQLiteWrap {


class SQL_LITE_WRAP_EXPORT Results : public Callback
{
public:

  /// Typedefs.
  typedef SQLiteWrap::Callback BaseClass;
  typedef std::vector<std::string> Column;
  typedef std::map<std::string,Column> Columns;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Results );

  // Constructor
  Results();

  // Clear internal memory.
  void                    clear();

  // Get the column. Have to externally lock the mutex when calling the first one.
  const Column &          column ( const std::string &name ) const;
  void                    column ( const std::string &name, Column & ) const;

  // Called by SQLlite during a select query.
  virtual void            operator () ( const Database &db, const std::string &name, const std::string &value );

  // Print the results.
  void                    print ( std::ostream & ) const;

  // Called right before a query.
  virtual void            start ( const Database &db );

protected:

  // Destructor
  virtual ~Results();

  void                    _clean();

private:

  // Can not copy or assign.
  Results ( const Results & );
  Results &operator = ( const Results & );

  // Data members.
  Columns _columns;
};


} // namespace SQLiteWrap


#endif // _SQL_LITE_WRAP_RESULTS_CALLBACK_H_
