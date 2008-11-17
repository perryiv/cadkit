
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
//  Encapsulates the result of a query.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _ORACLE_LITE_WRAP_RESULT_H_
#define _ORACLE_LITE_WRAP_RESULT_H_

#include "OracleWrap/Export.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"

#include <string>

namespace oracle { namespace occi { class Connection; class ResultSet; class Statement; } }

namespace OracleWrap { class Database; }


namespace OracleWrap {


class ORACLE_WRAP_EXPORT Result : public Usul::Base::Referenced
{
public:

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Threads::RecursiveMutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Result );

  // Prepare the next row for data retrieval. 
  // Returns false if there are no more rows.
  bool                    prepareNextRow();

  // Get the number of columns.
  unsigned int            numColumns() const;

  // Input operator for copying the results.
  Result &                operator >> ( std::string & );
  Result &                operator >> ( double & );
  Result &                operator >> ( unsigned int & );

protected:

  friend class Database;

  // Constructor
  Result ( oracle::occi::Connection *, oracle::occi::Statement *, oracle::occi::ResultSet *, Mutex & );

  // Destructor
  virtual ~Result();

private:

  // No default construction.
  Result();

  // Can not copy or assign.
  Result ( const Result & );
  Result &operator = ( const Result & );

  void                    _destroy();

  oracle::occi::Connection *_connection;
  oracle::occi::Statement *_statement;
  oracle::occi::ResultSet *_result;
  Mutex &_mutex;
  unsigned int _currentColumn;
};


} // namespace OracleWrap


#endif // _ORACLE_LITE_WRAP_RESULT_H_
