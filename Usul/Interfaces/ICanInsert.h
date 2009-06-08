
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_CAN_INSERT_H_
#define _USUL_INTERFACE_CAN_INSERT_H_

#include "Usul/Interfaces/IUnknown.h"

#include <map>
#include <string>

namespace Usul {
namespace Interfaces {

struct ICanInsert : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ICanInsert );

  /// Id for this interface.
  enum { IID = 1833599936u };

  virtual bool                canInsert ( const std::string &ext ) const = 0;

}; // class ICanInsert


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_CAN_INSERT_H_
