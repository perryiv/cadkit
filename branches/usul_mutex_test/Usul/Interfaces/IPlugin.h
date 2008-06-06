
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for plugin
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_PLUGIN_H_
#define _USUL_INTERFACE_PLUGIN_H_

#include "Usul/Interfaces/IUnknown.h"

#include <string>


namespace Usul {
namespace Interfaces {


struct IPlugin : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IPlugin );

  /// Id for this interface.
  enum { IID = 2497111645u };

  virtual std::string getPluginName() const = 0;

}; // IPlugin


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_PLUGIN_H_
