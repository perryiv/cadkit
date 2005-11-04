
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

namespace OsgFox { namespace Views { class Canvas; }  }

namespace Usul {
namespace Interfaces {


struct IOsgFoxView : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IOsgFoxView );

  /// Id for this interface.
  enum { IID = 2445428656u };

  virtual OsgFox::Views::Canvas*       osgFoxView() = 0;
  virtual const OsgFox::Views::Canvas* osgFoxView() const = 0;
}; // struct IOsgFoxView


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_OSG_FOX_VIEW_H_
