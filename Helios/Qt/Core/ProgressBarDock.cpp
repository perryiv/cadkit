
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Qt/Core/ProgressBarDock.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/System/Sleep.h"
#include "Usul/Threads/Named.h"
#include "Usul/Threads/Safe.h"

#include "QtGui/QWidget"
#include "QtGui/QDockWidget"
#include "QtGui/QProgressBar"
#include "QtGui/QVBoxLayout"
#include "QtGui/QScrollArea"
#include "QtGui/QLabel"

using namespace CadKit::Helios::Core;

USUL_IMPLEMENT_TYPE_ID ( ProgressBarDock::ProgressBar );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ProgressBarDock::ProgressBarDock() : BaseClass(),
  _scrollArea ( 0x0 ),
  _layout ( 0x0 ),
  _progressBars(),
  _canCreate ( true )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ProgressBarDock::~ProgressBarDock()
{
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &ProgressBarDock::_destroy ), "2999505269" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarDock::_destroy()
{
  // No more progress bars allowed.
  {
    Guard guard ( this );
    _canCreate = false;
  }

  // Clean up the scroll area.
  QScrollArea *sa ( 0x0 );
  {
    Guard guard ( this );
    sa = _scrollArea;
    _scrollArea = 0x0;
  }
  delete sa;

  // Copy the progress bars proxy classes and clear the member.
  ProgressBars bars;
  {
    Guard guard ( this );
    bars = _progressBars;
    _progressBars.clear();
  }

  // Tell all the pending progress-bar proxy classes that the real 
  // progress bars, as well as their dock-bar, are going away.
  while ( false == bars.empty() )
  {
    ProgressBars::value_type bar ( bars.front() );
    if ( true == bar.valid() )
    {
      bar->setProgressBar ( 0x0 );
    }
    bars.pop_front();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the Widget.
//
///////////////////////////////////////////////////////////////////////////////

QWidget* ProgressBarDock::operator() ( QDockWidget* parent )
{
  _scrollArea = new QScrollArea ( parent );
  
  QWidget *widget ( new QWidget );
  
  _layout = new QVBoxLayout;

  widget->setLayout ( _layout );

  _scrollArea->setWidget ( widget );
  _scrollArea->setWidgetResizable ( true );

  return _scrollArea;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Wrapper around QProgressBar.
//
///////////////////////////////////////////////////////////////////////////////

ProgressBarDock::ProgressBar::ProgressBar() : 
  BaseClass(),
  _progressBar ( 0x0 ),
  _label ( 0x0 ),
  _layout ( 0x0 ),
  _parentLayout ( 0x0 ),
  _created ( false )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reference.
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarDock::ProgressBar::ref()
{
  BaseClass::ref();
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
//  Is it created?
//
///////////////////////////////////////////////////////////////////////////////

bool ProgressBarDock::ProgressBar::isCreated() const
{
  Guard guard ( this );
  return ( true == _created );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a progress bar.
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarDock::ProgressBar::operator() ( QVBoxLayout *vLayout )
{
  Guard guard ( this );

  QVBoxLayout *layout ( new QVBoxLayout );
  _label = new QLabel;
  _progressBar = new QProgressBar;
  
  layout->addWidget ( _label );
  layout->addWidget ( _progressBar );
  layout->addStretch();

  vLayout->addLayout ( layout );

  _parentLayout = vLayout;
  _layout = layout;

  _created = true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the progress bar.
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarDock::ProgressBar::setProgressBar ( QProgressBar * bar )
{
  Guard guard ( this );
  _progressBar = bar;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ProgressBarDock::ProgressBar::~ProgressBar()
{
  this->hideProgressBar();

  // This function is not thread safe.  Just hide for now.
  //if ( 0x0 != _parentLayout )
  //  _parentLayout->removeItem ( _layout );

  // Defer this delete for the main thread to take care of.
  {
    Guard guard ( this );
    QProgressBar *bar ( _progressBar );
    _progressBar = 0x0;
    if ( 0x0 != bar )
    {
      bar->deleteLater();
    }
  }

  {
    Guard guard ( this );
    QLabel *label ( _label );
    _label = 0x0;
    if ( 0x0 != label )
    {
      QMetaObject::invokeMethod ( label, "setText", Qt::AutoConnection, Q_ARG ( QString, "" ) );
      label->deleteLater();
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show the progress bar.
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarDock::ProgressBar::showProgressBar()
{
  QProgressBar *progressBar ( Usul::Threads::Safe::get ( this->mutex(), _progressBar ) );
  if ( 0x0 != progressBar )
  {
    QMetaObject::invokeMethod ( progressBar, "show", Qt::AutoConnection );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the total of progress bar.
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarDock::ProgressBar::setTotalProgressBar ( unsigned int value )
{
  QProgressBar *progressBar ( Usul::Threads::Safe::get ( this->mutex(), _progressBar ) );
  if ( 0x0 != progressBar )
  {
    QMetaObject::invokeMethod ( progressBar, "setMaximum", Qt::AutoConnection, 
                            Q_ARG ( int, static_cast < int > ( value ) ) );

    QMetaObject::invokeMethod ( progressBar, "setMinimum ", Qt::AutoConnection, 
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
  QProgressBar *progressBar ( Usul::Threads::Safe::get ( this->mutex(), _progressBar ) );
  if ( 0x0 != progressBar )
  {
    QMetaObject::invokeMethod ( progressBar, "setValue", Qt::AutoConnection, 
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
  QProgressBar *progressBar ( Usul::Threads::Safe::get ( this->mutex(), _progressBar ) );
  if ( 0x0 != progressBar )
  {
    QMetaObject::invokeMethod ( progressBar, "hide", Qt::AutoConnection );
  }

  QLabel *label ( Usul::Threads::Safe::get ( this->mutex(), _label ) );
  if ( 0x0 != label )
  {
    QMetaObject::invokeMethod ( label, "hide", Qt::AutoConnection );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the status bar text.
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarDock::ProgressBar::setStatusBarText ( const std::string &text, bool force )
{
  QLabel *label ( Usul::Threads::Safe::get ( this->mutex(), _label ) );
  if ( 0x0 != label )
  {
    QMetaObject::invokeMethod ( label, "setText", Qt::AutoConnection, 
      Q_ARG ( QString, text.c_str() ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append a progress bar.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown::RefPtr ProgressBarDock::createProgressBar ( bool waitIfNotGuiThread )
{
  if ( false == Usul::Threads::Safe::get ( this->mutex(), _canCreate ) )
  {
    return Usul::Interfaces::IUnknown::RefPtr ( 0x0 );
  }

  ProgressBar::RefPtr progress ( new ProgressBar );

  // Progress bars can only be created in the gui thread.
  if ( false == Usul::Threads::Named::instance().is ( Usul::Threads::Names::GUI ) )
  {
    {
      Guard guard ( this );
      _progressBars.push_back ( progress.get() );
    }

    // Make the progress bar in the main thread.
    QMetaObject::invokeMethod ( this, "_updateProgressBars", Qt::QueuedConnection );

    // Are we supposed to wait for it to be created?
    if ( true == waitIfNotGuiThread )
    {
      while ( false == progress->isCreated() )
      {
        Usul::System::Sleep::milliseconds ( 100 );
      }
    }

    // Return as unknown interface.
    return Usul::Interfaces::IUnknown::QueryPtr ( progress );
  }

  // We're in the gui thread.
  else
  {
    QVBoxLayout *layout ( Usul::Threads::Safe::get ( this->mutex(), _layout ) );
    if ( 0x0 != layout )
      (*progress) ( layout );

    return Usul::Interfaces::IUnknown::QueryPtr ( progress );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Give the wrappers actual progress bars.
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarDock::_updateProgressBars()
{
  if ( false == Usul::Threads::Safe::get ( this->mutex(), _canCreate ) )
  {
    return;
  }

  ProgressBar::RefPtr first ( 0x0 );
  {
    Guard guard ( this );
    if ( false == _progressBars.empty() )
    {
      first = _progressBars.front();
      _progressBars.pop_front();
    }
  }

  if ( first.valid() )
  {
    QVBoxLayout *layout ( Usul::Threads::Safe::get ( this->mutex(), _layout ) );
    if ( 0x0 != layout )
    {
      (*first) ( layout );
    }
    QScrollArea *scrollArea ( Usul::Threads::Safe::get ( this->mutex(), _scrollArea ) );
    if ( 0x0 != scrollArea )
    {
      scrollArea->update();
    }
  }
}

