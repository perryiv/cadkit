
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///
///  Interface for querying for a service
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_QUERY_SERVICE_H_
#define _USUL_INTERFACE_QUERY_SERVICE_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {

struct IQueryService : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IQueryService );

  /// Id for this interface.
  enum { IID = 1103227793u };

  virtual Usul::Interfaces::IUnknown* queryService( unsigned int iid ) = 0;

}; //  IQueryService

}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_QUERY_SERVICE_H_

