
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The event handler.
//
///////////////////////////////////////////////////////////////////////////////

#include "WxPrecompiled.h"
#include "WxEventHandler.h"
#include "WxObjectMap.h"
#include "WxMenuBar.h"
#include "AppFrameWork/Windows/MainWindow.h"
#include "AppFrameWork/Menus/Button.h"

#include "Usul/System/DateTime.h"
#include "Usul/Threads/Variable.h"
#include "Usul/Threads/Guard.h"

IMPLEMENT_DYNAMIC_CLASS ( WxEventHandler, WxEventHandler::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Internal set of handlers. We need the handlers to out-live their 
//  respective windows, and we need one handler per window.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  typedef boost::shared_ptr < WxEventHandler > HandlerPtr;
  typedef std::set < HandlerPtr > HandlerSet;
  typedef Usul::Threads::Variable < HandlerSet > Handlers;
  typedef Usul::Threads::Guard < Handlers::MutexType > Guard;
  Handlers handlers;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Macro to help implement member functions.
//
///////////////////////////////////////////////////////////////////////////////

#define IMPLEMENT_MEMBER_FUNCTION(event_type)\
void WxEventHandler::_##event_type##Handler ( event_type &event )\
{\
  this->_common ( event );\
  event.Skip();\
}


///////////////////////////////////////////////////////////////////////////////
//
//  Macro to make the name of the appropriate member function.
//
///////////////////////////////////////////////////////////////////////////////

#define MEMBER_FUNCTION(event_type) WxEventHandler::_##event_type##Handler


///////////////////////////////////////////////////////////////////////////////
//
//  Event table.
//
///////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE ( WxEventHandler, WxEventHandler::BaseClass )
  EVT_MENU_RANGE      ( wxID_ANY, wxID_ANY, MEMBER_FUNCTION ( wxCommandEvent ) )
  EVT_WINDOW_CREATE   (                     MEMBER_FUNCTION ( wxWindowCreateEvent ) )
  EVT_WINDOW_DESTROY  (                     MEMBER_FUNCTION ( wxWindowDestroyEvent ) )
  EVT_UPDATE_UI_RANGE ( wxID_ANY, wxID_ANY, MEMBER_FUNCTION ( wxUpdateUIEvent ) )
  EVT_CLOSE           (                     MEMBER_FUNCTION ( wxCloseEvent ) )
  EVT_MENU_CLOSE      (                     MEMBER_FUNCTION ( wxMenuEvent ) )
  EVT_IFM_DOCK        (                     MEMBER_FUNCTION ( wxIFMDockEvent ) )
  EVT_IFM_UNDOCK      (                     MEMBER_FUNCTION ( wxIFMUndockEvent ) )
  EVT_IFM_FLOAT       (                     MEMBER_FUNCTION ( wxIFMFloatEvent ) )
END_EVENT_TABLE()


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

WxEventHandler::WxEventHandler() : BaseClass()
{
  // Add to global set.
  Detail::Guard ( Detail::handlers.mutex() );
  Detail::handlers.value().insert ( Detail::HandlerPtr ( this ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

WxEventHandler::~WxEventHandler()
{
  // Do not remove from set, the set is most likely calling this function!
}


///////////////////////////////////////////////////////////////////////////////
//
//  Member functions for each event type.
//
///////////////////////////////////////////////////////////////////////////////

IMPLEMENT_MEMBER_FUNCTION ( wxScrollWinEvent );
IMPLEMENT_MEMBER_FUNCTION ( wxMouseEvent );
IMPLEMENT_MEMBER_FUNCTION ( wxSetCursorEvent );
IMPLEMENT_MEMBER_FUNCTION ( wxKeyEvent );
IMPLEMENT_MEMBER_FUNCTION ( wxSizeEvent );
IMPLEMENT_MEMBER_FUNCTION ( wxMoveEvent );
IMPLEMENT_MEMBER_FUNCTION ( wxPaintEvent );
IMPLEMENT_MEMBER_FUNCTION ( wxEraseEvent );
IMPLEMENT_MEMBER_FUNCTION ( wxFocusEvent );
IMPLEMENT_MEMBER_FUNCTION ( wxActivateEvent );
IMPLEMENT_MEMBER_FUNCTION ( wxJoystickEvent );
IMPLEMENT_MEMBER_FUNCTION ( wxDropFilesEvent );
IMPLEMENT_MEMBER_FUNCTION ( wxNavigationKeyEvent );
IMPLEMENT_MEMBER_FUNCTION ( wxIdleEvent );
IMPLEMENT_MEMBER_FUNCTION ( wxWindowCreateEvent );
IMPLEMENT_MEMBER_FUNCTION ( wxMenuEvent );
IMPLEMENT_MEMBER_FUNCTION ( wxIFMDockEvent );
IMPLEMENT_MEMBER_FUNCTION ( wxIFMUndockEvent );
IMPLEMENT_MEMBER_FUNCTION ( wxIFMFloatEvent );


///////////////////////////////////////////////////////////////////////////////
//
//  Called when a window is being updated.
//
///////////////////////////////////////////////////////////////////////////////

void WxEventHandler::_wxUpdateUIEventHandler ( wxUpdateUIEvent &event )
{
  // Call the common function.
  //this->_common ( event ); // This prints all update events to the text windows!

  // Call update actions.
  {
    wxObject *object ( event.GetEventObject() );
    AFW::Windows::Window::RefPtr window ( WxObjectMap::find<AFW::Windows::Window> ( object ) );
    if ( window.valid() )
      window->callUpdateActions();
  }

  // See if this is a menu item.
  {
    wxMenuItem *item ( WxObjectMap::findMenuItem ( event.GetId() ) );
    AFW::Menus::Button::RefPtr button ( WxObjectMap::find<AFW::Menus::Button> ( item ) );
    if ( button.valid() )
      button->callUpdateActions();
  }

  // Not handled.
  event.Skip();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when a window is being closed.
//
///////////////////////////////////////////////////////////////////////////////

void WxEventHandler::_wxCloseEventHandler ( wxCloseEvent &event )
{
  // Call the common function.
  this->_common ( event );

  // Not handled.
  event.Skip();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when a window is being destroyed.
//
///////////////////////////////////////////////////////////////////////////////

void WxEventHandler::_wxWindowDestroyEventHandler ( wxWindowDestroyEvent &event )
{
  // Call the common function.
  this->_common ( event );

  // Remove this object from our map.
  WxObjectMap::remove ( event.GetEventObject() );

  // Not handled.
  event.Skip();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when a menu button is pressed.
//
///////////////////////////////////////////////////////////////////////////////

void WxEventHandler::_wxCommandEventHandler ( wxCommandEvent &event )
{
  // Call the common function.
  this->_common ( event );

  // Should be the main window.
  AFW::Windows::MainWindow::RefPtr mw ( WxObjectMap::find<AFW::Windows::MainWindow> ( event.GetEventObject() ) );
  if ( mw.valid() )
  {
    // Get the menu bar.
    WxMenuBar::RefPtr bar ( dynamic_cast < WxMenuBar * > ( mw->menuBar() ) );
    if ( bar.valid() )
    {
      // Find the button.
      AFW::Menus::Button::RefPtr button ( bar->findButton ( event ) );
      if ( button.valid() )
      {
        button->callCommandActions ( false );
      }
    }
  }

  // Not handled.
  event.Skip();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Member function for every event.
//
///////////////////////////////////////////////////////////////////////////////

void WxEventHandler::_common ( wxEvent &event )
{
#ifdef _DEBUG

  std::ostringstream local;
  local << "Event type: " << event.GetEventType() << ", ID: " << event.GetId();
  local << ", Time: " << Usul::System::DateTime::format ( event.GetTimestamp() );
  local << ", Name: " << event.GetClassInfo()->GetClassName();

  wxObject *object ( event.GetEventObject() );
  if ( object )
  {
    local << ", Address: " << object;
    local << ", Class Name: " << object->GetClassInfo()->GetClassName();
    {
      wxFrame *frame ( wxDynamicCast ( object, wxFrame ) );
      if ( frame )
      {
        local << ", Frame Label: " << frame->GetLabel();
        wxMenuBar *bar ( frame->GetMenuBar() );
        if ( bar )
        {
          wxMenuItem *item ( bar->FindItem ( event.GetId() ) );
          if ( item )
          {
            local << ", Menu Item Text: " << item->GetText();
          }
        }
      }
      else
      {
        wxWindow *window ( wxDynamicCast ( object, wxWindow ) );
        if ( window )
        {
          local << ", Object Label: " << window->GetLabel();
        }
        else
        {
          wxEvtHandler *handler ( wxDynamicCast ( object, wxEvtHandler ) );
          if ( handler )
          {
            local << ", wxEvtHandler";
          }
        }
      }
    }
  }

  local << std::endl;
  std::cout << local.str() << std::flush;

#ifdef _MSC_VER
  ::OutputDebugString ( local.str().c_str() );
#endif

#endif
}
