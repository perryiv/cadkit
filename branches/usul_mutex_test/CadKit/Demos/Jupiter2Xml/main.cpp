
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A Jupiter to XML translator.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
# pragma warning(disable:4251) // See note 3ce3a140-2edf-11d4-98a4-0040054c86c7.
# pragma warning(disable:4786) // Truncated debug names.
#endif

#include "Translators/Jupiter2Xml/TrJt2Xml.h"
#include "Database/XML/DbXmlWrite.h"
#include "Database/XML/DbXmlGroup.h"
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

bool _translate ( std::string &filename, TrJt2Xml &jt2xml )
{
  SL_ASSERT ( false == filename.empty() );
  std::cout << "Translating: " << filename << std::endl;

  // Declare an XML group.
  DbXmlGroup::Ptr root = new DbXmlGroup ( "jt2xml" );

  // Translate.
  if ( false == jt2xml.translate ( filename.c_str(), *root ) )
  {
    std::cout << jt2xml.getLastError() << std::endl;
    std::cout << "Failed to translate: " << filename << std::endl;
    return false;
  }

  // The output filename should have the same path as the input filename, 
  // except with a different extension.
  SL_ASSERT ( filename.size() - 3 == filename.find ( ".jt" ) );
  filename.resize ( filename.size() - 2 );
  filename += "xml";

  std::cout << "Writing:     " << filename << std::endl;

  // Declare an XML writer.
  DbXmlWrite::Ptr out = new DbXmlWrite;

//#ifdef _DEBUG
//  // Write only the names (no values) to the file.
//  out->setMode ( DbXmlWrite::WRITE_NAME_ONLY );
//#endif

  // Write the XML tree to file.
  if ( false == out->write ( *root, filename.c_str() ) )
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

  // Declare an instance of the Jupiter to XML translator.
  TrJt2Xml jt2xml;

  // Initialize.
  if ( false == jt2xml.init() )
  {
    std::cout << "Failed to initialize translator" << std::endl;
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
    jt2xml.setVerbose ( true );
    args.pop_front();
  }

  // Loop through all the input files.
  for ( Args::iterator f = args.begin(); f != args.end(); ++f )
  {
    // Translate the jupiter database.
    ::_translate ( *f, jt2xml );
  }

  // It worked.
  return 1;
}
