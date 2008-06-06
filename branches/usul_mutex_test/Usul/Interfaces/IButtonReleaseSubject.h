
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for a button release.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACES_BUTTON_RELEASE_SUBJECT_H_
#define _USUL_INTERFACES_BUTTON_RELEASE_SUBJECT_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {


struct IButtonReleaseSubject : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IButtonReleaseSubject );

  // Id for this interface.
  enum { IID = 3549366798u };

  // Add the listener.
  virtual void            addButtonReleaseListener ( Usul::Interfaces::IUnknown * ) = 0;

  // Remove all listeners.
  virtual void            clearButtonReleaseListeners() = 0;

  // Remove the listener.
  virtual void            removeButtonReleaseListener ( Usul::Interfaces::IUnknown * ) = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACES_BUTTON_RELEASE_SUBJECT_H_
