
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting the view
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_OSG_FOX_VIEW_H_
#define _USUL_INTERFACE_OSG_FOX_VIEW_H_

#include "Usul/Interfaces/IUnknown.h"

namespace OsgTools { namespace Render { class Viewer; }  }

namespace Usul {
namespace Interfaces {


struct IHeliosView : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IHeliosView );

  /// Id for this interface.
  enum { IID = 2445428656u };

  virtual OsgTools::Render::Viewer*       HeliosView() = 0;
  virtual const OsgTools::Render::Viewer* HeliosView() const = 0;
}; // struct IHeliosView


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_OSG_FOX_VIEW_H_
