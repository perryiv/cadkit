
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting width
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_GET_WIDTH_H_
#define _USUL_INTERFACE_GET_WIDTH_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IGetWidth : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IGetWidth );

  /// Id for this interface.
  enum { IID = 1835530174u };

  virtual int getWidthInt() = 0;

}; // IGetWidth


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_GET_WIDTH_H_
