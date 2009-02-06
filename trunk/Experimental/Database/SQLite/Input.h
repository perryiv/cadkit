
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Generic input operator for copying the results. This is in a separate 
//  header file because it is optional, and not always desired.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SQL_LITE_WRAP_GENERIC_INPUT_FROM_RESULTS_H_
#define _SQL_LITE_WRAP_GENERIC_INPUT_FROM_RESULTS_H_

#include "Database/SQLite/Result.h"

#include "Usul/Strings/Format.h"
#include "Usul/Types/Types.h"

#include <stdexcept>


///////////////////////////////////////////////////////////////////////////////
//
//  Generic definition of input operator for copying the results.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > CadKit::Database::SQLite::Result &operator >> ( CadKit::Database::SQLite::Result &result, T &t )
{
  typedef typename CadKit::Database::SQLite::Blob Blob;

  Blob blob;
  result >> blob;

  const Usul::Types::Uint64 sizeOfT ( sizeof ( T ) );
  const Usul::Types::Uint64 sizeOfBlob ( blob.size() * sizeof ( Blob::value_type ) );
  if ( sizeOfT != sizeOfBlob )
  {
    throw std::runtime_error ( Usul::Strings::format 
      ( "Error 5821006000: Size of blob data is ", sizeOfBlob, " when given type's size is ", sizeOfT ) );
  }

  const T *temp ( reinterpret_cast < T * > ( &blob[0] ) );
  t = (*temp);

  return result;
}


#endif // _SQL_LITE_WRAP_GENERIC_INPUT_FROM_RESULTS_H_
