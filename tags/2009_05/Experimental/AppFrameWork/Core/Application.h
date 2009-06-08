
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Application class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_APPLICATION_CLASS_H_
#define _APP_FRAME_WORK_APPLICATION_CLASS_H_

#include "AppFrameWork/Core/Object.h"
#include "AppFrameWork/Core/Icon.h"
#include "AppFrameWork/Core/Model.h"
#include "AppFrameWork/Core/Constants.h"
#include "AppFrameWork/Actions/Action.h"

#include <string>
#include <vector>
#include <list>

namespace AFW { namespace Core { class BaseVisitor; class Registry; } }
namespace AFW { namespace Windows { class MainWindow; } }


namespace AFW {
namespace Core {


class APP_FRAME_WORK_EXPORT Application : public Object
{
public:

  // Typedefs.
  typedef Object BaseClass;
  typedef std::pair < AFW::Actions::Action::RefPtr, Object::RefPtr > Event;
  typedef std::list < Event > EventQueue;
  typedef std::vector < Model::RefPtr > Models;
  typedef std::list < std::string > RecentFiles;
  typedef std::pair < Icon, unsigned int > SplashData; // Image, milliseconds.

  // Possible ways to "run".
  enum RunCommand
  {
    RUN_MAIN_EVENT_LOOP,
    RUN_WHILE_EVENTS
  };

  // Accept the visitor.
  virtual void                        accept ( AFW::Core::BaseVisitor * );

  // Set/get the dirty flag.
  virtual void                        dirty ( bool );
  bool                                dirty() const;

  // Work with events.
  void                                eventAppend ( AFW::Actions::Action *, Object *object = 0x0 );
  void                                eventsFlush();

  // Initialize application, for two-step construction.
  virtual void                        init();

  // Set/get the main window. Getting is not thread-safe.
  const AFW::Windows::MainWindow *    mainWindow() const;
  AFW::Windows::MainWindow *          mainWindow();
  void                                mainWindow ( AFW::Windows::MainWindow * );

  // Set/get the name of the application.
  std::string                         name() const;
  void                                name ( const std::string &n );

  // Append/remove recent files.
  void                                recentFileAppend ( const std::string & );
  void                                recentFileRemove ( const std::string & );

  // Access registry.
  const Registry *                    registry() const;
  Registry *                          registry();

  // Quit the application.
  virtual void                        quit();

  // Run the application.
  virtual void                        run ( RunCommand );

  // Set/get the splash-screen image.
  void                                splashScreen ( const Icon &, unsigned int );
  SplashData                          splashScreen() const;

protected:

  // Constructor, destructor.
  Application();
  virtual ~Application();

  void                                _buildDefault();

  void                                _initRegistry();

private:

  // No copying.
  Application ( const Application & );
  Application &operator = ( const Application & );

  std::string _name;
  AFW::Windows::MainWindow *_mainWindow;
  AFW::Core::State::Type _flags;
  SplashData _splash;
  EventQueue _events;
  Models _models;
  RecentFiles _recentFiles;
  AFW::Core::Registry *_registry;

  AFW_DECLARE_OBJECT ( Application );
};


} // namespace Core
} // namespace AFW


#endif //_APP_FRAME_WORK_APPLICATION_CLASS_H_
