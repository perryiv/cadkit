
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The wxWindows main window class.
//
///////////////////////////////////////////////////////////////////////////////

#include "WxPrecompiled.h"
#include "WxMainWindow.h"
#include "WxApplication.h"
#include "WxObjectMap.h"
#include "WxInternalMainWin.h"
#include "WxMenuBar.h"
#include "WxStatusBar.h"

#include "Usul/Errors/Assert.h"
#include "Usul/System/Screen.h"

AFW_IMPLEMENT_OBJECT ( WxMainWindow );


///////////////////////////////////////////////////////////////////////////////
//
//  Local typedefs.
//
///////////////////////////////////////////////////////////////////////////////

typedef std::pair < wxPoint, wxSize > WxRect;


///////////////////////////////////////////////////////////////////////////////
//
//  Initial rectangle.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  WxRect initialRect ( const std::string &name )
  {
    // Get dimensions of screen.
    const long sw ( static_cast < long > ( Usul::System::Screen::width() ) );
    const long sh ( static_cast < long > ( Usul::System::Screen::height() ) );

    // Default sizes. (They are correct!)
    long x ( 0.1 * sw );
    long y ( 0.1 * sh );
    long w ( 0.8 * sw );
    long h ( 0.8 * sh );

    // Save default.
    WxRect defaultRect ( wxPoint ( x, y ), wxSize ( w, h ) );

    // If the section name is empty then return default.
    if ( name.empty() )
      return defaultRect;

    // Make configuration.
    std::auto_ptr<wxConfig> config ( new wxConfig ( name.c_str() ) );

    // Get dimensions of main window.
    if ( config.get() )
    {
      config->Read ( AFW::Registry::Keys::X.c_str(),      &x );
      config->Read ( AFW::Registry::Keys::Y.c_str(),      &y );
      config->Read ( AFW::Registry::Keys::WIDTH.c_str(),  &w );
      config->Read ( AFW::Registry::Keys::HEIGHT.c_str(), &h );
    }

    // Make sure it fits.
    const bool xFits ( ( x > 0  ) && ( x < ( sw - w ) ) );
    const bool yFits ( ( y > 10 ) && ( y < ( sh - h ) ) );
    const bool wFits ( w < sw );
    const bool hFits ( h < sh );
    const bool fits ( xFits && yFits && wFits && hFits );

    // Return proper rectangle.
    if ( fits )
      return WxRect ( wxPoint ( x, y ), wxSize ( w, h ) );
    else
      return defaultRect;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

WxMainWindow::WxMainWindow() : BaseClass()
{
  // Should be true.
  USUL_ASSERT ( 0x0 != wxTheApp );

  // Make the frame.
  WxRect rect ( Detail::initialRect ( this->persistentName() ) );
  std::auto_ptr<WxInternalMainWin> frame ( new WxInternalMainWin ( this, rect.first, rect.second ) );

  // Add us to the map.
  WxObjectMap::set ( this, frame.get() );

  // Show it.
  frame->Show ( true );

  // Keep it.
  frame.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

WxMainWindow::~WxMainWindow()
{
  try
  {
    // Get our frame and delete it.
    std::auto_ptr<wxFrame> frame ( WxObjectMap::find<wxFrame> ( this ) );

    // Remove us from the map.
    WxObjectMap::remove ( this );

    // The wxMenu is about to die and it will delete the sub-menus and buttons.
    std::for_each ( this->begin(), this->end(), std::mem_fun ( AFW::Core::Object::detach ) );

    // Remove all children.
    this->removeAll();
  }

  AFW_CATCH_BLOCK ( "3284071988", "1224275350" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the configuration.
//
///////////////////////////////////////////////////////////////////////////////

void WxMainWindow::writeConfig()
{
  Guard guard ( this->mutex() );

  // Get our frame.
  wxFrame *frame ( WxObjectMap::find<wxFrame> ( this ) );

  // Handle no frame.
  if ( 0x0 == frame )
    return;

  // Make configuration.
  std::auto_ptr<wxConfig> config ( new wxConfig ( this->persistentName().c_str() ) );
  if ( 0x0 == config.get() )
    return;

  // Get the origin and size.
  wxPoint origin ( frame->GetPosition() );
  wxSize size    ( frame->GetSize() );

  // Write the window rectangle.
  config->Write ( AFW::Registry::Keys::X.c_str(),      origin.x );
  config->Write ( AFW::Registry::Keys::Y.c_str(),      origin.y );
  config->Write ( AFW::Registry::Keys::WIDTH.c_str(),  size.x   );
  config->Write ( AFW::Registry::Keys::HEIGHT.c_str(), size.y   );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the menu bar.
//
///////////////////////////////////////////////////////////////////////////////

WxMenuBar *WxMainWindow::menuBar()
{
  Guard guard ( this->mutex() );
  return ( dynamic_cast < WxMenuBar * > ( BaseClass::menuBar() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the menu bar.
//
///////////////////////////////////////////////////////////////////////////////

const WxMenuBar *WxMainWindow::menuBar() const
{
  Guard guard ( this->mutex() );
  return ( dynamic_cast < const WxMenuBar * > ( BaseClass::menuBar() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the menu bar.
//
///////////////////////////////////////////////////////////////////////////////

void WxMainWindow::menuBar ( AFW::Menus::MenuBar *m )
{
  Guard guard ( this->mutex() );

  // Handle same menu bar.
  if ( this->menuBar() == m )
    return;

  // Call base class first.
  BaseClass::menuBar ( m );

  // Get the menu bar, which may still be null, or not a WxMenuBar.
  WxMenuBar::RefPtr bar ( this->menuBar() ) ;
  if ( false == bar.valid() )
    return;

  // Get our frame.
  wxFrame *frame ( WxObjectMap::find<wxFrame> ( this ) );

  // Set the frame's menu bar.
  if ( frame )
  {
    USUL_ASSERT ( 0x0 == frame->GetMenuBar() );
    frame->SetMenuBar ( bar->get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Detach from gui object.
//
///////////////////////////////////////////////////////////////////////////////

void WxMainWindow::detach()
{
  WxObjectMap::remove ( this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the status bar.
//
///////////////////////////////////////////////////////////////////////////////

WxStatusBar *WxMainWindow::statusBar()
{
  Guard guard ( this->mutex() );
  return ( dynamic_cast < WxStatusBar * > ( BaseClass::statusBar() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the status bar.
//
///////////////////////////////////////////////////////////////////////////////

const WxStatusBar *WxMainWindow::statusBar() const
{
  Guard guard ( this->mutex() );
  return ( dynamic_cast < const WxStatusBar * > ( BaseClass::statusBar() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the status bar.
//
///////////////////////////////////////////////////////////////////////////////

void WxMainWindow::statusBar ( AFW::Core::StatusBar *sb )
{
  Guard guard ( this->mutex() );

  // Handle same status bar.
  if ( this->statusBar() == sb )
    return;

  // Call base class first.
  BaseClass::statusBar ( sb );

  // Get our frame.
  wxFrame *frame ( WxObjectMap::find<wxFrame> ( this ) );
  if ( 0x0 == frame )
    return;

  // Get the status bar, which may still be null, or not a WxStatusBar.
  WxStatusBar::RefPtr bar ( this->statusBar() ) ;
  if ( false == bar.valid() )
    return;

  // Should be true.
  USUL_ASSERT ( 0x0 == frame->GetStatusBar() );

  // Create our status bar.
  wxStatusBar *status ( bar->create ( this ) );

  // Should be true.
  USUL_ASSERT ( status == frame->GetStatusBar() );
}
