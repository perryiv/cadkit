
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

#ifndef _WX_GUI_INTERNAL_MAIN_WINDOW_CLASS_H_
#define _WX_GUI_INTERNAL_MAIN_WINDOW_CLASS_H_

#include "AppFrameWork/Wx/CompileGuard.h"

class wxPoint;
class wxSize;
class wxCommandEvent;
class WxMainWindow;


class WxInternalMainWin : public wxMDIParentFrame
{
public:

  typedef wxMDIParentFrame BaseClass;

  WxInternalMainWin ( WxMainWindow *, const wxPoint &, const wxSize & );
  virtual ~WxInternalMainWin();

  virtual void    UpdateWindowUI ( long flags = wxUPDATE_UI_NONE );

protected:

  void            _onClose ( wxCloseEvent &event );

private:

  DECLARE_CLASS ( WxInternalMainWin );
};


#endif // _WX_GUI_INTERNAL_APPLICATION_CLASS_H_
