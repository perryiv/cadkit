
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for reading a menu configuration.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VIRTUAL_REALITY_VIEWER_INTERFACE_READ_MENU_FILE_H_
#define _VIRTUAL_REALITY_VIEWER_INTERFACE_READ_MENU_FILE_H_

#include "Usul/Interfaces/IUnknown.h"

#include <string>


namespace VRV {
namespace Interfaces {


struct IMenuRead : public Usul::Interfaces::IUnknown
{
  typedef Usul::Interfaces::IUnknown Unknown;

  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IMenuRead );

  // Id for this interface.
  enum { IID = 4260194301u };

  // Read the file and build the menu.
  virtual void readMenuFile ( const std::string &filename, Unknown *caller ) = 0;
};


}; // namespace Interfaces
}; // namespace VRV


#endif // _VIRTUAL_REALITY_VIEWER_INTERFACE_READ_MENU_FILE_H_
