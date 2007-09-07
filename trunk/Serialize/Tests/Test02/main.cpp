
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Serialize/XML/Serialize.h"
#include "Serialize/XML/Deserialize.h"
#include "Serialize/XML/SimpleDataMember.h"
#include "Serialize/XML/DataMemberMap.h"
#include "Serialize/XML/Macros.h"

#include "Usul/Factory/RegisterCreator.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Threads/Mutex.h"

#include "Usul/Interfaces/IUnknown.h"

#include "Create.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Constants.
//
///////////////////////////////////////////////////////////////////////////////

const std::string FILE_NAME_1 ( "out_1.xml" );
const std::string FILE_NAME_2 ( "out_2.xml" );


///////////////////////////////////////////////////////////////////////////////
//
//  Set mutex factory.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Threads::SetMutexFactory factory ( &Usul::Threads::newSingleThreadedMutexStub );


///////////////////////////////////////////////////////////////////////////////
//
//  Run the test.
//
///////////////////////////////////////////////////////////////////////////////

void _run()
{
#if 0
  Usul::Interfaces::IUnknown::QueryPtr unknown ( create() );

  // Write objects to file.
  {
    std::cout << "Writing: " << FILE_NAME_1 << std::endl;
    Serialize::XML::serialize ( "test", unknown, FILE_NAME_1 );
  }

  // Read objects from first file and write to a second file.
  {
    Usul::Interfaces::IUnknown::QueryPtr unknown ( create() );
    std::cout << "Reading: " << FILE_NAME_1 << std::endl;
    Serialize::XML::deserialize ( FILE_NAME_1, unknown );
    std::cout << "Writing: " << FILE_NAME_2 << std::endl;
    Serialize::XML::serialize ( "test", unknown, FILE_NAME_2 );
  }

  // Compare contents of the two files.
  {
    std::string file1, file2;
    std::cout << "Reading: " << FILE_NAME_1 << std::endl;
    Usul::File::contents ( FILE_NAME_1, false, file1 );
    std::cout << "Reading: " << FILE_NAME_2 << std::endl;
    Usul::File::contents ( FILE_NAME_2, false, file2 );
    std::cout << "Comparing " << FILE_NAME_1 << " and " << FILE_NAME_2 << std::endl;
    USUL_ASSERT ( file1.compare ( file2 ) == 0 );
  }
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clean up.
//
///////////////////////////////////////////////////////////////////////////////

void _clean()
{
  Usul::Factory::ObjectFactory::instance ( 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Main function.
//
///////////////////////////////////////////////////////////////////////////////

int main( int argc, char** argv )
{
  Usul::Functions::safeCall ( _run,   "2314601630" );
  Usul::Functions::safeCall ( _clean, "3986609160" );

  return 0;
}
