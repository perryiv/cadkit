
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Predicate class that determines if the file can be written to.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_PREDICATES_CAN_WRITE_TO_FILE_H_
#define _USUL_PREDICATES_CAN_WRITE_TO_FILE_H_

#include "Usul/Export/Export.h"

#include <string>
#include <functional>


namespace Usul {
namespace Predicates {


struct USUL_EXPORT CanWrite : public std::unary_function < const std::string &, bool >
{
  static bool test ( const std::string &s );
  bool operator () ( const std::string &s ) const;
};


} // namespace Predicates
} // namespace Usul


#endif // _USUL_PREDICATES_CAN_WRITE_TO_FILE_H_
