
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Test code for XML reader.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
# pragma warning(disable:4786) // Truncated debug names.
#endif

#include "XmlDom/Policy.h"
#include "XmlDom/Reader.h"
#include "XmlDom/File.h"

#include <string>
#include <iostream>
#include <fstream>


///////////////////////////////////////////////////////////////////////////////
//
//  Test parsing with callbacks.
//
///////////////////////////////////////////////////////////////////////////////

class Parser
{
public:

  // Typedefs.
  typedef XML::Config::Policy<>  Policy;
  typedef XML::Reader < Policy > Reader;
  typedef XML::File < Policy >   File;

  // Constructor/destructor.
  Parser(){}
  ~Parser(){}

  // Parse the file.
  void parse ( const std::string &filename );

protected:

  // Callbacks.
  void _start1  ( const std::string &s );
  void _start2  ( const std::string &s );
  void _start3  ( const std::string &s );
  void _finish1 ( const std::string &s );
  void _finish2 ( const std::string &s );
  void _finish3 ( const std::string &s );
};


///////////////////////////////////////////////////////////////////////////////
//
//  Test parsing with callbacks.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::parse ( const std::string &filename )
{
  // Read the file.
  File file ( filename );
  Reader reader;
  reader.read ( file.begin(), file.end() );

  // Walk the tree.
  //Visitor visitor;
  //visitor.add
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test parsing with callbacks.
//
///////////////////////////////////////////////////////////////////////////////

void testParser ( const std::string &filename )
{
  try
  {
    Parser p;
    p.parse ( filename );
  }
  catch ( const std::exception &e )
  {
    std::cout << "Error: " << e.what() << std::endl;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Main.
//
///////////////////////////////////////////////////////////////////////////////

struct MyData { double value; };

#include <vector>

int main ( int argc, char **argv )
{
  // Run the test.
  testParser ( "example02.xml" );

  return 1;
}
