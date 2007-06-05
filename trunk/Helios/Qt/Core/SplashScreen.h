
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

#include "Helios/Qt/Core/Export.h"

#include "Usul/Base/Object.h"
#include "Usul/Interfaces/GUI/IStatusBar.h"
#include "Usul/Interfaces/GUI/IProgressBar.h"

#include <string>

class QWidget;
class QLabel;
class QProgressBar;


namespace CadKit {
namespace Helios {
namespace Core {


class SplashScreen : public Usul::Base::Object,
										 public Usul::Interfaces::IStatusBar,
										 public Usul::Interfaces::IProgressBar
{
public:

  typedef Usul::Base::Object  BaseClass;
  typedef BaseClass::Mutex    Mutex;
  typedef BaseClass::Guard    Guard;

  // Constructor.
  SplashScreen ( const std::string &image );

  // Type information.
  USUL_DECLARE_TYPE_ID ( SplashScreen );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( SplashScreen );

  // Usul::Interfaces::IUnknown members.
	USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Show and hide this window.
  void                      show();
  void                      hide();

protected:

  // Use reference counting.
	virtual ~SplashScreen();

  // Usul::Interfaces::IStatusBar
	// Set the status bar text.
  virtual void              setStatusBarText ( const std::string &text, bool force );
	
	// Usul::Interfaces::IProgressBar.h
	// Show the progress bar
  virtual void              showProgressBar();

  // Set the total of progress bar
  virtual void              setTotalProgressBar ( unsigned int value );

  // Update the progress bar
  virtual void              updateProgressBar ( unsigned int value );

  // Hide the progress bar
  virtual void              hideProgressBar();

private:

  // No copying or assignment.
  SplashScreen ( const SplashScreen & );
  SplashScreen &operator = ( const SplashScreen & );

  void                      _destroy();

  void                      _loadSplashImage();

  QWidget *_window;
  QLabel *_image;
	QProgressBar *_progress;
  std::string _file;
};


} // namespace Core
} // namespace Helios
} // namespace CadKit


#endif // __HELIOS_QT_CORE_SPLASH_SCREEN_H__
