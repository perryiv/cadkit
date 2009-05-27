
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Jeff Conner
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IVPIDELEGATE_H__
#define __USUL_INTERFACES_IVPIDELEGATE_H__

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Math/Vector4.h"

#include <vector>
#include <map>

namespace VaporIntrusionGUI {
namespace Interfaces {

struct IVPIDelegate : public Usul::Interfaces::IUnknown
{

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IVPIDelegate );

  /// Id for this interface.
  enum { IID = 1235852807u };

  // Add a dock
  virtual void    addDock( const std::string& name ) = 0;

  // add a material to a dock
  virtual void    addToDock( const std::string& dockName, const std::string& name, const std::string& value ) = 0;
    
}; // struct IVPIDelegate

}
}

#endif // __USUL_INTERFACES_IVPIDELEGATE_H__
