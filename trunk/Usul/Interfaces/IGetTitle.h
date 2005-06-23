
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_GET_TITLE_H_
#define _USUL_INTERFACE_GET_TITLE_H_

#include "Usul/Interfaces/IUnknown.h"

#include <string>

namespace Usul {
namespace Interfaces {

struct IWindow;

struct IGetTitle : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IGetTitle );

  /// Id for this interface.
  enum { IID = 2425521902u };

  virtual std::string   getTitle ( Usul::Interfaces::IWindow* ) = 0;


}; // class IGetTitle


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_GET_TITLE_H_
