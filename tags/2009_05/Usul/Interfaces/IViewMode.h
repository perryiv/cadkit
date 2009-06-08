
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting the view mode
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_VIEW_MODE_H_
#define _USUL_INTERFACE_VIEW_MODE_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IViewMode : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IViewMode );

  /// Id for this interface.
  enum { IID = 3486164218u };
  
  enum ViewMode
  {
    NAVIGATION,
    PICK,
    SEEK,
    NONE
  };

  /// Set/get the mode.
  virtual void                  setViewMode ( ViewMode mode ) = 0;
  virtual ViewMode              getViewMode () const = 0;
  virtual bool                  isViewModeCurrent ( ViewMode mode ) const = 0;

}; // class IViewMode


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_VIEW_MODE_H_
