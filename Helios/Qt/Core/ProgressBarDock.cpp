
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Qt/Core/ProgressBarDock.h"

#include "Usul/Threads/Named.h"

#include "QtGui/QWidget"
#include "QtGui/QDockWidget"
#include "QtGui/QProgressBar"
#include "QtGui/QVBoxLayout"
#include "QtGui/QScrollArea"
#include "QtGui/QLabel"

using namespace CadKit::Helios::Core;

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ProgressBarDock::ProgressBarDock () : BaseClass (),
_widget ( 0x0 ),
_layout ( 0x0 ),
_progressBars ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ProgressBarDock::~ProgressBarDock ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the Widget.
//
///////////////////////////////////////////////////////////////////////////////

QWidget* ProgressBarDock::operator () ( QDockWidget* parent )
{
  _widget = new QScrollArea ( parent );
  _layout = new QVBoxLayout ( _widget );
  _widget->setLayout ( _layout );

  return _widget;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Wrapper around QProgressBar.
//
///////////////////////////////////////////////////////////////////////////////

ProgressBarDock::ProgressBar::ProgressBar () : 
  BaseClass (),
  _progressBar ( 0x0 ),
  _label ( 0x0 ),
  _layout ( 0x0 ),
  _parentLayout ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reference.
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarDock::ProgressBar::ref ()
{
  BaseClass::ref ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unreference.
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarDock::ProgressBar::unref ( bool allowDeletion )
{
  BaseClass::unref ( allowDeletion );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* ProgressBarDock::ProgressBar::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IProgressBar::IID:
    return static_cast < Usul::Interfaces::IProgressBar * > ( this );
  case Usul::Interfaces::IStatusBar::IID:
    return static_cast < Usul::Interfaces::IStatusBar * > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a progress bar.
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarDock::ProgressBar::operator () ( QVBoxLayout *vLayout )
{
  QVBoxLayout *layout ( new QVBoxLayout );
  _label = new QLabel;
  _progressBar = new QProgressBar;
  
  layout->addWidget ( _label );
  layout->addWidget ( _progressBar );
  layout->addStretch();

  vLayout->addLayout ( layout );

  _parentLayout = vLayout;
  _layout = layout;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the progress bar.
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarDock::ProgressBar::progressBar ( QProgressBar * bar )
{
  _progressBar = bar;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ProgressBarDock::ProgressBar::~ProgressBar ()
{
  this->hideProgressBar ();

  if ( 0x0 != _parentLayout )
    _parentLayout->removeItem ( _layout );

  // Defer this delete for the main thread to take care of.
  if ( 0x0 != _progressBar )
  {
    _progressBar->deleteLater ();
    _progressBar = 0x0;
  }

  if ( 0x0 != _label )
  {
    this->setStatusBarText ( "", true );
    _label->deleteLater();
    _label = 0x0;
  }
}



///////////////////////////////////////////////////////////////////////////////
//
//  Show the progress bar.
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarDock::ProgressBar::showProgressBar()
{
  if ( 0x0 != _progressBar )
  {
    QMetaObject::invokeMethod ( _progressBar, "show", Qt::AutoConnection );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the total of progress bar.
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarDock::ProgressBar::setTotalProgressBar ( unsigned int value )
{
  if ( 0x0 != _progressBar )
  {
    QMetaObject::invokeMethod ( _progressBar, "setMaximum", Qt::AutoConnection, 
                            Q_ARG ( int, static_cast < int > ( value ) ) );

    QMetaObject::invokeMethod ( _progressBar, "setMinimum ", Qt::AutoConnection, 
                            Q_ARG ( int, static_cast < int > ( 0 ) ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the progress bar.
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarDock::ProgressBar::updateProgressBar ( unsigned int value )
{
  if ( 0x0 != _progressBar )
  {
    QMetaObject::invokeMethod ( _progressBar, "setValue", Qt::AutoConnection, 
                            Q_ARG ( int, static_cast < int > ( value ) ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Hide the progress bar.
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarDock::ProgressBar::hideProgressBar()
{
  if ( 0x0 != _progressBar )
  {
    QMetaObject::invokeMethod ( _progressBar, "hide", Qt::AutoConnection );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the status bar text.
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarDock::ProgressBar::setStatusBarText ( const std::string &text, bool force )
{
  if ( 0x0 != _label )
  {
    QMetaObject::invokeMethod ( _label, "setText", Qt::AutoConnection, 
      Q_ARG ( QString, text.c_str() ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append a progress bar.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown * ProgressBarDock::createProgressBar ()
{
  ProgressBar::RefPtr progress ( new ProgressBar );

  // Progress bars can only be created in the gui thread.
  if ( false == Usul::Threads::Named::instance().is ( Usul::Threads::Names::GUI ) )
  {
    {
      Guard guard ( this->mutex () );
      _progressBars.push_back ( progress.get() );
    }

    QMetaObject::invokeMethod ( this, "_updateProgressBars", Qt::QueuedConnection );
    return progress->queryInterface ( Usul::Interfaces::IUnknown::IID );
  }
  else
  {
    if ( 0x0 != _layout )
      (*progress) ( _layout );

    Usul::Interfaces::IUnknown::QueryPtr unknown ( progress );
    progress = 0x0;
    return unknown.release();
  }

  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Give the wrappers actual progress bars.
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarDock::_updateProgressBars ()
{
  ProgressBar::RefPtr first ( 0x0 );
  {
    Guard guard ( this->mutex () );
    if ( false == _progressBars.empty () );
    {
      first = _progressBars.front();
      _progressBars.pop_front();
    }
  }

  if ( first.valid () )
  {
    //QProgressBar *bar ( new QProgressBar ( _widget ) );
    //first->progressBar ( bar );
  
    if ( 0x0 != _layout )
      (*first) ( _layout );
    _widget->update();
  }
}

