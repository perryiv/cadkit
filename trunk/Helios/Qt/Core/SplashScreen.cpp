
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Qt/Core/SplashScreen.h"

#include "Usul/Errors/Assert.h"
#include "Usul/App/Application.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Threads/Named.h"
#include "Usul/Trace/Trace.h"

#include "QtGui/QLabel"
#include "QtGui/QSplashScreen"
#include "QtGui/QProgressBar"
#include "QtGui/QPixmap"
#include "QtGui/QVBoxLayout"

using namespace CadKit::Helios::Core;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SplashScreen::SplashScreen ( QWidget *mainWindow ) : 
  BaseClass ( 0x0, Qt::FramelessWindowHint | Qt::SplashScreen | Qt::WindowStaysOnTopHint ),
  _image ( 0x0 ),
  _progressBar ( 0x0 ),
  _refCount ( 0 ),
  _mutex()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD_OR_THROW ( "7770121390" );

  _image = new QLabel ( this );
  _progressBar = new QProgressBar ( this );

  this->setContentsMargins ( 0, 0, 0, 0 );
  _image->setContentsMargins ( 0, 0, 0, 0 );
  _progressBar->setContentsMargins ( 0, 0, 0, 0 );

  this->_loadSplashImage();

  // Could not get layout to work. This code makes sense.
  _image->setGeometry ( 0, 0, _image->pixmap()->width(), _image->pixmap()->height() );
  _progressBar->setGeometry ( 0, _image->height(), _image->width(), _progressBar->height() / 2 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor. Qt deletes child widgets.
//
///////////////////////////////////////////////////////////////////////////////

SplashScreen::~SplashScreen()
{
  USUL_TRACE_SCOPE;

	// Make sure.
	USUL_ASSERT ( 0 == _refCount );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the status bar text.
//
///////////////////////////////////////////////////////////////////////////////

void SplashScreen::setStatusBarText ( const std::string &text, bool force )
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD_OR_THROW ( "2442782510" );

  if ( 0x0 != _image )
		_image->setText ( tr ( text.c_str() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show the progress bar.
//
///////////////////////////////////////////////////////////////////////////////

void SplashScreen::showProgressBar()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD_OR_THROW ( "6541436130" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the total of progress bar.
//
///////////////////////////////////////////////////////////////////////////////

void SplashScreen::setTotalProgressBar ( unsigned int value )
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD_OR_THROW ( "1137254021" );

  if ( 0x0 != _progressBar )
		_progressBar->setRange ( 0, static_cast < int > ( value ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the progress bar.
//
///////////////////////////////////////////////////////////////////////////////

void SplashScreen::updateProgressBar ( unsigned int value )
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD_OR_THROW ( "1900994211" );

  if ( 0x0 != _progressBar )
		_progressBar->setValue ( static_cast < int > ( value ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Hide the progress bar.
//
///////////////////////////////////////////////////////////////////////////////

void SplashScreen::hideProgressBar()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD_OR_THROW ( "1401918977" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* SplashScreen::queryInterface ( unsigned long iid )
{
  USUL_TRACE_SCOPE;

	switch ( iid )
	{
	case Usul::Interfaces::IUnknown::IID:
	case Usul::Interfaces::IStatusBar::IID:
		return static_cast < Usul::Interfaces::IStatusBar* > ( this );
	case Usul::Interfaces::IProgressBar::IID:
		return static_cast < Usul::Interfaces::IProgressBar *  > ( this );
	default:
		return 0x0;
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//  Ref.  Keep track for debugging purposes.
//
///////////////////////////////////////////////////////////////////////////////

void SplashScreen::ref()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
	++_refCount;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unref.  Do not delete.  Qt will handle deletion.
//
///////////////////////////////////////////////////////////////////////////////

void SplashScreen::unref ( bool )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  if ( 0 == _refCount )
  {
    USUL_ASSERT ( 0 );
    Usul::Exceptions::Thrower<std::runtime_error> ( "Error 3396931127: Reference count is already 0" );
  }

  --_refCount;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the splash screen image.
//
///////////////////////////////////////////////////////////////////////////////

void SplashScreen::_loadSplashImage()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD_OR_THROW ( "8048315890" );

  // Try to load the splash screen image.
  QPixmap pixmap;
  std::string splashImage ( Usul::App::Application::instance().splashImagePath() );

  if ( true == Usul::Predicates::FileExists::test ( splashImage ) )
  {
    if ( false == pixmap.load ( splashImage.c_str() ) )
    {
      std::cout << "Warning 3987519476: Failed to load splash screen image :" << splashImage << std::endl;
    }
  }
  else
  {
    std::cout << "Warning 3987519476: Splash screen image '" << splashImage << "' does not exist" << std::endl;
  }

  // If we have an image then set it.
  if ( false == pixmap.isNull() )
  {
    _image->setPixmap ( pixmap );
  }
}
