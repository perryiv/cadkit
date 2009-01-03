
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

#include "Database/SQLite/Result.h"

#include "Usul/Base/Object.h"

#include <string>

struct sqlite3;


namespace CadKit {
namespace Database {
namespace SQLite {


class SQL_LITE_WRAP_EXPORT Connection : public Usul::Base::Object
{
public:

  /// Typedefs.
  typedef Usul::Base::Object BaseClass;

  // Type information.
  USUL_DECLARE_TYPE_ID ( Connection );

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Connection );

  // Constructor
  Connection ( const std::string &file );

  // Execute the sql string.
  Result::RefPtr          execute ( const std::string & );

  // Get the file name.
  std::string             file() const;

protected:

  // Destructor
  virtual ~Connection();

  void                    _destroy();

private:

  // Can not copy or assign.
  Connection ( const Connection & );
  Connection &operator = ( const Connection & );

  // Data members.
  std::string _file;
  ::sqlite3 *_db;
};


} // namespace SQLite
} // namespace Database
} // namespace CadKit


#endif // _SQL_LITE_WRAP_DATABASE_H_
