
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The wxWindows internal application class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GUI_INTERNAL_APPLICATION_CLASS_H_
#define _WX_GUI_INTERNAL_APPLICATION_CLASS_H_

#include "AppFrameWork/Wx/CompileGuard.h"


class WxInternalApp : public wxApp
{
public:

  typedef wxApp BaseClass;

  WxInternalApp();
  virtual ~WxInternalApp();

protected:

  virtual void                        HandleEvent ( wxEvtHandler *, wxEventFunction, wxEvent & ) const;

  virtual bool                        OnInit();
  virtual int                         OnExit();

  virtual bool                        ProcessIdle();
};


DECLARE_APP ( WxInternalApp );


#endif // _WX_GUI_INTERNAL_APPLICATION_CLASS_H_
