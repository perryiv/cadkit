
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Thread class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VIRTUAL_REALITY_VIEWER_THREAD_CLASS_H_
#define _VIRTUAL_REALITY_VIEWER_THREAD_CLASS_H_

#include "VRV/Interfaces/IThread.h"
#include "VRV/Core/Mutex.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Threads/Variable.h"

namespace vpr { class BaseThread; };


namespace VRV {
namespace Threads {


class Thread : public Usul::Base::Referenced,
               public VRV::Interfaces::IThread
{
public:

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Thread );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

protected:

  Thread();
  virtual ~Thread();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  VRV::Interfaces::IThread
  //
  /////////////////////////////////////////////////////////////////////////////

  // Is the thread done?
  virtual bool            done() const;

  // Get the error string.
  virtual std::string     error() const;

  // Get/set the name of the Thread.
  virtual void            name ( const std::string &n );
  virtual std::string     name() const;

  // Report final results, if any.
  virtual void            report();

  // Is the thread running?
  virtual bool            running() const;

  // Start the Thread.
  virtual void            start();

  // Stop the Thread.
  virtual void            stop();

  /////////////////////////////////////////////////////////////////////////////

  virtual void            _setError   ( const std::string &e );
  virtual void            _setRunning ( bool r );
  virtual void            _setDone    ( bool d );

  virtual void            _run() = 0;
  void                    _run ( void * );

private:

  typedef VRV::Threads::Mutex Mutex;

  // No copying or assigning.
  Thread ( const Thread & );
  Thread &operator = ( const Thread & );

  vpr::BaseThread *_thread;
  Usul::Threads::Variable<bool> _done;
  Usul::Threads::Variable<bool> _running;
  Usul::Threads::Variable<std::string> _name;
  Usul::Threads::Variable<std::string> _error;
};


}; // namespace Threads
}; // namespace VRV


#endif // _VIRTUAL_REALITY_VIEWER_THREAD_CLASS_H_
