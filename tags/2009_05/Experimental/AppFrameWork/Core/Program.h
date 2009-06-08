
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Convenience class for managing the program life-cycle.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_PROGRAM_CLASS_H_
#define _APP_FRAME_WORK_PROGRAM_CLASS_H_

#include "AppFrameWork/Core/Export.h"
#include "AppFrameWork/Core/NewObjectFunctor.h"
#include "AppFrameWork/Core/PluginListener.h"

#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"

#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>

namespace AFW { namespace Core { class InitFunctor; class Application; } }


namespace AFW {
namespace Core {


class APP_FRAME_WORK_EXPORT Program
{
public:

  // Typedefs.
  typedef Usul::Threads::RecursiveMutex Mutex;
  typedef Usul::Threads::Guard < Mutex > Guard;
  typedef std::map < std::string, NewObjectFunctor::RefPtr > Factories;
  typedef std::vector < std::string > StringVector;
  typedef std::set < std::string > PluginExtensions;
  typedef std::list < std::string > PluginFiles;

  // Set/get the application. Getting is not thread-safe.
  void                                app ( Application * );
  const Application *                 app() const;
  Application *                       app();

  // Convenience functions related to the application.
  void                                appInit ( InitFunctor *init, const std::string & );
  void                                appRun();

  // Clean up the application.
  void                                cleanup();

  // Destroy singleton.
  static void                         destroy();

  // Return any accumulated errors. Flushes the errors.
  std::string                         errors ( bool includeSystemErrors = false ) const;

  // Convenience function to run and cleanup the application.
  void                                execute ( int argc, char **argv, InitFunctor *init = 0x0, const std::string &name = std::string() );

  // Set/get the factory.
  void                                factory ( const std::string &, NewObjectFunctor * );
  NewObjectFunctor *                  factory ( const std::string & );

  // It's a singleton.
  static Program &                    instance();

  // Access to the mutex.
  Mutex &                             mutex() const;

  // Create a new object.
  Object *                            newObject ( const std::string & );
  template < class T > T *            newObject() { return ( dynamic_cast < T * > ( this->newObject ( ( typeid ( T ) ).name() ) ) ); }

  // Set/get the plugin extensions.
  PluginExtensions                    pluginExtensions() const;
  void                                pluginExtensions ( const PluginExtensions &p );

  // Get the plugin listener. This function is not thread-safe.
  PluginListener *                    pluginListener();

  // Work with plugins.
  PluginFiles                         pluginsFind() const;
  void                                pluginsLoad();
  void                                pluginsRelease();

  // Set/get the name of the file that stdio and stderr are redirected to.
  std::string                         redirect() const;
  void                                redirect ( const std::string & );

  // Run application. Assumes the application has already been created.
  void                                run();

  // Set/get stdout or stderr.
  void                                standardOutAppend ( const char *s, std::streamsize );
  unsigned int                        standardOutGet ( unsigned int start, std::string & );

  // See if the single instance is valid.
  static bool                         valid();

  // Set/get the name of the vendor.
  std::string                         vendor() const;
  void                                vendor ( const std::string &v );

protected:

  // Constructor, destructor.
  Program();
  ~Program();

  std::string                         _defaultRedirect ( const std::string &name ) const;

  void                                _execute ( int argc, char **argv, InitFunctor *init, const std::string &name );
  void                                _execute ( InitFunctor *init, const std::string &name );

  void                                _readPluginDir ( PluginFiles & ) const;
  bool                                _readPluginRegistry ( PluginFiles & ) const;

private:

  // No copying.
  Program ( const Program & );
  Program &operator = ( const Program & );

  mutable Mutex _mutex;
  Factories _factories;
  StringVector _stdout;
  PluginExtensions _plugExts;
  PluginListener::RefPtr _pluginCB;
  std::string _redirect;
  std::string _vendor;
  Application *_app;
};


} // namespace Core
} // namespace AFW



///////////////////////////////////////////////////////////////////////////////
//
//  Macro for guarding access to the program instance.
//
///////////////////////////////////////////////////////////////////////////////

#define AFW_GUARD_PROGRAM\
  AFW::Core::Program::Guard program_guard ( AFW::Core::Program::instance().mutex() )


#endif //_APP_FRAME_WORK_PROGRAM_CLASS_H_
