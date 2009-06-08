
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The AnimationControlComponent class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Qt/Plugins/AnimationControl/AnimationControlComponent.h"

#include "Usul/Components/Factory.h"
#include "Usul/Interfaces/Qt/IMainWindow.h"
#include "Usul/Interfaces/IQtDockWidgetMenu.h"

#include "QtTools/AnimationControl.h"

#include "QtGui/QDockWidget"
#include "QtGui/QHeaderView"
#include "QtGui/QMainWindow"
#include "QtGui/QTreeWidget"

USUL_DECLARE_COMPONENT_FACTORY ( AnimationControlComponent );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( AnimationControlComponent, AnimationControlComponent::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

AnimationControlComponent::AnimationControlComponent() : BaseClass(),
  _caller(),
  _dock ( 0x0 ),
	_animation ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

AnimationControlComponent::~AnimationControlComponent()
{
  if ( 0x0 != _dock )
  {
    Usul::Interfaces::Qt::IMainWindow::QueryPtr mainWindow ( _caller );

    if( mainWindow.valid() )
    {
      // Remove the DockWidget from the MainWindow.
      QMainWindow * main ( mainWindow->mainWindow() );
      main->removeDockWidget ( _dock );
    }

    _dock->setWidget ( 0x0 );

    // Delete the DockWidget.
    delete _dock;   
  }

  _dock = 0x0;

	// This was just deleted by the parent.
  _animation = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *AnimationControlComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPluginInitialize::IID:
    return static_cast< Usul::Interfaces::IPluginInitialize*>(this);
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin*>(this);
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the tab for the scene tree.
//
///////////////////////////////////////////////////////////////////////////////

void AnimationControlComponent::initializePlugin ( Usul::Interfaces::IUnknown *caller )
{
  _caller = caller;

  Usul::Interfaces::Qt::IMainWindow::QueryPtr mainWindow ( caller );

  if ( mainWindow.valid() )
  {
    QMainWindow * main ( mainWindow->mainWindow() );

    // Build the docking window.
    _dock = new QDockWidget ( QObject::tr ( "Animation Control" ), main );
    _dock->setAllowedAreas ( Qt::AllDockWidgetAreas );

    // Create the widget
    _animation = new QtTools::AnimationControl ( caller, 0x0 );

    // Add the dock to the main window.
    _dock->setWidget( _animation );
    main->addDockWidget ( Qt::LeftDockWidgetArea, _dock );

    // Set the object name.
    _dock->setObjectName ( "Animation Control" );

    // Add toggle to the menu.
    Usul::Interfaces::IQtDockWidgetMenu::QueryPtr dwm ( caller );
    if ( dwm.valid () )
      dwm->addDockWidgetMenu ( _dock );
  }
}
