
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for an on/off state.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACES_BOOLEAN_STATE_H_
#define _USUL_INTERFACES_BOOLEAN_STATE_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {


struct IBooleanState : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IBooleanState );

  // Id for this interface.
  enum { IID = 2723036100u };

  // Set/get the state.
  virtual void                          setBooleanState ( bool ) = 0;
  virtual bool                          getBooleanState() const = 0;
};


} // end namespace Interfaces
} // end namespace Usul


#endif // _USUL_INTERFACES_BOOLEAN_STATE_H_
