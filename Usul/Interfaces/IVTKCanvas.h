
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting the VTK canvas
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_VTK_CANVAS_H_
#define _USUL_INTERFACE_VTK_CANVAS_H_

#include "Usul/Interfaces/IUnknown.h"

namespace OsgFox { namespace Views { namespace VTK { class Canvas; }  } }

namespace Usul {
namespace Interfaces {


struct IVTKCanvas : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IVTKCanvas );

  /// Id for this interface.
  enum { IID = 1891500283u };

  virtual OsgFox::Views::VTK::Canvas*       vtkCanvas() = 0;
  virtual const OsgFox::Views::VTK::Canvas* vtkCanvas() const = 0;

}; // struct IVTKCanvas


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_VTK_CANVAS_H_
