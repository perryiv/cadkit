
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting the active document
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_ACTIVE_DOCUMENT_H_
#define _USUL_INTERFACE_ACTIVE_DOCUMENT_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IActiveDocument : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IActiveDocument );

  /// Id for this interface.
  enum { IID = 3735044595u };

  virtual Usul::Interfaces::IUnknown* getActiveDocument() = 0;
}; // struct IActiveDocument


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_ACTIVE_DOCUMENT_H_
