
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  MDI Main window.
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Fox/Core/MdiMainWindow.h"

#include "Usul/Documents/Manager.h"
#include "Usul/Cast/Cast.h"
#include "Usul/File/Path.h"
#include "Usul/State/Busy.h"

#include "FoxTools/Errors/ErrorChecker.h"
#include "FoxTools/Windows/MdiChildWindow.h"
#include "FoxTools/Dialogs/Message.h"
#include "FoxTools/Functions/Enable.h"
#include "FoxTools/Functions/Check.h"
#include "FoxTools/Headers/MDIButton.h"
#include "FoxTools/Headers/MenuBar.h"
#include "FoxTools/Headers/MenuTitle.h"
#include "FoxTools/Headers/MenuCommand.h"
#include "FoxTools/Headers/MenuSeparator.h"
#include "FoxTools/Headers/App.h"
#include "FoxTools/Headers/MenuRadio.h"
#include "FoxTools/Headers/ToggleButton.h"
#include "FoxTools/Headers/MessageBox.h"
#include "FoxTools/Headers/MenuCheck.h"

#include "FoxTools/Menu/Button.h"
#include "FoxTools/Menu/Separator.h"

using namespace Helios::Core;


///////////////////////////////////////////////////////////////////////////////
//
//  Message maps.
//
///////////////////////////////////////////////////////////////////////////////

FXDEFMAP ( MdiMainWindow ) WindowMap[] = 
{
  // Message Type              ID                            Message Handler.
  FXMAPFUNC ( FX::SEL_UPDATE,  MdiMainWindow::ID_NEW_WINDOW, MdiMainWindow::onUpdateNeedingChild ),
};

FOX_TOOLS_IMPLEMENT ( MdiMainWindow, MdiMainWindow::BaseClass, WindowMap, ARRAYNUMBER ( WindowMap ) );

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( MdiMainWindow, MdiMainWindow::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Initializer list.
//
///////////////////////////////////////////////////////////////////////////////

#define INITIALIZER_LIST \
  _clientArea   ( 0x0 ), \
  _mdiMenu      ( 0x0 ), \
  _wasMaximized ( false )


///////////////////////////////////////////////////////////////////////////////
//
//  Typedefs
//
///////////////////////////////////////////////////////////////////////////////

typedef FoxTools::Menu::Group MenuGroup;
typedef FoxTools::Menu::Button MenuButton;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MdiMainWindow::MdiMainWindow() : BaseClass(), 
  INITIALIZER_LIST
{
  // If this static-assertion fails then make ISendMessage::ID_FIRST bigger.
  USUL_STATIC_ASSERT ( static_cast < unsigned int > ( Usul::Interfaces::ISendMessage::ID_FIRST ) > 
                       static_cast < unsigned int > ( MdiMainWindow::ID_LAST ) );

  // Do not use this constructor.
  FOX_TOOLS_ERROR_CHECKER ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MdiMainWindow::MdiMainWindow ( 
  FXApp *app,
  const FXString &name,
  FXIcon *icon,
  FXIcon *mi,
  FXuint options ) : 
  BaseClass ( app, name, icon, mi, options ), 
  INITIALIZER_LIST
{
  this->createDefaultGUI();

  FX::FXHorizontalFrame *hf ( new FX::FXHorizontalFrame ( this->splitterVertical(), FX::LAYOUT_FILL_X | FX::LAYOUT_FILL_Y | FX::FRAME_THICK | FX::FRAME_SUNKEN, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ) );
  _clientArea = new FX::FXMDIClient ( hf, FX::LAYOUT_FILL_X | FX::LAYOUT_FILL_Y | FX::FRAME_NONE );

  // Make MDI Menu
  _mdiMenu = new FX::FXMDIMenu ( this, _clientArea );

  // Insert the window menu before the help menu.
  this->_initWindowMenu();

  this->menuBar().clientArea  ( _clientArea );
  this->menuBar().mdiMenu     ( _mdiMenu    );

  this->menuBar().build();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MdiMainWindow::~MdiMainWindow()
{
  // Nothing to delete. FOX deletes all child windows.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a new child window.
//
///////////////////////////////////////////////////////////////////////////////

void MdiMainWindow::_initWindowMenu()
{
  // Window menu buttons.
  MenuGroup::ValidRefPtr window ( new MenuGroup ( "&Window" ) );
    window->append ( new MenuButton ( "New &Window",        this,        MdiMainWindow::ID_NEW_WINDOW       ) );
    window->append ( new MenuButton ( "Tile &Horizontally", _clientArea, FXMDIClient::ID_MDI_TILEHORIZONTAL ) );
    window->append ( new MenuButton ( "Tile &Vertically",   _clientArea, FXMDIClient::ID_MDI_TILEVERTICAL   ) );
    window->append ( new MenuButton ( "C&ascade",           _clientArea, FXMDIClient::ID_MDI_CASCADE        ) );
    window->append ( new MenuButton ( "&Close",             _clientArea, FXMDIClient::ID_MDI_CLOSE          ) );

    // A separator.
    FoxTools::Menu::Separator::ValidRefPtr separator ( new FoxTools::Menu::Separator );
    separator->target ( _clientArea );
    separator->selector ( FXMDIClient::ID_MDI_ANY );
    window->append ( separator );

    // Commands for each window.
    for ( unsigned int i = FXMDIClient::ID_MDI_1; i <= FXMDIClient::ID_MDI_10; ++i )
      window->append ( new MenuButton  ( "", _clientArea, i ) );

  // Insert before help menu
  this->menuBar().insert ( "Help", window );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the button is updated. This only checks for child windows.
//
///////////////////////////////////////////////////////////////////////////////

long MdiMainWindow::onUpdateNeedingChild ( FX::FXObject *object, FX::FXSelector, void * )
{
  // Enable or disable.
  FoxTools::Functions::enable ( this->activeChild() && !Usul::State::busy() , object );

  // Message is handled.
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the active child, which may be null.
//
///////////////////////////////////////////////////////////////////////////////

const MdiMainWindow::MdiChildWindow *MdiMainWindow::activeChild() const
{
  return SAFE_CAST_FOX_CONST ( MdiChildWindow, _clientArea->getActiveChild() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the active child, which may be null.
//
///////////////////////////////////////////////////////////////////////////////

MdiMainWindow::MdiChildWindow *MdiMainWindow::activeChild()
{
  return SAFE_CAST_FOX ( MdiChildWindow, _clientArea->getActiveChild() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start full screen mode
//
///////////////////////////////////////////////////////////////////////////////

void MdiMainWindow::_fullScreenStart()
{
  if ( 0x0 == this->activeChild() )
    return;

  _wasMaximized = ( this->activeChild()->isMaximized() ? true : false );
  this->activeChild()->maximize();
  BaseClass::_fullScreenStart();
}


///////////////////////////////////////////////////////////////////////////////
//
//  End full screen mode
//
///////////////////////////////////////////////////////////////////////////////

void MdiMainWindow::_fullScreenEnd()
{
  if ( this->activeChild() )
  {
    if ( _wasMaximized )
      this->activeChild()->maximize();
    else
      this->activeChild()->restore();
  }

  BaseClass::_fullScreenEnd();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface id
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* MdiMainWindow::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IFoxClientArea::IID:
    return static_cast < Usul::Interfaces::IFoxClientArea* > ( this );
  case Usul::Interfaces::IFoxMDIMenu::IID:
    return static_cast < Usul::Interfaces::IFoxMDIMenu* > ( this );
  default:
    return BaseClass::queryInterface( iid );
  }
}
