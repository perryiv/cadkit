
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for to see if Unknown is handled
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_HANDLE_H_
#define _USUL_INTERFACE_HANDLE_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IHandle : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IHandle );

  /// Id for this interface.
  enum { IID = 3506136566u };

  virtual bool doesHandle( Usul::Interfaces::IUnknown* ) = 0;

}; // struct IHandle


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_HANDLE_H_
