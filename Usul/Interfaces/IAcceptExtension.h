
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface to see if plugin accepts given extension
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_ACCEPT_EXTENSION_H_
#define _USUL_INTERFACE_ACCEPT_EXTENSION_H_

#include "Usul/Interfaces/IUnknown.h"

#include <string>


namespace Usul {
namespace Interfaces {


struct IAcceptExtension : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IAcceptExtension );

  /// Id for this interface.
  enum { IID = 2988214617u };

  /// Does this accept given extension?
  virtual bool acceptExtension( const std::string& ext ) = 0;
};


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_ACCEPT_EXTENSION_H_
