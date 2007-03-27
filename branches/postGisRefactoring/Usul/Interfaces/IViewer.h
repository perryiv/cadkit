
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

#ifndef _USUL_INTERFACE_VIEWER_H_
#define _USUL_INTERFACE_VIEWER_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IViewer : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IViewer );

  /// Id for this interface.
  enum { IID = 3739984930u };

  virtual void render() = 0;

  virtual void clearScene() = 0;

  // TODO Move these to a IViewport interface
  virtual int x() const = 0;
  virtual int y() const = 0;
  virtual unsigned int height() const = 0;
  virtual unsigned int width() const = 0;

  virtual void handleMessage ( unsigned short message ) = 0;

}; // struct IViewer


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_VIEWER_H_
