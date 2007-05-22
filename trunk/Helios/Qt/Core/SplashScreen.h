
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

#include "QtGui/QWidget"

class QSplashScreen;
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
	typedef QWidget BaseClass;
	
  SplashScreen( QWidget *mainWindow );
	~SplashScreen();
	
	USUL_DECLARE_IUNKNOWN_MEMBERS;
	
protected:
	
	/// Usul::Interfaces::IStatusBar
	/// Set the status bar text.
  virtual void setStatusBarText ( const std::string &text, bool force );
	
	/// Usul::Interfaces::IProgressBar.h
	/// Show the progress bar
  virtual void showProgressBar();

  /// Set the total of progress bar
  virtual void setTotalProgressBar ( unsigned int value );

  /// Update the progress bar
  virtual void updateProgressBar ( unsigned int value );

  /// Hide the progress bar
  virtual void hideProgressBar();
  
private:
	QSplashScreen *_splashScreen;
	QProgressBar  *_progressBar;
	
	unsigned int _refCount;
	
};


}
}
}


#endif // __HELIOS_QT_CORE_SPLASH_SCREEN_H__
