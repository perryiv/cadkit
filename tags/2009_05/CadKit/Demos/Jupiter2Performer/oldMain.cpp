///////////////////////////////////////////////////////////////////////////////
//
//  BSD License
//  http://www.opensource.org/licenses/bsd-license.html
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
//
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
//  - Neither the name of the CAD Toolkit nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A Jupiter to Performer translator.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
# pragma warning(disable:4251) // See note 3ce3a140-2edf-11d4-98a4-0040054c86c7.
# pragma warning(disable:4786) // Truncated debug names.
#endif

#include "Translators/Jupiter2Performer/TrJt2Pf.h"
#include "Translators/Jupiter2Performer/TrJt2PfPerformer.h"

#include "Standard/SlPathname.h"

#include <string>
#include <iostream>
#include <list>

using namespace CadKit;


///////////////////////////////////////////////////////////////////////////////
//
//  Translate the file.
//
///////////////////////////////////////////////////////////////////////////////

bool _translate ( std::string &filename, TrJt2Pf &jt2pf )
{
  SL_ASSERT ( false == filename.empty() );
  std::cout << "Translating: " << filename << std::endl;

  // Declare a Performer group node.
  SlRefPtr<pfGroup> root = new pfGroup;

  // Set the node's name to be the input file.
  root->setName ( filename.c_str() );

  // Translate.
  if ( false == jt2pf.translate ( filename.c_str(), *root ) )
  {
    std::cout << jt2pf.getLastError() << std::endl;
    std::cout << "Failed to translate: " << filename << std::endl;
    return false;
  }

  // The output filename should have the same path as the input filename, 
  // except with a different extension.
  SL_ASSERT ( filename.size() - 3 == filename.find ( ".jt" ) );
  filename.resize ( filename.size() - 2 );
  filename += "pfb";

  std::cout << "Writing:     " << filename << std::endl;

  // Write the Performer scene file.
  if ( 0 == ::pfdStoreFile ( root, filename.c_str() ) )
  {
    std::cout << "Failed to write: " << filename << std::endl;
    return false;
  }

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Main.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
  // There should be at least one argument.
  if ( argc < 2 )
  {
    std::cout << "Usage: " << CadKit::justFilename ( std::string ( argv[0] ) );
    std::cout << " [-v] <filename1.jt> [filename2.jt] ..." << std::endl;
    std::cout << "\t-v  Verbose output to stdout." << std::endl;
    return 0;
  }

  // Initialize the shared arenas. From the man pages, "However, a libpr
  // application that wishes to use Performer's arenas, must call 
  // pfInitArenas before calling pfInit to ensure that the type system
  // is created in shared memory." The man pages are not clear on what 
  // the return values mean.
  //::pfInitArenas();

  // Initialize Performer.
  int result = ::pfInit();
  if ( 1 != result )
  {
    std::cout << "Failed to initialize Performer, pfInit() returned " << result << std::endl;
    return 0;
  }

  // Tell Performer to print all warnings.
  ::pfNotifyLevel ( PFNFY_ALWAYS );

  // No multiprocessing.
  result = ::pfMultiprocess ( 0 );
  if ( 1 != result )
  {
    std::cout << "Failed to set the number of processes, pfMultiprocess(0) returned " << result << std::endl;
    return 0;
  }

  // Configure Performer, this finally gets Performer to a usable state.
  result = ::pfConfig();
  if ( 1 != result )
  {
    std::cout << "Failed to configure Performer, pfConfig(0) returned " << result << std::endl;
    return 0;
  }

  // Declare an instance of the Jupiter to Performer translator.
  TrJt2Pf jt2pf;

  // Initialize.
  if ( false == jt2pf.init() )
  {
    std::cout << "Failed to initialize translator." << std::endl;
    return 0;
  }

  // Get the arguments.
  typedef std::list<std::string> Args;
  Args args;
  for ( int i = 1; i < argc; ++i )
    args.push_back ( argv[i] );

  // See if the first argument is "-v".
  if ( args.front() == "-v" )
  {
    // Set the verbose flag and lose that argument.
    jt2pf.setVerbose ( true );
    args.pop_front();
  }

  // Loop through all the input files.
  for ( Args::iterator f = args.begin(); f != args.end(); ++f )
  {
    // Translate the jupiter database.
    ::_translate ( *f, jt2pf );
  }

  // Done with Performer.
  ::pfExit();

  // It worked.
  return 1;
}
