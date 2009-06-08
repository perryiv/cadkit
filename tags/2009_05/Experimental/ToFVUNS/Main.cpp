
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Jeff Conner, Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Program to convert raw data to FieldView Unstructured format
//
///////////////////////////////////////////////////////////////////////////////

#include "ToFVUNS.h"

#include <iomanip>
#include <iostream>


///////////////////////////////////////////////////////////////////////////////
//
//  Main function.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
  try
  {
    // Single threaded execution.
    // Usul::Threads::Mutex::createFunction ( &Usul::Threads::newSingleThreadedMutexStub );

    // Declare and execute the program.
    ToFVUNS::RefPtr program ( new ToFVUNS ( argc, argv ) );
    program->run();

    // It worked.
    return 0;
  }
  catch ( const std::exception &e )
  {
    std::cout << ( ( 0x0 != e.what() ) ? e.what() : "Standard exception caught" ) << std::endl;
    return 1;
  }
  catch ( ... )
  {
    std::cout << "Unknown exception caught" << std::endl;
    return 1;
  }
}
