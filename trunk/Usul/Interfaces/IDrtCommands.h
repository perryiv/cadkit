
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Jeff Conner
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IDrtCommands_H__
#define __USUL_INTERFACES_IDrtCommands_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IDrtCommands : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IDrtCommands );

  /// Id for this interface.
  enum { IID = 2516491801u };

  // set the animation state
  virtual void              animating ( bool state ) = 0;

  // get the animation state
  virtual bool              animating () = 0;

  virtual void				startOver () = 0;

  virtual void				agentShow ( bool state )  = 0;
  virtual bool				agentShow () = 0;

  virtual void				stockShow ( bool state ) = 0;
  virtual bool				stockShow () = 0;

  virtual void				sporeShow ( bool state ) = 0;
  virtual bool				sporeShow () = 0;

  virtual void				transShow ( bool state ) = 0;
  virtual bool				transShow () = 0;



}; //struct IDrtCommands



}
}

#endif // __USUL_INTERFACES_IDrtCommands_H__
