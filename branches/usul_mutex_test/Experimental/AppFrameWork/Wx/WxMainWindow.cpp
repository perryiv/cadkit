
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
#include "WxBitmap.h"
#include "WxEventHandler.h"

#include "AppFrameWork/Core/Program.h"
#include "AppFrameWork/Core/Registry.h"

#include "Usul/Errors/Assert.h"
#include "Usul/System/Screen.h"

AFW_IMPLEMENT_OBJECT ( WxMainWindow );


///////////////////////////////////////////////////////////////////////////////
//
//  Possible dock sites.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  typedef std::map < AFW::Core::DockSite::Type, int > Sites;
  Sites sites;
  struct InitDockSites
  {
    InitDockSites()
    {
      sites[AFW::Core::DockSite::TOP]    = IFM_ORIENTATION_TOP;
      sites[AFW::Core::DockSite::BOTTOM] = IFM_ORIENTATION_BOTTOM;
      sites[AFW::Core::DockSite::LEFT]   = IFM_ORIENTATION_LEFT;
      sites[AFW::Core::DockSite::RIGHT]  = IFM_ORIENTATION_RIGHT;
      sites[AFW::Core::DockSite::NONE]   = IFM_ORIENTATION_FLOAT;
    }
  } _initDockSites;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

WxMainWindow::WxMainWindow() : BaseClass(),
  _ifm ( 0x0 )
{
  // Should be true.
  USUL_ASSERT ( 0x0 != wxTheApp );

  // Get initial configuration.
  wxPoint origin ( 10, 10 );
  wxSize size ( 400, 300 );
  bool maximized ( false );
  this->_configRead ( origin, size, maximized );

  // Make the frame.
  std::auto_ptr<WxInternalMainWin> mw ( new WxInternalMainWin ( this, origin, size ) );

  // Add us to the map.
  WxObjectMap::set ( this, mw.get() );

  // Show it and keep it.
  mw->Show ( true );

  // Call this after we show. See wxTopLevelWindowMSW::Maximize()'s 
  // implementation to read about why. Also, this order does not mess up the 
  // "restore" rectangle passed to WxInternalMainWin's constructor.
  if ( maximized )
    mw->Maximize ( true );

  // Create and initialize the interface management object.
  _ifm = new wxInterfaceManager ( mw.get(), mw->GetClientWindow() );
  _ifm->Initialize ( true );

  // Release.
  mw.release();
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
    // Delete the interface management object first.
    delete _ifm; _ifm = 0x0;

    // Get our frame and delete it.
    std::auto_ptr<wxFrame> frame ( WxObjectMap::find<wxFrame> ( this ) );

    // Remove us from the map.
    WxObjectMap::remove ( this );

    // The wxMenu is about to die and it will delete the sub-menus and buttons.
    std::for_each ( this->begin(), this->end(), std::mem_fun ( AFW::Core::Object::detach ) );

    // Remove all children.
    this->removeAll();
  }

  AFW_CATCH_BLOCK ( 3284071988ul );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the configuration.
//
///////////////////////////////////////////////////////////////////////////////

void WxMainWindow::configWrite() const
{
  Guard guard ( this->mutex() );
  BaseClass::configWrite();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initial rectangle.
//
///////////////////////////////////////////////////////////////////////////////

void WxMainWindow::_configRead ( wxPoint &origin, wxSize &size, bool &maximized )
{
  // Set this now.
  maximized = false;

  // Get dimensions of screen.
  const long sw ( static_cast < long > ( Usul::System::Screen::width() ) );
  const long sh ( static_cast < long > ( Usul::System::Screen::height() ) );

  // Default sizes. (They are correct!)
  long x ( 0.1 * sw );
  long y ( 0.1 * sh );
  long w ( 0.8 * sw );
  long h ( 0.8 * sh );

  // Save default.
  origin = wxPoint ( x, y );
  size   = wxSize  ( w, h );

  // If the section name is empty then return default.
  const std::string section ( this->persistentName() );
  if ( section.empty() )
    return;

  // Get dimensions of main window.
  AFW::Core::Application::RefPtr app ( AFW::Core::Program::instance().app() );
  AFW::Core::Registry::RefPtr reg ( ( app.valid() ) ? app->registry() : 0x0 );
  if ( reg.valid() )
  {
    x = reg->readValue ( section, AFW::Registry::Keys::X,      x );
    y = reg->readValue ( section, AFW::Registry::Keys::Y,      y );
    w = reg->readValue ( section, AFW::Registry::Keys::WIDTH,  w );
    h = reg->readValue ( section, AFW::Registry::Keys::HEIGHT, h );
  }

  // Make sure it fits.
  const bool xFits ( ( x > 0  ) && ( x < ( sw - w ) ) );
  const bool yFits ( ( y > 10 ) && ( y < ( sh - h ) ) );
  const bool wFits ( w < sw );
  const bool hFits ( h < sh );
  const bool fits ( xFits && yFits && wFits && hFits );

  // Set origin and size if it fits.
  if ( fits )
  {
    origin = wxPoint ( x, y );
    size   = wxSize  ( w, h );
  }

  // Is it maximized?
  if ( reg.valid() )
    maximized = reg->readValue ( section, AFW::Registry::Keys::MAXIMIZED, maximized );
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

void WxMainWindow::statusBar ( AFW::Windows::StatusBar *sb )
{
  Guard guard ( this->mutex() );

  // Handle same status bar.
  if ( this->statusBar() == sb )
    return;

  // Tell the interface management object.
  if ( _ifm )
    _ifm->SetStatusMessagePane ( 0x0, IFM_DISABLE_STATUS_MESSAGES );

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
  bar->create ( this );
  wxStatusBar *status ( bar->get() );

  // Should be true.
  USUL_ASSERT ( status == frame->GetStatusBar() );

  // Tell the interface management object about the new status bar.
  if ( _ifm )
    _ifm->SetStatusMessagePane ( frame->GetStatusBar(), 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Insert the window.
//
///////////////////////////////////////////////////////////////////////////////

bool WxMainWindow::insert ( AFW::Windows::Group::Itr where, AFW::Windows::Window *w )
{
  Guard guard ( this->mutex() );

  // Call base class first.
  if ( false == BaseClass::insert ( where, w ) )
    return false;

  // Ignore a few special cases.
  if ( dynamic_cast < AFW::Windows::StatusBar * > ( w ) ||
       dynamic_cast < AFW::Menus::MenuBar *  > ( w ) )
    return true; // Yes, true.

  // Get the window.
  wxWindow *window ( WxObjectMap::find<wxWindow> ( w ) );
  if ( 0x0 == window )
    return true; // Yes, true.

  // Add the window to the interface manager.
  wxIFMDefaultChildData data;
  data.m_name = wxString ( w->title().c_str() );
  data.m_type = IFM_CHILD_GENERIC;
  data.m_desiredSize.Set ( 200, 150 );  // Do these do anything?
  data.m_minSize.Set ( 80, 60 );        // What should this be?
  data.m_child = window;
  data.m_hideable = true;
  data.m_tabify = true;

  // Set bitmap.
  wxBitmap bitmap ( WxBitmap::load ( w->icon().file() ) );
  data.m_bitmap = ( ( bitmap.Ok() ) ? bitmap : wxNullBitmap );

  // Set dock site.
  const AFW::Core::DockSite::Type site ( w->dockState().first );
  data.m_orientation = Detail::sites[site];

  // Add it.
  _ifm->AddChild ( &data );

  // Add event handler to the internal window just created.
  //_ifm->GetContentWindow()->PushEventHandler ( new WxEventHandler );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get window size.
//
///////////////////////////////////////////////////////////////////////////////

AFW::Windows::Window::Size WxMainWindow::size() const
{
  Guard guard ( this->mutex() );
  wxFrame *frame ( WxObjectMap::find<wxFrame> ( this ) );
  if ( frame )
  {
    wxSize s ( frame->GetSize() );
    return ( AFW::Windows::Window::Size ( s.x, s.y ) );
  }
  return BaseClass::size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get window position.
//
///////////////////////////////////////////////////////////////////////////////

AFW::Windows::Window::Position WxMainWindow::position() const
{
  Guard guard ( this->mutex() );
  wxFrame *frame ( WxObjectMap::find<wxFrame> ( this ) );
  if ( frame )
  {
    wxPoint p ( frame->GetPosition() );
    return AFW::Windows::Window::Position ( p.x, p.y );
  }
  return BaseClass::Position();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set window size.
//
///////////////////////////////////////////////////////////////////////////////

void WxMainWindow::size ( const AFW::Windows::Window::Size &s )
{
  Guard guard ( this->mutex() );
  wxFrame *frame ( WxObjectMap::find<wxFrame> ( this ) );
  if ( frame )
  {
    frame->SetSize ( wxSize ( s[0], s[1] ) );
    return;
  }
  BaseClass::size ( s );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set window position.
//
///////////////////////////////////////////////////////////////////////////////

void WxMainWindow::position ( const AFW::Windows::Window::Position &p )
{
  Guard guard ( this->mutex() );
  wxFrame *frame ( WxObjectMap::find<wxFrame> ( this ) );
  if ( frame )
  {
    frame->SetPosition ( wxPoint ( p[0], p[1] ) );
    return;
  }
  BaseClass::position ( p );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the frame maximized?
//
///////////////////////////////////////////////////////////////////////////////

bool WxMainWindow::maximized() const
{
  Guard guard ( this->mutex() );
  wxFrame *frame ( WxObjectMap::find<wxFrame> ( this ) );
  return ( ( frame ) ? frame->IsMaximized() : false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the frame's maximized state
//
///////////////////////////////////////////////////////////////////////////////

void WxMainWindow::maximized ( bool state )
{
  Guard guard ( this->mutex() );
  wxFrame *frame ( WxObjectMap::find<wxFrame> ( this ) );
  if ( frame )
    frame->Maximize ( state );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the visible state.
//
///////////////////////////////////////////////////////////////////////////////

void WxMainWindow::visible ( Window *w, bool state )
{
  Guard guard ( this->mutex() );
  wxWindow *window ( WxObjectMap::find<wxWindow> ( w ) );
  if ( ( 0x0 != window ) && ( this->end() != this->find ( w ) ) )
    _ifm->ShowChild ( window, state );
  BaseClass::visible ( w, state );
}
