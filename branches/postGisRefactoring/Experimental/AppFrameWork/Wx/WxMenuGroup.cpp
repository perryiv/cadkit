
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The wxWindows menu group class.
//
///////////////////////////////////////////////////////////////////////////////

#include "WxPrecompiled.h"
#include "WxMenuGroup.h"
#include "WxMenuButton.h"
#include "WxObjectMap.h"

#include "Usul/Errors/Assert.h"

AFW_IMPLEMENT_OBJECT ( WxMenuGroup );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

WxMenuGroup::WxMenuGroup() : BaseClass()
{
  // Make menu.
  std::auto_ptr<wxMenu> menu ( new wxMenu );

  // Add us to the map.
  WxObjectMap::set ( this, menu.get() );

  // Keep it.
  menu.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

WxMenuGroup::~WxMenuGroup()
{
  // Safely...
  try
  {
    // Get our menu and delete it.
    std::auto_ptr<wxMenu> menu ( WxObjectMap::find<wxMenu> ( this ) );

    // Remove us from the map.
    WxObjectMap::remove ( this );

    // The wxMenu is about to die and it will delete the sub-menus and buttons.
    std::for_each ( this->begin(), this->end(), std::mem_fun ( AFW::Core::Object::detach ) );

    // Remove all children.
    this->removeAll();
  }

  // Catch all exceptions.
  AFW_CATCH_BLOCK ( 2089862060ul );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the existing menu.
//
///////////////////////////////////////////////////////////////////////////////

wxMenu *WxMenuGroup::get()
{
  Guard guard ( this->mutex() );
  return ( WxObjectMap::find<wxMenu> ( this ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the existing menu.
//
///////////////////////////////////////////////////////////////////////////////

const wxMenu *WxMenuGroup::get() const
{
  Guard guard ( this->mutex() );
  return ( WxObjectMap::find<wxMenu> ( this ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Insert the window.
//
///////////////////////////////////////////////////////////////////////////////

bool WxMenuGroup::insert ( Itr where, Window *w )
{
  Guard guard ( this->mutex() );

  // Try to insert it.
  if ( false == BaseClass::insert ( where, w ) )
    return false;

  // Is it a menu button?
  WxMenuButton::RefPtr button ( dynamic_cast < WxMenuButton * > ( w ) );
  if ( false == button.valid() )
    return true;

  // Get the menu.
  wxMenu *menu ( this->get() );
  if ( 0x0 == menu )
    return false;

  // Create the menu item.
  wxMenuItem *item ( button->create ( this ) );
  if ( 0x0 == item )
    return false;

  // Add it to the menu.
  menu->Append ( item );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Detach from gui object.
//
///////////////////////////////////////////////////////////////////////////////

void WxMenuGroup::detach()
{
  WxObjectMap::remove ( this );
}
