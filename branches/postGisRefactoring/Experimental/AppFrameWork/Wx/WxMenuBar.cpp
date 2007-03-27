
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The wxWindows menu bar class.
//
///////////////////////////////////////////////////////////////////////////////

#include "WxPrecompiled.h"
#include "WxMenuBar.h"
#include "WxObjectMap.h"
#include "WxEventHandler.h"
#include "WxMenuGroup.h"
#include "AppFrameWork/Menus/Button.h"

#include "Usul/Errors/Assert.h"

AFW_IMPLEMENT_OBJECT ( WxMenuBar );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

WxMenuBar::WxMenuBar() : BaseClass()
{
  // Make menu bar.
  std::auto_ptr<wxMenuBar> bar ( new wxMenuBar );

  // Add another event handler.
  bar->PushEventHandler ( new WxEventHandler );

  // Add us to the map.
  WxObjectMap::set ( this, bar.release() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

WxMenuBar::~WxMenuBar()
{
  // Safely...
  try
  {
    // Need local scope.
    {
      // If there is a menu bar then delete it.
      wxFrame *frame ( wxDynamicCast ( wxTheApp->GetTopWindow(), wxFrame ) );
      wxMenuBar *menuBar ( ( frame ) ? frame->GetMenuBar() : 0x0 );
      if ( frame && menuBar )
      {
        // Remove all child menus.
        while ( menuBar && menuBar->GetMenuCount() )
          menuBar->Remove ( menuBar->GetMenuCount() - 1 );

        // Make sure the frame does not have a menu bar.
        frame->SetMenuBar ( 0x0 );

        // Remove it from the map.
        WxObjectMap::remove ( menuBar );

        // Destroy the menu bar.
        menuBar->Destroy();

        // Update the main window.
        frame->Refresh();
      }
    }

    // Need local scope.
    {
      // Get our menu bar and delete it.
      std::auto_ptr<wxMenuBar> bar ( WxObjectMap::find<wxMenuBar> ( this ) );

      // Remove us from the map.
      WxObjectMap::remove ( this );

      // The wxMenuBar is about to die and it will delete the menus.
      std::for_each ( this->begin(), this->end(), std::mem_fun ( AFW::Core::Object::detach ) );

      // Remove all children.
      this->removeAll();
    }
  }

  // Catch all exceptions.
  AFW_CATCH_BLOCK ( 2089862060ul );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the existing menu bar.
//
///////////////////////////////////////////////////////////////////////////////

wxMenuBar *WxMenuBar::get()
{
  Guard guard ( this->mutex() );
  return ( WxObjectMap::find<wxMenuBar> ( this ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the existing menu bar.
//
///////////////////////////////////////////////////////////////////////////////

const wxMenuBar *WxMenuBar::get() const
{
  Guard guard ( this->mutex() );
  return ( WxObjectMap::find<wxMenuBar> ( this ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Insert the window.
//
///////////////////////////////////////////////////////////////////////////////

bool WxMenuBar::insert ( Itr where, Window *w )
{
  Guard guard ( this->mutex() );

  // Try to insert it.
  if ( false == BaseClass::insert ( where, w ) )
    return false;

  // Is it a menu group?
  WxMenuGroup::RefPtr group ( dynamic_cast < WxMenuGroup * > ( w ) );
  if ( false == group.valid() )
    return false;

  // Get the menu.
  wxMenu *menu ( group->get() );
  if ( 0x0 == menu )
    return false;

  // Get the menu bar.
  wxMenuBar *bar ( this->get() );
  if ( 0x0 == bar )
    return false;

  // Add it to the menu bar.
  wxString text ( group->textGet().c_str() );
  bar->Append ( menu, text );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Detach from gui object.
//
///////////////////////////////////////////////////////////////////////////////

void WxMenuBar::detach()
{
  WxObjectMap::remove ( this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a default GUI.
//
///////////////////////////////////////////////////////////////////////////////

void WxMenuBar::init()
{
  Guard guard ( this->mutex() );
  BaseClass::init();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find the button corresponding to the event.
//
///////////////////////////////////////////////////////////////////////////////

AFW::Menus::Button *WxMenuBar::findButton ( const wxCommandEvent &event )
{
  Guard guard ( this->mutex() );

  // Get the menu bar.
  wxMenuBar *bar ( this->get() );
  if ( 0x0 == bar )
    return 0x0;

  // Find the menu item.
  wxMenuItem *item ( bar->FindItem ( event.GetId() ) );

  // Return the button.
  AFW::Menus::Button::RefPtr button ( WxObjectMap::find<AFW::Menus::Button> ( item ) );
  return button.get();
}
