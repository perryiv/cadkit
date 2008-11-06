
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Qt/Core/SplashScreen.h"

#include "QtTools/Show.h"
#include "QtTools/Image.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Threads/Named.h"
#include "Usul/Trace/Trace.h"

#include "QtGui/QLabel"
#include "QtGui/QPixmap"
#include "QtGui/QProgressBar"

using namespace CadKit::Helios::Core;

USUL_IMPLEMENT_TYPE_ID ( SplashScreen );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( SplashScreen, SplashScreen::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SplashScreen::SplashScreen ( const std::string &image ) : BaseClass(),
  _window   ( 0x0 ),
  _image    ( 0x0 ),
  _progress ( 0x0 ),
  _file     ( image )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor. Qt deletes child widgets.
//
///////////////////////////////////////////////////////////////////////////////

SplashScreen::~SplashScreen()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &SplashScreen::_destroy ), "6002667960" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor contents.
//
///////////////////////////////////////////////////////////////////////////////

void SplashScreen::_destroy()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD_OR_THROW ( "2336754210" );
  this->hide();
  _file.clear();
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

  // As of Qt 4.4.3, this deletes the pixmap (the image) which has already 
  // been set. QLabel::setText() does this by calling d->clearContents(), 
  // which is QLabelPrivate::clearContents().
#if 0
  if ( 0x0 != _image )
  {
    _image->setText ( text.c_str() );
  }
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Implementating this for a splash screen does not make sense.
//
///////////////////////////////////////////////////////////////////////////////

void SplashScreen::showProgressBar()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD_OR_THROW ( "6541436130" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Implementating this for a splash screen does not make sense.
//
///////////////////////////////////////////////////////////////////////////////

void SplashScreen::hideProgressBar()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD_OR_THROW ( "1401918977" );
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

  if ( 0x0 != _progress )
		_progress->setRange ( 0, static_cast < int > ( value ) );
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

  if ( 0x0 != _progress )
		_progress->setValue ( static_cast < int > ( value ) );
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
//  Load the splash screen image.
//
///////////////////////////////////////////////////////////////////////////////

void SplashScreen::_loadSplashImage()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD_OR_THROW ( "8048315890" );

  QtTools::Image::pixmap ( _file, _image );
  if ( ( 0x0 == _image ) || ( 0x0 == _image->pixmap() ) || ( true == _image->pixmap()->isNull() ) )
  {
    std::cout << "Warning 3987519476: Failed to load splash screen image: " << _file << std::endl;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show this window.
//
///////////////////////////////////////////////////////////////////////////////

void SplashScreen::show()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD_OR_THROW ( "3645128969" );

  // Handle repeated calls.
  if ( 0x0 != _window )
  {
		QtTools::Show::center ( _window );
    return;
  }

  // Always on top.
  //const Qt::WindowFlags flags ( Qt::FramelessWindowHint | Qt::SplashScreen | Qt::WindowStaysOnTopHint );

  // The above is too problematic when debugging during startup.
  const Qt::WindowFlags flags ( Qt::FramelessWindowHint | Qt::SplashScreen );

  // Make the top-level window.
  _window = new QWidget ( 0x0, Qt::FramelessWindowHint | Qt::SplashScreen );

  // Make the image and progress bar.
  _image = new QLabel ( _window );
  _progress = new QProgressBar ( _window );

  // No margins.
  _window->setContentsMargins ( 0, 0, 0, 0 );
  _image->setContentsMargins ( 0, 0, 0, 0 );
  _progress->setContentsMargins ( 0, 0, 0, 0 );

  // Load the image.
  this->_loadSplashImage();

  // Could not get layout to work. This code makes sense.
  if ( ( 0x0 != _image ) && ( 0x0 != _image->pixmap() ) && ( false == _image->pixmap()->isNull() ) )
  {
    _image->setGeometry ( 0, 0, _image->pixmap()->width(), _image->pixmap()->height() );
    _progress->setGeometry ( 0, _image->height(), _image->width(), _progress->height() / 2 );
  }

  // Show it at the center of the screen.
  QtTools::Show::center ( _window );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Hide this window.
//
///////////////////////////////////////////////////////////////////////////////

void SplashScreen::hide()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD_OR_THROW ( "3011770616" );

  // Delete parent window.
  delete _window; _window = 0x0;

  // These should already be deleted by above statement.
  _image = 0x0;
	_progress = 0x0;
}
