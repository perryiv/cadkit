
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

#include "Error.h"
#include "Config.h"
#include "Callback.h"
#include "Node.h"
#include "Reader.h"

#include <iterator>
#include <string>
#include <cassert>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <sys/stat.h>

#ifdef _WIN32
#define STAT _stat
#else
#define STAT stat
#endif

// For convenience.
typedef std::string String;


///////////////////////////////////////////////////////////////////////////////
//
//  Print the node tree.
//
///////////////////////////////////////////////////////////////////////////////

template < class Node > struct Print
{
  typedef typename Node::String String;
  typedef typename Node::Pointer Pointer;

  void operator() ( const Pointer &node )
  {
    static String indent;

    String name = node->getName();
    String value = node->getValue();

    std::cout << indent << '<' << name << '>';

    indent += " ";

    if ( 0 == node->getNumChildren() )
    {
      std::cout << value << "</" << name << ">\n";
      indent.resize ( indent.size() - 1 );
    }

    else
    {
      std::cout << '\n';
      std::for_each ( node->begin(), node->end(), *this );
      indent.resize ( indent.size() - 1 );
      std::cout << indent << "</" << name << ">\n";
    }
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Read the iterators.
//
///////////////////////////////////////////////////////////////////////////////

template < class I > void read ( const I &start, const I &stop )
{
  // For convenience.
  const bool checkForErrors ( true );
  const bool createMissingChildren ( true );
  typedef typename XML::Error::Assert < checkForErrors > AssertPolicy;
  typedef typename XML::Error::Thrower < checkForErrors > ThrowPolicy;
  typedef typename XML::Error::Pair < AssertPolicy, ThrowPolicy > ErrorPolicy;
  typedef typename XML::Node < String, ErrorPolicy, createMissingChildren > Node;
  typedef typename XML::Callback::Notify < String > NodeCallback;
  typedef typename XML::Config::Trim TrimPolicy;
  typedef typename XML::Reader < Node, ErrorPolicy, NodeCallback, TrimPolicy > Reader;
  typedef typename Reader::Node Node;
  typedef typename Node::Pointer Pointer;

  // Construct the node tree.
  Reader reader ( start, stop );

  // Print the node tree.
  Print<Node> print;
  print ( reader.getRoot() );

  // Try to find a specific node.
  try
  {
    Pointer root = reader.getRoot();
    Pointer node = root->getChild ( 0, "b0" );
    node = node->getChild ( 0, "c2" );
    node = node->getChild ( 0, "e" );
    node = node->getChild ( 0, "s" );
    node = root->getChild ( 0, "article" )->getChild ( 0, "date" )->getChild ( 0, "year" );
    node = root->getChild ( "article/date/year", '/' );
    node = root->getChild ( "article/author/title/prefix", '/' );
    node = root->getChild ( "article/author/name/prefix", '/' );
  }
  catch ( const std::exception &e )
  {
    std::cout << "Error: " << e.what() << std::endl;
  }
  catch ( const XML::Error::Exception & )
  {
    std::cout << "Error when reading XML file" << std::endl;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the contents of the file as a string.
//
///////////////////////////////////////////////////////////////////////////////

void getString ( const String &filename, String &contents )
{
  struct STAT buf;
  int result = _stat ( filename.c_str(), &buf );
  assert ( 0 == result );

  // Size the string that will hold the entire file.
  contents.resize ( buf.st_size );

  // Open the file.
  std::ifstream in ( filename.c_str() );
  assert ( in.is_open() );

  // Read the file into the string.
  for ( String::size_type i = 0; i < contents.size(); ++i )
    contents[i] = in.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test using string iterators.
//
///////////////////////////////////////////////////////////////////////////////

void testString ( const String &filename )
{
  // Get file as a string.
  String contents;
  getString ( filename, contents );

  // Read the file.
  read ( contents.begin(), contents.end() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test using istream iterators.
//
///////////////////////////////////////////////////////////////////////////////

void testIstream ( const String &filename )
{
  typedef String::value_type Char;

  // Open the file.
  std::ifstream in ( filename.c_str() );
  assert ( in.is_open() );

  // Read the file.
  read ( std::istream_iterator<Char> ( in ), std::istream_iterator<Char>() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test using an array.
//
///////////////////////////////////////////////////////////////////////////////

void testArray ( const String &filename )
{
  typedef String::value_type Char;

  // Get file as a string.
  String s;
  getString ( filename, s );

  // Read the file.
  const Char *str = s.c_str();
  read ( str, str + s.size() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test using list iterators.
//
///////////////////////////////////////////////////////////////////////////////

void testList ( const String &filename )
{
  typedef String::value_type Char;

  // Get file as a string.
  String s;
  getString ( filename, s );

  // Construct a list.
  std::list<Char> contents ( s.begin(), s.end() );

  // Read the file.
  read ( contents.begin(), contents.end() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test using vector iterators.
//
///////////////////////////////////////////////////////////////////////////////

void testVector ( const String &filename )
{
  typedef String::value_type Char;

  // Get file as a string.
  String s;
  getString ( filename, s );

  // Construct a list.
  std::vector<Char> contents ( s.begin(), s.end() );

  // Read the file.
  read ( contents.begin(), contents.end() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Main.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
  // Get the file size.
  String filename ( "test.xml" );

  // Run the tests.
  testArray ( filename );
  testString ( filename );
  testList ( filename );
  testVector ( filename );
  //testIstream ( filename ); // Doesn't work.

  return 1;
}
