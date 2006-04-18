
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///
///  Interface for setting and releasing tools
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_SET_TOOL_H_
#define _USUL_INTERFACE_SET_TOOL_H_

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Interfaces/Fox/IFoxEvent.h"

namespace Usul {
namespace Interfaces {

struct ISetTool : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ISetTool );

  /// Id for this interface.
  enum { IID = 1104426962u };

  virtual void setTool ( Usul::Interfaces::IFoxEvent * ) = 0;
  virtual Usul::Interfaces::IFoxEvent* getTool () = 0;
  virtual void doneTool () = 0;
  virtual void loadLastTool () = 0;


}; //  ISetTool


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_SET_TOOL_H_
