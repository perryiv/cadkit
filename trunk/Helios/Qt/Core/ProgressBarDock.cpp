
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Qt/Core/ProgressBarDock.h"

#include "Usul/Interfaces/GUI/IProgressBar.h"
#include "Usul/Threads/Named.h"

#include "QtGui/QWidget"
#include "QtGui/QDockWidget"
#include "QtGui/QProgressBar"
#include "QtGui/QVBoxLayout"
#include "QtGui/QScrollArea"

using namespace CadKit::Helios::Core;

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ProgressBarDock::ProgressBarDock () : BaseClass (),
_widget ( 0x0 ),
_layout ( 0x0 )
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
  //QScrollArea *scroll ( new QScrollArea ( parent ) );
  _widget = new QScrollArea ( parent );
  _layout = new QVBoxLayout ( _widget );

  return _widget;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Wrapper around QProgressBar.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  class ProgressBar : public Usul::Base::Referenced,
                      public Usul::Interfaces::IProgressBar
  {
  public:
    typedef Usul::Base::Referenced BaseClass;

    USUL_DECLARE_QUERY_POINTERS ( ProgressBar );

    ProgressBar () : BaseClass (), _progressBar ( 0x0 )
    {
    }

    void ref ()
    {
      BaseClass::ref ();
    }

    void unref ( bool allowDeletion = true )
    {
      BaseClass::unref ( allowDeletion );
    }


    Usul::Interfaces::IUnknown* queryInterface ( unsigned long iid )
    {
	    switch ( iid )
	    {
	    case Usul::Interfaces::IUnknown::IID:
	    case Usul::Interfaces::IProgressBar::IID:
		    return static_cast < Usul::Interfaces::IProgressBar *  > ( this );
	    default:
		    return 0x0;
	    }
    }

    QWidget* operator () ( QWidget *parent )
    {
      _progressBar = new QProgressBar ( parent );
      return _progressBar;
    }

  protected:
    virtual ~ProgressBar ()
    {
      this->hideProgressBar ();

      // Defer this delete for the main thread to take care of.
      if ( 0x0 != _progressBar )
      {
        _progressBar->deleteLater ();
        _progressBar = 0x0;
      }
    }

    // Usul::Interfaces::IProgressBar.h
	  // Show the progress bar
    virtual void showProgressBar()
    {
      if ( 0x0 != _progressBar )
      {
        QMetaObject::invokeMethod ( _progressBar, "show", Qt::AutoConnection );
      }
    }

    // Set the total of progress bar
    virtual void setTotalProgressBar ( unsigned int value )
    {
      if ( 0x0 != _progressBar )
      {
        QMetaObject::invokeMethod ( _progressBar, "setMaximum", Qt::AutoConnection, 
                                Q_ARG ( int, static_cast < int > ( value ) ) );

        QMetaObject::invokeMethod ( _progressBar, "setMinimum ", Qt::AutoConnection, 
                                Q_ARG ( int, static_cast < int > ( 0 ) ) );
      }
    }

    // Update the progress bar
    virtual void updateProgressBar ( unsigned int value )
    {
      if ( 0x0 != _progressBar )
      {
        QMetaObject::invokeMethod ( _progressBar, "setValue", Qt::AutoConnection, 
                                Q_ARG ( int, static_cast < int > ( value ) ) );
      }
    }

    // Hide the progress bar
    virtual void hideProgressBar()
    {
      if ( 0x0 != _progressBar )
      {
        QMetaObject::invokeMethod ( _progressBar, "hide", Qt::AutoConnection );
      }
    }

  private:
    QProgressBar *_progressBar;
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append a progress bar.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown * ProgressBarDock::createProgressBar ()
{
  // Progress bars can only be created in the gui thread.
  if ( false == Usul::Threads::Named::instance().is ( Usul::Threads::Names::GUI ) )
    return 0x0;

  Detail::ProgressBar::RefPtr progress ( new Detail::ProgressBar );

  QWidget *w ( ( *progress ) ( _widget ) );
  
  if ( 0x0 != _layout )
    _layout->addWidget ( w );

  return progress.release();
}
