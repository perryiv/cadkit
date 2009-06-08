
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_DIRTY_STATE_H__
#define __USUL_INTERFACES_DIRTY_STATE_H__

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {


struct IDirtyState : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IDirtyState );

  /// Id for this interface.
  enum { IID = 3837989730u };

  /// Set/get the dirty state.
  virtual void                          dirtyState ( bool ) = 0;
  virtual bool                          dirtyState() const = 0;
};


} // end namespace Interfaces
} // end namespace Usul


#endif // __USUL_INTERFACES_DIRTY_STATE_H__
