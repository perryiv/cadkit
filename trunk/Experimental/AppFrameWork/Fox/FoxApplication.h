
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The application class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FOX_GUI_SERVER_APPLICATION_CLASS_H_
#define _FOX_GUI_SERVER_APPLICATION_CLASS_H_

#include "AppFrameWork/Fox/CompileGuard.h"
#include "AppFrameWork/Core/Application.h"

namespace FoxTools { namespace App { class Application; } }


class FoxApplication : public AFW::Core::Application
{
public:

  // Typedefs.
  typedef AFW::Core::Object BaseClass;

  // Destroy the application.
  virtual void                        destroy();

  // Run the application.
  virtual void                        run ( AFW::Core::Application::RunCommand );

protected: 

  // Default construction.
  FoxApplication();

  // Use reference counting.
  virtual ~FoxApplication();

  void                                _build();

  void                                _runMainEventLoop();
  void                                _runWhileEvents();

private:

  // Do not copy.
  FoxApplication ( const FoxApplication & );
  FoxApplication &operator = ( const FoxApplication & );

  FoxTools::App::Application *_app;

  AFW_DECLARE_OBJECT ( FoxApplication );
};


#endif // _FOX_GUI_SERVER_APPLICATION_CLASS_H_
