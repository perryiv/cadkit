
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

#ifndef _WX_GUI_STATUS_BAR_CLASS_H_
#define _WX_GUI_STATUS_BAR_CLASS_H_

#include "AppFrameWork/Wx/CompileGuard.h"
#include "AppFrameWork/Core/StatusBar.h"

class WxMainWindow;
class wxStatusBar;


class WxStatusBar : public AFW::Core::StatusBar
{
public:

  // Typedefs.
  typedef AFW::Core::StatusBar BaseClass;

  // Create the internal status bar.
  wxStatusBar *                       create ( WxMainWindow * );

  // Detach from gui object.
  virtual void                        detach();

  // Get the status bar.
  const wxStatusBar *                 get() const;
  wxStatusBar *                       get();

protected:

  // Default construction.
  WxStatusBar();

  // Use reference counting.
  virtual ~WxStatusBar();

private:

  // Do not copy.
  WxStatusBar ( const WxStatusBar & );
  WxStatusBar &operator = ( const WxStatusBar & );

  AFW_DECLARE_OBJECT ( WxStatusBar );
};


#endif // _WX_GUI_STATUS_BAR_CLASS_H_
