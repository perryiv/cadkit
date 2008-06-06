
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The wxWindows menu button class.
//
///////////////////////////////////////////////////////////////////////////////

#include "WxPrecompiled.h"
#include "WxMenuButton.h"
#include "WxMenuGroup.h"
#include "WxObjectMap.h"
#include "WxBitmap.h"

#include "Usul/Errors/Assert.h"

AFW_IMPLEMENT_OBJECT ( WxMenuButton );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

WxMenuButton::WxMenuButton() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

WxMenuButton::~WxMenuButton()
{
  // Safely...
  try
  {
    // Get our button and delete it.
    std::auto_ptr<wxMenuItem> button ( WxObjectMap::find<wxMenuItem> ( this ) );

    // Remove us from the map.
    WxObjectMap::remove ( this );
  }

  // Catch all exceptions.
  AFW_CATCH_BLOCK ( 2161300628ul );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return appropriate menu item kind.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  inline wxItemKind convert ( const AFW::Menus::Button::Type &type )
  {
    switch ( type )
    {
      case AFW::Menus::Button::MENU_CHECK:
        return wxITEM_CHECK;
      case AFW::Menus::Button::MENU_RADIO:
        return wxITEM_RADIO;
      case AFW::Menus::Button::MENU_SEPARATOR:
        return wxITEM_SEPARATOR;
      default:
        return wxITEM_NORMAL;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the menu item.
//
///////////////////////////////////////////////////////////////////////////////

wxMenuItem *WxMenuButton::create ( WxMenuGroup *menu )
{
  Guard guard ( this->mutex() );

  // Handle bad input.
  if ( 0x0 == menu || 0x0 == menu->get() )
    return 0x0;

  // Should be true.
  USUL_ASSERT ( 0x0 == WxObjectMap::find<wxMenuItem> ( this ) );

  // Remove us from the map.
  WxObjectMap::remove ( this );

  // Menu properties.
  const wxString text ( this->textGet().c_str() );
  const wxItemKind kind ( Detail::convert ( this->type() ) );
  const int id ( ( wxITEM_SEPARATOR == kind ) ? wxID_SEPARATOR : wxID_ANY );

  // Make menu item.
  std::auto_ptr<wxMenuItem> button ( new wxMenuItem ( menu->get(), id, text, wxString(), kind ) );

  // Try to load bitmap.
  wxBitmap bitmap ( WxBitmap::load ( this->icon().file(), false ) );
  if ( bitmap.Ok() )
    button->SetBitmap ( bitmap );

  // Add us to the map.
  WxObjectMap::set ( this, button.get() );

  // Return it.
  return button.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Detach from gui object.
//
///////////////////////////////////////////////////////////////////////////////

void WxMenuButton::detach()
{
  WxObjectMap::remove ( this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Enable the window.
//
///////////////////////////////////////////////////////////////////////////////

void WxMenuButton::enable ( bool state )
{
  Guard guard ( this->mutex() );

  // Get our button.
  wxMenuItem *button ( WxObjectMap::find<wxMenuItem> ( this ) );
  if ( button )
  {
    button->Enable ( state );
    BaseClass::enable ( button->IsEnabled() );
  }
  else
  {
    BaseClass::enable ( state );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Enable the window.
//
///////////////////////////////////////////////////////////////////////////////

void WxMenuButton::check ( bool state )
{
  Guard guard ( this->mutex() );

  // Get our button.
  wxMenuItem *button ( WxObjectMap::find<wxMenuItem> ( this ) );
  if ( button && button->IsCheckable() )
  {
    button->Check ( state );
    BaseClass::check ( button->IsChecked() );
  }
  else
  {
    BaseClass::check ( state );
  }
}
