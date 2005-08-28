
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting height
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_GET_HEIGHT_H_
#define _USUL_INTERFACE_GET_HEIGHT_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IGetHeight : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IGetHeight );

  /// Id for this interface.
  enum { IID = 1104766572u };

  virtual int getHeightInt() = 0;

}; // class IGetHeight


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_GET_HEIGHT_H_
