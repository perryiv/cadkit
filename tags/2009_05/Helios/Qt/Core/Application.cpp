
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
#include "Usul/Strings/Format.h"
#include "Usul/System/LastError.h"
#include "Usul/Trace/Trace.h"

#include <iostream>

#ifdef _MSC_VER
# define NOMINMAX
# define WIN32_LEAN_AND_MEAN
# include "windows.h"
# include "excpt.h"
#endif

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
//  Helper function to handle Windows structured exception.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER

namespace Helper
{
  int handleStructuredException ( DWORD code )
  {
    const std::string message ( Usul::System::LastError::message ( code ) );
    std::cout << Usul::Strings::format ( 
      "Error 2917508413: structured exception ", code, " generated when processing event.", 
      ( ( false == message.empty() ) ? message : "" ) ) << std::endl;
    return EXCEPTION_EXECUTE_HANDLER;
  }
}

#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Notification of every event.
//
///////////////////////////////////////////////////////////////////////////////

bool Application::notify ( QObject *object, QEvent *event )
{
  USUL_TRACE_SCOPE;

  #ifdef _MSC_VER

    __try 
    {
      return this->_notify ( object, event );
    }

    __except ( Helper::handleStructuredException ( ::GetExceptionCode() ) )
    {
      return false;
    }

  #else

    return this->_notify ( object, event );

  #endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notification of every event.
//
///////////////////////////////////////////////////////////////////////////////

bool Application::_notify ( QObject *object, QEvent *event )
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
    std::cout << Usul::Strings::format ( "Error 8033290760: ", e.what() ) << std::endl;
  }
  catch ( ... )
  {
    std::cout << Usul::Strings::format ( "Error 9522173940: exception generated when processing event" ) << std::endl;
  }
  return false;
}
