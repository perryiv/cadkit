
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Main window.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_HELIOS_QT_CORE_MAIN_WINDOW_H_
#define _CADKIT_HELIOS_QT_CORE_MAIN_WINDOW_H_

#include "Helios/Qt/Core/Export.h"

#include "Usul/Threads/Guard.h"
#include "Usul/Threads/Pool.h"
#include "Usul/Threads/RecursiveMutex.h"

#include "QtCore/QSettings"
#include "QtGui/QMainWindow"

#include <string>
#include <map>

namespace Usul { namespace Threads { class Thread; } }

namespace CadKit {
namespace Helios {
namespace Core {


class HELIOS_QT_CORE_EXPORT MainWindow : public QMainWindow
{
  Q_OBJECT

public:

  // Useful typedefs.
  typedef QMainWindow                       BaseClass;
  typedef Usul::Threads::RecursiveMutex     Mutex;
  typedef Usul::Threads::Guard<Mutex>       Guard;
  typedef std::map<std::string,QAction*>    Actions;
  typedef std::map<std::string,QToolBar*>   ToolBars;
  typedef Usul::Threads::Pool               ThreadPool;

  // Constructor and destructor.
  MainWindow ( const std::string &vendor, const std::string &url, const std::string &program );
  virtual ~MainWindow();

  // Get the mutex.
  Mutex &                   mutex() const { return *_mutex; }

  // Get the settings.
  const QSettings &         settings() const;
  QSettings &               settings();

protected:

  void                      _buildMenu();
  void                      _buildToolBar();

  void                      _loadSettings();

  void                      _makeActions();

  void                      _saveSettings();
  
  /// Load plugin functions.
  void                      _loadPlugins();
  void                      _startLoadPlugins  ( Usul::Threads::Thread* );
  void                      _finishLoadPlugins ( Usul::Threads::Thread* );

private slots:

  void                      _quit();
  void                      _open();

private:

  // No copying or assignment.
  MainWindow ( const MainWindow & );
  MainWindow &operator = ( const MainWindow & );

  void                      _destroy();

  mutable Mutex *_mutex;
  QSettings _settings;
  Actions _actions;
  ToolBars _toolBars;
  ThreadPool::ValidAccessRefPtr _threadPool;
};


} // namespace Core
} // namespace Helios
} // namespace CadKit


#endif //_CADKIT_HELIOS_QT_CORE_MAIN_WINDOW_H_
