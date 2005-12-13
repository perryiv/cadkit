
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for toggling axes
//
///////////////////////////////////////////////////////////////////////////////

/*!
  @class IAxex
  @abstract Interface for togglinig axes.
  @superclass IUnknown
*/

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
  /*!
    @enum Id for this interface.
    @abstract Unique ID for this interface
    @discussion Unique ID for this interface
   */
  enum { IID = 2335136600u };

  /*!
    @function setAxes
    @abstract Set axes visibility
    @param show Hide/Show the axes.
   */

  virtual void               setAxes ( bool ) = 0;

  /*!
    @function hasAxes
    @abstract See if the axes are visible.
    @result Are the axes visible.
   */

  virtual bool               hasAxes ( ) const = 0;

};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_SPIN_H_
