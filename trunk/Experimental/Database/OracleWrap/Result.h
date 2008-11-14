
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

namespace OracleWrap { class Database; }

struct OCI_Resultset;
struct OCI_Statement;


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

  // Does the result have more rows?
  bool                    hasMoreRows() const;

  // Get the number of columns.
  unsigned int            numColumns() const;

  // Input operator for copying the results.
  Result &                operator >> ( std::string & );
  Result &                operator >> ( double & );
  Result &                operator >> ( unsigned long & );

protected:

  friend class Database;

  // Constructor
  Result ( OCI_Statement *, Mutex & );

  // Destructor
  virtual ~Result();

private:

  // No default construction.
  Result();

  // Can not copy or assign.
  Result ( const Result & );
  Result &operator = ( const Result & );

  void                    _destroy();

  OCI_Statement *_statement;
  OCI_Resultset *_result;
  Mutex &_mutex;
};


} // namespace OracleWrap


#endif // _ORACLE_LITE_WRAP_RESULT_H_
