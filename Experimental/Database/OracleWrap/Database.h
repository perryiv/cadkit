
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
//  Encapsulates all database functionality.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _ORACLE_LITE_WRAP_DATABASE_H_
#define _ORACLE_LITE_WRAP_DATABASE_H_

#include "OracleWrap/Result.h"

#include "Usul/Base/Object.h"
#include "Usul/Pointers/Pointers.h"

#include <string>

struct OCI_Connection;
struct OCI_Statement;


namespace OracleWrap {


class ORACLE_WRAP_EXPORT Database : public Usul::Base::Object
{
public:

  // Typedefs.
  typedef Usul::Base::Object BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Database );

  // Constructor
  Database ( const std::string &database, const std::string &user, const std::string &password );

  // Execute the sql string and return the result, which may be empty.
  Result::RefPtr          execute ( const std::string & );

protected:

  // Destructor
  virtual ~Database();

private:

  // Can not copy or assign.
  Database ( const Database & );
  Database &operator = ( const Database & );

  void                    _destroy();

  OCI_Connection *_connection;
  OCI_Statement *_statement;
};


} // namespace OracleWrap


#endif // _ORACLE_LITE_WRAP_DATABASE_H_
