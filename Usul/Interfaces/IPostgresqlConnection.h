
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IPOSTGRESQL_CONNECTION_H__
#define __USUL_INTERFACES_IPOSTGRESQL_CONNECTION_H__

#include "Usul/Interfaces/IUnknown.h"

namespace pqxx { class result; }

namespace Usul {
namespace Interfaces {


struct IPostgresqlConnection : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IPostgresqlConnection );

  /// Id for this interface.
  enum { IID = 3571046046u };

  /// Execute the query.
  virtual pqxx::result         executeQuery( const std::string& ) const = 0;

}; // struct IPostgresqlConnection


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_IPOSTGRESQL_CONNECTION_H__ */

