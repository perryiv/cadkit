
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __HELIOS_QT_CORE_SPLASH_SCREEN_H__
#define __HELIOS_QT_CORE_SPLASH_SCREEN_H__

#include "Usul/Interfaces/GUI/IStatusBar.h"
#include "Usul/Interfaces/GUI/IProgressBar.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Threads/RecursiveMutex.h"

#include "QtGui/QWidget"

class QLabel;
class QProgressBar;


namespace CadKit {
namespace Helios {
namespace Core {


class SplashScreen : public QWidget,
										 public Usul::Interfaces::IStatusBar,
										 public Usul::Interfaces::IProgressBar
{
	Q_OBJECT

public:

  typedef QWidget                           BaseClass;
  typedef Usul::Threads::RecursiveMutex     Mutex;
  typedef Usul::Threads::Guard<Mutex>       Guard;
	
  SplashScreen ( QWidget *mainWindow );
	~SplashScreen();

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( SplashScreen );

  // Usul::Interfaces::IUnknown members.
	USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Get the mutex.
  Mutex &                   mutex() const { return *_mutex; }

protected:
	
	/// Usul::Interfaces::IStatusBar
	/// Set the status bar text.
  virtual void              setStatusBarText ( const std::string &text, bool force );
	
	/// Usul::Interfaces::IProgressBar.h
	/// Show the progress bar
  virtual void              showProgressBar();

  /// Set the total of progress bar
  virtual void              setTotalProgressBar ( unsigned int value );

  /// Update the progress bar
  virtual void              updateProgressBar ( unsigned int value );

  /// Hide the progress bar
  virtual void              hideProgressBar();

private:

  void                      _loadSplashImage();

  QLabel *_image;
	QProgressBar *_progressBar;
	unsigned int _refCount;
  mutable Mutex *_mutex;
};


}
}
}


#endif // __HELIOS_QT_CORE_SPLASH_SCREEN_H__
