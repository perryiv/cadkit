
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Application class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Qt/Core/Application.h"

#include "Usul/Exceptions/Canceled.h"
#include "Usul/Trace/Trace.h"

#include <iostream>
#include <sstream>

using namespace CadKit::Helios::Core;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Application::Application ( int &argc, char **argv ) : BaseClass ( argc, argv )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Application::~Application()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notification of every event.
//
///////////////////////////////////////////////////////////////////////////////

bool Application::notify ( QObject *object, QEvent *event )
{
  USUL_TRACE_SCOPE;
  try
  {
    return BaseClass::notify ( object, event );
  }
  catch ( const Usul::Exceptions::Canceled & )
  {
    #ifdef _DEBUG
    std::cout << "Message 3196212174: Operation cancelled by user" << std::endl;
    #endif
  }
  catch ( const std::exception &e )
  {
    std::ostringstream out;
    out << "Error 8033290760: " << e.what() << '\n';
    std::cout << out.str() << std::flush;
  }
  catch ( ... )
  {
    std::ostringstream out;
    out << "Error 9522173940: exception generated when processing event\n";
    std::cout << out.str() << std::flush;
  }
  return false;
}
