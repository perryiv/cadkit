
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting positions
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_GET_POSITIONS_H_
#define _USUL_INTERFACE_GET_POSITIONS_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IGetPosition : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IGetPosition );

  /// Id for this interface.
  enum { IID = 3237528025u };

  virtual int getXInt() = 0;
  virtual int getYInt() = 0;

}; // IGetPosition


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_GET_POSITIONS_H_
