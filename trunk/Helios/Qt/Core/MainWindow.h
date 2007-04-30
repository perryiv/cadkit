
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
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

#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"

#include "QtCore/QSettings"
#include "QtGui/QMainWindow"

#include <string>


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

  // Constructor and destructor.
  MainWindow ( const std::string &vendor, const std::string &url, const std::string &program );
  virtual ~MainWindow();

  // See if the calling thread is the GUI thread.
  bool                      isGuiThread() const;

  // Get the mutex.
  Mutex &                   mutex() const { return *_mutex; }

protected:

  void                      _buildMenu();

  void                      _loadSettings();

  void                      _saveSettings();

private slots:

  void                      _quit();
  void                      _open();

private:

  void                      _destroy();

  mutable Mutex *_mutex;
  QSettings _settings;
  const unsigned long _guiThread;
};


} // namespace Core
} // namespace Helios
} // namespace CadKit


#endif //_CADKIT_HELIOS_QT_CORE_MAIN_WINDOW_H_
