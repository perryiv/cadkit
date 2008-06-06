
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The wxWindows internal main window class.
//
///////////////////////////////////////////////////////////////////////////////

#include "WxPrecompiled.h"
#include "WxInternalMainWin.h"
#include "WxMainWindow.h"
#include "WxApplication.h"
#include "WxObjectMap.h"
#include "WxIcon.h"
#include "WxEventHandler.h"
#include "AppFrameWork/Core/Define.h"

#include "Usul/Bits/Bits.h"
#include "Usul/Errors/Assert.h"

IMPLEMENT_CLASS ( WxInternalMainWin, WxInternalMainWin::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

WxInternalMainWin::WxInternalMainWin ( WxMainWindow *mw, const wxPoint &origin, const wxSize &size ) : 
  BaseClass ( 0x0, wxID_ANY, 
              ( ( mw ) ? mw->title().c_str() : "Default Title" ), origin, size,
              wxDEFAULT_FRAME_STYLE | wxFRAME_NO_WINDOW_MENU )
{
  // Add us to the map.
  WxObjectMap::set ( this, mw );

  // Register as top window.
  if ( wxTheApp )
    wxTheApp->SetTopWindow ( this );

  // Load the icon.
  this->SetIcon ( WxIcon::load ( mw ) );

  // Attach special event handler.
  this->Connect ( wxID_ANY, wxEVT_CLOSE_WINDOW, wxCloseEventHandler ( WxInternalMainWin::_onClose ) );

  // Add another event handler.
  this->PushEventHandler ( new WxEventHandler );
  this->GetClientWindow()->PushEventHandler ( new WxEventHandler );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

WxInternalMainWin::~WxInternalMainWin()
{
  try
  {
    // Remove us from the map.
    WxObjectMap::remove ( this );

    // Remove anything else that may be deleted, or will be soon.
    WxObjectMap::remove ( this->GetMenuBar() );
    WxObjectMap::remove ( this->GetStatusBar() );
    WxObjectMap::remove ( this->GetToolBar() );

    // This is probably not necessary.
    this->DestroyChildren();

    // No more top-level window.
    if ( wxTheApp )
      wxTheApp->SetTopWindow ( 0x0 );
  }

  AFW_CATCH_BLOCK ( 1007659073ul );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the frame is about to close.
//
///////////////////////////////////////////////////////////////////////////////

void WxInternalMainWin::_onClose ( wxCloseEvent &event )
{
  // Get main window.
  WxMainWindow::RefPtr mw ( WxObjectMap::find<WxMainWindow> ( this ) );

  // If we still have a main window.
  if ( mw.valid() )
  {
    // Write the configuration.
    mw->configWrite();

    // Delete the menu bar.
    mw->menuBar ( 0x0 );
  }

  // Pass the event along.
  event.Skip();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when things need to be updated.
//
///////////////////////////////////////////////////////////////////////////////

void WxInternalMainWin::UpdateWindowUI ( long flags )
{
  flags = Usul::Bits::add ( flags, wxUPDATE_UI_RECURSE );
  BaseClass::UpdateWindowUI ( flags );
}
