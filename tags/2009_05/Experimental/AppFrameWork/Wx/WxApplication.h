
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The wxWindows application class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GUI_APPLICATION_CLASS_H_
#define _WX_GUI_APPLICATION_CLASS_H_

#include "AppFrameWork/Wx/CompileGuard.h"
#include "AppFrameWork/Core/Application.h"

class wxDocManager;


class WxApplication : public AFW::Core::Application
{
public:

  // Typedefs.
  typedef AFW::Core::Application BaseClass;

  // There should only be one instance. Return it.
  static WxApplication *              get();

  // Run the application.
  virtual void                        run ( RunCommand );

  // Quit the application.
  virtual void                        quit();

protected:

  // Default construction.
  WxApplication();

  // Use reference counting.
  virtual ~WxApplication();

  void                                _splashScreen();

private:

  // Do not copy.
  WxApplication ( const WxApplication & );
  WxApplication &operator = ( const WxApplication & );

  AFW_DECLARE_OBJECT ( WxApplication );
};


#endif // _WX_GUI_APPLICATION_CLASS_H_
