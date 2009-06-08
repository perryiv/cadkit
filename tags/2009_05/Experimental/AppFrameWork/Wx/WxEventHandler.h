
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

#ifndef _WX_GUI_EVENT_HANDLER_CLASS_H_
#define _WX_GUI_EVENT_HANDLER_CLASS_H_

#include "AppFrameWork/Wx/CompileGuard.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Macro to help define member functions.
//
///////////////////////////////////////////////////////////////////////////////

#define DEFINE_MEMBER_FUNCTION(event_type)\
void _##event_type##Handler ( event_type & )


///////////////////////////////////////////////////////////////////////////////
//
//  The event handler class.
//
///////////////////////////////////////////////////////////////////////////////

class WxEventHandler : public wxEvtHandler
{
public:

  typedef wxEvtHandler BaseClass;

  WxEventHandler();
  virtual ~WxEventHandler();

protected:

  DEFINE_MEMBER_FUNCTION ( wxCommandEvent );
  DEFINE_MEMBER_FUNCTION ( wxScrollWinEvent );
  DEFINE_MEMBER_FUNCTION ( wxMouseEvent );
  DEFINE_MEMBER_FUNCTION ( wxSetCursorEvent );
  DEFINE_MEMBER_FUNCTION ( wxKeyEvent );
  DEFINE_MEMBER_FUNCTION ( wxSizeEvent );
  DEFINE_MEMBER_FUNCTION ( wxMoveEvent );
  DEFINE_MEMBER_FUNCTION ( wxPaintEvent );
  DEFINE_MEMBER_FUNCTION ( wxEraseEvent );
  DEFINE_MEMBER_FUNCTION ( wxFocusEvent );
  DEFINE_MEMBER_FUNCTION ( wxActivateEvent );
  DEFINE_MEMBER_FUNCTION ( wxMenuEvent );
  DEFINE_MEMBER_FUNCTION ( wxCloseEvent );
  DEFINE_MEMBER_FUNCTION ( wxJoystickEvent );
  DEFINE_MEMBER_FUNCTION ( wxDropFilesEvent );
  DEFINE_MEMBER_FUNCTION ( wxNavigationKeyEvent );
  DEFINE_MEMBER_FUNCTION ( wxIdleEvent );
  DEFINE_MEMBER_FUNCTION ( wxWindowCreateEvent );
  DEFINE_MEMBER_FUNCTION ( wxWindowDestroyEvent );
  DEFINE_MEMBER_FUNCTION ( wxUpdateUIEvent );
  DEFINE_MEMBER_FUNCTION ( wxIFMDockEvent );
  DEFINE_MEMBER_FUNCTION ( wxIFMUndockEvent );
  DEFINE_MEMBER_FUNCTION ( wxIFMFloatEvent );

  void        _common ( wxEvent &event );

  DECLARE_EVENT_TABLE();
  DECLARE_DYNAMIC_CLASS ( WxEventHandler );
};


#undef DEFINE_MEMBER_FUNCTION


#endif // _WX_GUI_EVENT_HANDLER_CLASS_H_
