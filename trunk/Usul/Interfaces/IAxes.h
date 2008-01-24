
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for setting axes state.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_AXES_H_
#define _USUL_INTERFACE_AXES_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IAxes : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IAxes );

  /// Id for this interface.
  enum { IID = 2335136600u };

  /// Hide/Show the axes.
  virtual void               axesShown ( bool ) = 0;
  virtual bool               isAxesShown() const = 0;

  /// Set the axes label.
  virtual void               axesLabels ( const std::string& x, const std::string& y, const std::string& z ) = 0;

}; // class IAxes.


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_SPIN_H_
