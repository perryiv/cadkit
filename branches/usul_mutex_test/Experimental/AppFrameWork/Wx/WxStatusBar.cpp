
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The wxWindows status bar class.
//
///////////////////////////////////////////////////////////////////////////////

#include "WxPrecompiled.h"
#include "WxStatusBar.h"
#include "WxObjectMap.h"
#include "WxMainWindow.h"

#include "Usul/Errors/Assert.h"

AFW_IMPLEMENT_OBJECT ( WxStatusBar );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

WxStatusBar::WxStatusBar() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

WxStatusBar::~WxStatusBar()
{
  // Safely...
  try
  {
    // Need local scope.
    {
      // If there is a status bar then delete it.
      wxFrame *frame ( wxDynamicCast ( wxTheApp->GetTopWindow(), wxFrame ) );
      wxStatusBar *statusBar ( ( frame ) ? frame->GetStatusBar() : 0x0 );
      if ( frame && statusBar )
      {
        // Make sure the frame does not have a status bar.
        frame->SetStatusBar ( 0x0 );

        // Remove it from the map.
        WxObjectMap::remove ( statusBar );

        // Destroy the status bar.
        statusBar->Destroy();

        // Update the main window.
        frame->Refresh();
      }
    }

    // Need local scope.
    {
      // Get our status bar and delete it.
      std::auto_ptr<wxStatusBar> bar ( WxObjectMap::find<wxStatusBar> ( this ) );

      // Remove us from the map.
      WxObjectMap::remove ( this );

      // The wxStatusBar is about to die and it will delete it's children.
      std::for_each ( this->begin(), this->end(), std::mem_fun ( AFW::Core::Object::detach ) );

      // Remove all children.
      this->removeAll();
    }
  }

  // Catch all exceptions.
  AFW_CATCH_BLOCK ( 2299288050ul );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the existing status bar.
//
///////////////////////////////////////////////////////////////////////////////

wxStatusBar *WxStatusBar::get()
{
  Guard guard ( this->mutex() );
  return ( WxObjectMap::find<wxStatusBar> ( this ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the existing status bar.
//
///////////////////////////////////////////////////////////////////////////////

const wxStatusBar *WxStatusBar::get() const
{
  Guard guard ( this->mutex() );
  return ( WxObjectMap::find<wxStatusBar> ( this ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Detach from gui object.
//
///////////////////////////////////////////////////////////////////////////////

void WxStatusBar::detach()
{
  WxObjectMap::remove ( this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the status bar.
//
///////////////////////////////////////////////////////////////////////////////

bool WxStatusBar::create ( AFW::Windows::Window *mw )
{
  Guard guard ( this->mutex() );

  // Get the frame.
  wxFrame *frame ( WxObjectMap::find<wxFrame> ( mw ) );
  if ( 0x0 == frame )
    return false;

  // Don't create twice.
  if ( 0x0 != frame->GetStatusBar() || 0x0 != this->get() )
    return false;

  // Ask frame to create the status bar.
  frame->CreateStatusBar();

  // Set our window in the map.
  WxObjectMap::set ( this, frame->GetStatusBar() );

  // It worked.
  return true;
}

