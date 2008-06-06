
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#include "WxPrecompiled.h"
#include "WxComponent.h"
#include "WxApplication.h"
#include "WxMainWindow.h"
#include "WxStatusBar.h"
#include "WxTextWindow.h"
#include "WxLogWindow.h"
#include "WxMenuBar.h"
#include "WxMenuGroup.h"
#include "WxMenuButton.h"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( WxComponent , WxComponent::BaseClass );
USUL_IMPLEMENT_TYPE_ID ( WxComponent );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

WxComponent::WxComponent() : BaseClass(),
  _mutex    (),
  _original ()
{
  // This plugin may get loaded by anything, so we don't want to be the 
  // reason the "program" gets instantiated.
  if ( AFW::Core::Program::valid() )
  {
    // Register factories.
    this->_registerFactory ( typeid ( AFW::Core::Application    ), new WxApplication::NewWxApplication );
    this->_registerFactory ( typeid ( AFW::Windows::MainWindow  ), new WxMainWindow::NewWxMainWindow   );
    this->_registerFactory ( typeid ( AFW::Windows::StatusBar   ), new WxStatusBar::NewWxStatusBar     );
    this->_registerFactory ( typeid ( AFW::Windows::TextWindow  ), new WxTextWindow::NewWxTextWindow   );
    this->_registerFactory ( typeid ( AFW::Windows::LogWindow   ), new WxLogWindow::NewWxLogWindow     );
    this->_registerFactory ( typeid ( AFW::Menus::MenuBar       ), new WxMenuBar::NewWxMenuBar         );
    this->_registerFactory ( typeid ( AFW::Menus::MenuGroup     ), new WxMenuGroup::NewWxMenuGroup     );
    this->_registerFactory ( typeid ( AFW::Menus::Button        ), new WxMenuButton::NewWxMenuButton   );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

WxComponent::~WxComponent()
{
  try
  {
    // Restore original factories.
    while ( false == _original.empty() )
    {
      Factories::iterator i ( _original.begin() );
      AFW::Core::Program::instance().factory ( i->first, i->second );
      _original.erase ( i );
    }
  }

  AFW_CATCH_BLOCK ( 3966798560ul );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *WxComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin * > ( this );
  case Usul::Interfaces::IChangeMenuBar::IID:
    return static_cast < Usul::Interfaces::IChangeMenuBar * > ( this );
  case Usul::Interfaces::IGuiInit::IID:
    return static_cast < Usul::Interfaces::IGuiInit * > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return unknown pointer.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *WxComponent::_unknown()
{
  return this->queryInterface ( Usul::Interfaces::IUnknown::IID );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return name of plugin.
//
///////////////////////////////////////////////////////////////////////////////

std::string WxComponent::getPluginName() const
{
  return "wxWindows GUI";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Register factory. Save the original one.
//
///////////////////////////////////////////////////////////////////////////////

void WxComponent::_registerFactory ( const std::type_info &info, AFW::Core::NewObjectFunctor *functor )
{
  Guard guard ( _mutex );

  // Get original factory.
  const std::string name ( info.name() );
  AFW::Core::NewObjectFunctor::RefPtr original ( AFW::Core::Program::instance().factory ( name ) );
  _original[name] = original;

  // Add new factory.
  AFW::Core::Program::instance().factory ( name, functor );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return name of plugin.
//
///////////////////////////////////////////////////////////////////////////////

void WxComponent::changeMenuBar ( IUnknown *caller )
{
  Guard guard ( _mutex );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the gui.
//
///////////////////////////////////////////////////////////////////////////////

void WxComponent::guiInit ( Usul::Interfaces::IUnknown * )
{
  Guard guard ( _mutex );
}
