
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
#include "XmlDom/BuildTree.h"
#include "XmlDom/Node.h"

#include <iterator>
#include <string>
#include <cassert>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>


///////////////////////////////////////////////////////////////////////////////
//
//  Print the node tree.
//
///////////////////////////////////////////////////////////////////////////////

template < class NodeType > struct Print
{
  typedef NodeType Node;
  typedef typename Node::String String;

  void operator() ( const Node *node )
  {
    static String indent;

    String name  ( node->name()  );
    String value ( node->value() );

    std::cout << indent << '<' << name << '>';

    indent += " ";

    if ( 0 == node->numChildren() )
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
  // Typedefs.
  typedef XML::Config::Policy<>                 Policy;
  typedef XML::Node < Policy >                  Node;
  typedef XML::Callback::BuildTree < Node >     BuildCB;
  typedef XML::Reader < Policy, BuildCB >       Reader;

  // Read the file-string.
  Reader reader;
  reader.read ( start, stop );

  // Print the node tree.
  Print<Node> print;
  print ( reader.callback().root() );

  // Try to find a specific node.
  try
  {
    Node *root = reader.callback().root();
    Node *node = root->child ( 0, "b0" );
    node = node->child ( 0, "c2" );
    node = node->child ( 0, "e" );
    node = node->child ( 0, "s" );
    node = root->child ( 0, "article" )->child ( 0, "date" )->child ( 0, "year" );
    node = root->child ( "article/date/year", '/' );
    node = root->child ( "article/author/title/prefix", '/' );
    node = root->child ( "article/author/name/prefix", '/' );
  }
  catch ( const std::exception &e )
  {
    std::cout << "Error: " << e.what() << std::endl;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the contents of the file as a string.
//
///////////////////////////////////////////////////////////////////////////////

void getString ( const std::string &filename, std::string &contents )
{
  // Load the file into a string.
  typedef XML::Config::Policy<> Policy;
  XML::File < Policy > file ( filename );
  contents.assign ( file.begin(), file.end() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test using string iterators.
//
///////////////////////////////////////////////////////////////////////////////

void testString ( const std::string &filename )
{
  // Get file as a string.
  std::string contents;
  getString ( filename, contents );

  // Read the file.
  read ( contents.begin(), contents.end() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test using istream iterators.
//
///////////////////////////////////////////////////////////////////////////////

void testIstream ( const std::string &filename )
{
  typedef std::string::value_type Char;

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

void testArray ( const std::string &filename )
{
  typedef std::string::value_type Char;

  // Get file as a string.
  std::string s;
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

void testList ( const std::string &filename )
{
  typedef std::string::value_type Char;

  // Get file as a string.
  std::string s;
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

void testVector ( const std::string &filename )
{
  typedef std::string::value_type Char;

  // Get file as a string.
  std::string s;
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
  std::string filename ( "example01.xml" );

  // Run the tests.
  testArray ( filename );
  testString ( filename );
  testList ( filename );
  testVector ( filename );
  //testIstream ( filename ); // Doesn't work.

  return 1;
}
