
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for managing a separate thread.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VIRTUAL_REALITY_VIEWER_INTERFACE_THREAD_H_
#define _VIRTUAL_REALITY_VIEWER_INTERFACE_THREAD_H_

#include "Usul/Interfaces/IUnknown.h"

#include <string>


namespace VRV {
namespace Interfaces {


struct IThread : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IThread );

  // Id for this interface.
  enum { IID = 3567866689u };

  // Possible states.
  enum State
  {
    NOT_STARTED,
    RUNNING,
    FINISHED,
    CANCELLED,
  };

  // Is the thread done?
  virtual bool            done() const = 0;

  // Get the error string.
  virtual std::string     error() const = 0;

  // Get/set the name of the thread.
  virtual void            name ( const std::string &n ) = 0;
  virtual std::string     name() const = 0;

  // Report final results, if any.
  virtual void            report() = 0;

  // Is the thread running?
  virtual bool            running() const = 0;

  // Start the thread.
  virtual void            start() = 0;

  // Stop the thread.
  virtual void            stop() = 0;
};


}; // namespace Interfaces
}; // namespace VRV


#endif // _VIRTUAL_REALITY_VIEWER_INTERFACE_THREAD_H_
