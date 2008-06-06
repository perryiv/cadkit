
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IVIEW_PORT_H__
#define __USUL_INTERFACES_IVIEW_PORT_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IViewport : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IViewport );

  /// Id for this interface.
  enum { IID = 2853292018u };

  virtual double x () const = 0;
  virtual double y () const = 0;
  virtual double height () const = 0;
  virtual double width () const = 0;

}; // struct IViewport


} // End namespace Interfaces
} // End namespace Usul

#endif // __USUL_INTERFACES_IVIEW_PORT_H__
