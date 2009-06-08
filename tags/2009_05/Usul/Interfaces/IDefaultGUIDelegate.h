
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///
///  Interface the specify the default GUI delegate.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_DEFAULT_GUI_DELEGATE_H_
#define _USUL_INTERFACE_DEFAULT_GUI_DELEGATE_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {
    
    
struct IDefaultGUIDelegate : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IDefaultGUIDelegate );
      
  /// Id for this interface.
  enum { IID = 2912918608u };
};
    
    
} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_DEFAULT_GUI_DELEGATE_H_
