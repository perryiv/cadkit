
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
#include "XmlDom/Print.h"
#include "XmlDom/BuildTree.h"
#include "XmlDom/Node.h"
#include "XmlDom/Visitor.h"
#include "XmlDom/Convert.h"

#include "Usul/Adaptors/MemberFunction.h"

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
  typedef XML::Config::Policy<>                 Policy;
  typedef Policy::String                        String;
  typedef void (Parser::*Function) ( const String &s );
  typedef Usul::Adaptors::MemberFunction < Parser*, Function > MemFun;
  typedef XML::Callback::Print                  PrintCB;
  typedef XML::Node < Policy >                  Node;
  typedef XML::Callback::BuildTree < Node >     BuildCB;
  typedef XML::Reader < Policy, BuildCB >       Reader;
  typedef XML::File < Policy >                  File;
  typedef XML::Visitor < Policy, Node, MemFun > Visitor;

  // Constructor/destructor.
  Parser(){}
  ~Parser(){}

  // Parse the file.
  void parse ( const String &filename );

protected:

  // Callbacks.
  void _start0  ( const String &s );
  void _start1  ( const String &s );
  void _start2  ( const String &s );
  void _start3  ( const String &s );
  void _end0    ( const String &s );
  void _end1    ( const String &s );
  void _end2    ( const String &s );
  void _end3    ( const String &s );
};


///////////////////////////////////////////////////////////////////////////////
//
//  Parsing the file.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::parse ( const String &filename )
{
  // Load the file into a string.
  File file ( filename );

  // Read the file-string.
  Reader reader;
  reader.read ( file.begin(), file.end() );

  // Prepare a visitor.
  Visitor visitor;
  visitor.add ( Visitor::NODE_START, "zero",               MemFun ( this, &Parser::_start0 ) );
  visitor.add ( Visitor::NODE_START, "zero/one",           MemFun ( this, &Parser::_start1 ) );
  visitor.add ( Visitor::NODE_START, "zero/one/two",       MemFun ( this, &Parser::_start2 ) );
  visitor.add ( Visitor::NODE_START, "zero/one/two/three", MemFun ( this, &Parser::_start3 ) );
  visitor.add ( Visitor::NODE_END,   "zero",               MemFun ( this, &Parser::_end0   ) );
  visitor.add ( Visitor::NODE_END,   "zero/one",           MemFun ( this, &Parser::_end1   ) );
  visitor.add ( Visitor::NODE_END,   "zero/one/two",       MemFun ( this, &Parser::_end2   ) );
  visitor.add ( Visitor::NODE_END,   "zero/one/two/three", MemFun ( this, &Parser::_end3   ) );

  // Walk the tree.
  reader.callback().root()->accept ( visitor );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start callbacks.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_start0 ( const String &s )
{
  // Convert for demonstration purposes...
  unsigned int num ( XML::Convert<unsigned int>() ( s ) );
}
void Parser::_start1 ( const String &s )
{
  int num ( XML::Convert<int>() ( s ) );
}
void Parser::_start2 ( const String &s )
{
  bool flag ( XML::Convert<bool>() ( s ) );
}
void Parser::_start3 ( const String &s )
{
  double num ( XML::Convert<double>() ( s ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  End callbacks.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_end0 ( const String &s )
{
}
void Parser::_end1 ( const String &s )
{
}
void Parser::_end2 ( const String &s )
{
}
void Parser::_end3 ( const String &s )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test parsing with callbacks.
//
///////////////////////////////////////////////////////////////////////////////

void testParser ( const Parser::String &filename )
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

int main ( int argc, char **argv )
{
  // Run the test.
  testParser ( "example02.xml" );

  return 1;
}
