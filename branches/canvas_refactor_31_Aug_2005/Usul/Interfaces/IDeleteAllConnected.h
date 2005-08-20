
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Delete all primitives connected to seed.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_DELETE_ALL_CONNECTED_H_
#define _USUL_INTERFACE_DELETE_ALL_CONNECTED_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IDeleteAllConnected : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IDeleteAllConnected );

  /// Id for this interface.
  enum { IID = 1313723240u };

  virtual void deleteAllConnected ( Usul::Interfaces::IUnknown* caller, unsigned int seed ) = 0;

}; // struct IDeleteAllConnected


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_DELETE_ALL_CONNECTED_H_
