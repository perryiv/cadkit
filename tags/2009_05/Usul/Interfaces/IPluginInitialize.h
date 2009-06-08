
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Jeff Conner
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for plugin
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_PLUGIN_INITIALIZE_H_
#define _USUL_INTERFACE_PLUGIN_INITIALIZE_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {


struct IPluginInitialize : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IPluginInitialize );

  /// Id for this interface.
  enum { IID = 3010870509u };

  virtual void initializePlugin ( Usul::Interfaces::IUnknown *caller ) = 0;

}; // IPluginInitialize


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_PLUGIN_INITIALIZE_H_
