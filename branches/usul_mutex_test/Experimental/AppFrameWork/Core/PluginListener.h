
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Plugin activity listener.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_PLUGIN_LISTENER_CLASS_H_
#define _APP_FRAME_WORK_PLUGIN_LISTENER_CLASS_H_

#include "AppFrameWork/Core/Export.h"

#include "Usul/DLL/Listener.h"
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"

#include <string>


namespace AFW {
namespace Core {


class APP_FRAME_WORK_EXPORT PluginListener : public Usul::DLL::Listener
{
public:

  // Typedefs.
  typedef Usul::DLL::Listener BaseClass;
  typedef Usul::Threads::RecursiveMutex Mutex;
  typedef Usul::Threads::Guard < Mutex > Guard;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( PluginListener );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( PluginListener );

  // Constructor
  PluginListener();

  // Access to the mutex.
  Mutex &                             mutex() const;

  // Called when there is plugin activity.
  virtual void                        operator () ( const std::string & );

  // Get the text. Calling this is not thread-safe. Use mutex function above.
  const std::string &                 text() const;

protected:

  // Use reference counting.
  virtual ~PluginListener();

private:

  // No copying.
  PluginListener ( const PluginListener & );
  PluginListener &operator = ( const PluginListener & );

  std::string _text;
  mutable Mutex _mutex;
};


} // namespace Core
} // namespace AFW


#endif //_APP_FRAME_WORK_PLUGIN_LISTENER_CLASS_H_
