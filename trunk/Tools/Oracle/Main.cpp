
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Program for executing oracle sql commands.
//
///////////////////////////////////////////////////////////////////////////////

#include "Database/Oracle/Execute.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Exceptions/Exception.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Strings/Format.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Main function.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
  typedef CadKit::Database::Oracle::Execute Execute;

  int result ( -1 );
  Execute program ( argc, argv );

  USUL_TRY_BLOCK
  {
    program.run();
    result = 0;
  }

  catch ( const Execute::Usage & )
  {
    Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( &program, &Execute::writeUsage ), "2462809390" );
  }

  catch ( const Usul::Exceptions::Exception &e )
  {
    const std::string message ( Usul::Strings::format ( "Error 1277129746: Standard exception caught. ", ( ( 0x0 == e.what() ) ? "" : e.what() ), "\nStack Trace:\n", e.stackTrace() ) );
    Usul::Functions::safeCallR1 ( Usul::Adaptors::memberFunction ( &program, &Execute::writeError ), message, "3951800720" );
  }

  catch ( const std::exception &e )
  {
    const std::string message ( Usul::Strings::format ( "Error 3946572659: Standard exception caught. ", ( ( 0x0 == e.what() ) ? "" : e.what() ) ) );
    Usul::Functions::safeCallR1 ( Usul::Adaptors::memberFunction ( &program, &Execute::writeError ), message, "3951800720" );
  }

  catch ( ... )
  {
    const std::string message ( "Error 2699001256: Unknown exception caught" );
    Usul::Functions::safeCallR1 ( Usul::Adaptors::memberFunction ( &program, &Execute::writeError ), message, "4032328237" );
  }

  return result;
}
