
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Custom startup sequence for wxWindows.
//
///////////////////////////////////////////////////////////////////////////////

#include "WxPrecompiled.h"
#include "WxInit.h"
#include "AppFrameWork/Core/Define.h"

#include "Usul/CommandLine/Arguments.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize wxWindows. We cannot use their default startup function, 
//  wxEntry(), because it continues right into the event-loop.
//
///////////////////////////////////////////////////////////////////////////////

void WxInit::init()
{
  // Get copy of command-line arguments, wxWindows will delete them.
  typedef Usul::CommandLine::Arguments Arguments;
  Arguments &arguments ( Arguments::instance() );
  int argc ( arguments.argc() );
  char **argv = arguments.copy();

  // This is vital. Without it, we get an infinite loop when we exit.
  ::wxSetInstance ( ::GetModuleHandle ( arguments.program().c_str() ) );

  // This makes image handlers other than BMP available.
  ::wxInitAllImageHandlers();

  // Library initialization.
  if ( false == ::wxEntryStart ( argc, argv ) )
    throw std::runtime_error ( "Error 1018307280: failed to initialize wxWindows library" );

  // Application initialization
  if ( false == wxTheApp->CallOnInit() )
    throw std::runtime_error ( "Error 1182127598: failed to initialize wxWindows application" );
}
