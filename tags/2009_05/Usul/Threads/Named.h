
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Container of named threads.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_THREADS_NAMED_THREADS_CLASS_H_
#define _USUL_THREADS_NAMED_THREADS_CLASS_H_

#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/Guard.h"

#include <map>
#include <string>


namespace Usul {
namespace Threads {


class USUL_EXPORT Named
{
public:

  // Useful typedefs.
  typedef Usul::Threads::Mutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;
  typedef std::map<std::string,unsigned long> ThreadMap;

  // Add the pair.
  void                    add ( const std::string &name, unsigned long id );

  // Singleton destruction.
  static void             clear();

  // Find the id. Throws if the name does not exist.
  unsigned long           find ( const std::string &name ) const;

  // See if the name exists.
  bool                    has ( const std::string &name ) const;

  // Singleton construction.
  static Named &          instance();

  // See if the current thread is registered by the given name.
  bool                    is ( const std::string &name ) const;

  // Return the mutex. Use with caution.
  Mutex &                 mutex() const;

  // Set the name of the current thread.
  void                    set ( const std::string &name );

protected:

  // Constructor and destructor.
  Named();
  ~Named();

private:

  // No copying or assigning.
  Named ( const Named & );
  Named &operator = ( const Named & );

  void                    _destroy();

  // Data members.
  static Named *_instance;
  mutable Mutex *_mutex;
  ThreadMap _map;
};


// Common names.
namespace Names
{
  const std::string MAIN      ( "main_thread" );
  const std::string GUI       ( "gui_thread" );
  const std::string INSTANCE  ( "instance_thread" );
}


} // namespace Threads
} // namespace Usul


///////////////////////////////////////////////////////////////////////////////
//
//  Convenient macro(s).
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_THREADS_ENSURE_GUI_THREAD(exp)\
  if ( false == Usul::Threads::Named::instance().is ( Usul::Threads::Names::GUI ) )\
  {\
    exp;\
  }
#define USUL_THREADS_ENSURE_GUI_THREAD_OR_THROW(id)\
  USUL_THREADS_ENSURE_GUI_THREAD ( throw std::runtime_error ( std::string ( "Error " ) + std::string ( id ) + std::string ( ": Not GUI thread" ) ) );

#endif // _USUL_THREADS_NAMED_THREADS_CLASS_H_
