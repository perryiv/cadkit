
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Program to generate an html page.
//
///////////////////////////////////////////////////////////////////////////////

#include "XmlTree/Document.h"
#include "XmlTree/RegistryIO.h"
#include "XmlTree/XercesLife.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/File/Make.h"
#include "Usul/File/Path.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Registry/Database.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Strings/Format.h"
#include "Usul/Strings/Split.h"
#include "Usul/System/Environment.h"
#include "Usul/System/LastError.h"
#include "Usul/Threads/Mutex.h"

#include <iostream>
#include <list>
#include <map>
#include <stdexcept>
#include <vector>


///////////////////////////////////////////////////////////////////////////////
//
//  The program class.
//
///////////////////////////////////////////////////////////////////////////////

class Program
{
public:

  typedef Usul::Registry::Node RegistryNode;
  typedef std::map<std::string,std::string> StringMap;

  Program ( int argc, char **argv, char **env );
  ~Program();

  void                               run();

protected:

  XmlTree::Node::ValidRefPtr         _loadXmlFile ( const std::string &file );

  XmlTree::Node::ValidRefPtr         _makeBody() const;
  XmlTree::Node::ValidRefPtr         _makeHead() const;

  std::string                        _specialChar ( const std::string &name, unsigned int num ) const;

private:

  RegistryNode &_query;
  RegistryNode &_env;
  StringMap _chars;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Program::Program ( int argc, char **argv, char **env ) : 
  _query ( Usul::Registry::Database::instance()["query_string"] ),
  _env   ( Usul::Registry::Database::instance()["environment"] ),
  _chars ()
{
  typedef std::list<std::string> StringList;

  // Grab the command-line arguments.
  Usul::CommandLine::Arguments::instance().set ( argc, argv );

  // Parse the query string.
  {
    const std::string query ( Usul::System::Environment::get ( "QUERY_STRING" ) );
    StringList pairs;
    Usul::Strings::split ( query, '&', false, pairs );
    for ( StringList::const_iterator i = pairs.begin(); i != pairs.end(); ++i )
    {
      StringList nv;
      Usul::Strings::split ( *i, '=', true, nv );
      if ( 2 == nv.size() )
      {
        StringList::const_iterator nvi ( nv.begin() );
        const std::string name  ( *nvi ); ++nvi;
        const std::string value ( *nvi );
        _query[name] = value;
      }
    }
  }

  // Grab the environment variables.
  if ( 0x0 != env )
  {
    while  ( 0x0 != *env )
    {
      StringList nv;
      Usul::Strings::split ( *env, '=', true, nv );
      if ( 2 == nv.size() )
      {
        StringList::const_iterator nvi ( nv.begin() );
        const std::string name  ( *nvi ); ++nvi;
        const std::string value ( *nvi );
        _env[name] = value;
      }
      ++env;
    }
  }

  // Populate the special characters.
  // See http://www.utexas.edu/learn/html/spchar.html
  _chars[" "] = "&nbsp;";
  _chars["<"] = "&lt;";
  _chars[">"] = "&gt;";
  _chars["&"] = "&amp;";
  _chars["'"] = "&apos;";
  _chars["\""]= "&quot;";
  _chars["–"] = "&ndash;";
  _chars["—"] = "&mdash;";
  _chars["&"] = "&amp;";
  _chars["©"] = "&copy;";
  _chars["÷"] = "&divide;";
  _chars["¶"] = "&para;";
  _chars["±"] = "&plusmn;";
  _chars["®"] = "&reg;";

  // Same values but friendly keys.
  _chars["space"]         = "&nbsp;";
  _chars["short_dash"]    = "&ndash;";
  _chars["long_dash"]     = "&mdash;";
  _chars["copyright"]     = "&copy;";
  _chars["divide"]        = "&divide;";
  _chars["paragraph"]     = "&para;";
  _chars["plus_or_minus"] = "&plusmn;";
  _chars["registered"]    = "&reg;";}


///////////////////////////////////////////////////////////////////////////////
//
//  Desstructor.
//
///////////////////////////////////////////////////////////////////////////////

Program::~Program()
{
  // Write the registry.
  const std::string dir ( Usul::File::fullPath ( "log" ) + "/" );
  const std::string file ( dir + "registry.xml" );
  try
  {
    Usul::File::make ( dir );
    XmlTree::RegistryIO::write ( file, Usul::Registry::Database::instance() );
  }
  catch ( const std::exception &e )
  {
    std::cout << ( ( 0x0 != e.what() ) ? e.what() : "" ) << "<br /><br />" << std::endl;
    std::cout << "Error 9896765970: Failed to write registry file: " << file << "<br />" << std::endl;
    std::cout << this->_specialChar ( "space", 4 ) << "Does the user running this process have write permissions?" << std::endl;
  }

  // Reset the mutex factory.
  Usul::Threads::Mutex::createFunction ( 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return a string holding the special character(s).
//
///////////////////////////////////////////////////////////////////////////////

std::string Program::_specialChar ( const std::string &name, unsigned int num ) const
{
  StringMap::const_iterator i ( _chars.find ( name ) );
  std::string result ( ( _chars.end() != i ) ? i->second : "" );
  if ( num > 1 )
  {
    for ( unsigned int i = 1; i < num; ++i )
    {
      result = result + result;
    }
  }
  return result;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the page header.
//
///////////////////////////////////////////////////////////////////////////////

XmlTree::Node::ValidRefPtr Program::_makeHead() const
{
  // Add the header section.
  XmlTree::Node::ValidRefPtr head ( new XmlTree::Node ( "head" ) );

  // TODO: Add css files.
  // TODO: Add scripts.

  // Return it.
  return head;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the page body.
//
///////////////////////////////////////////////////////////////////////////////

XmlTree::Node::ValidRefPtr Program::_makeBody() const
{
  // Add the body sections.
  XmlTree::Node::ValidRefPtr body ( new XmlTree::Node ( "body" ) );

  // Indent all content of the body.
  body->append ( "blockquote" );

  // Return it.
  return body;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the xml file.
//
///////////////////////////////////////////////////////////////////////////////

XmlTree::Node::ValidRefPtr Program::_loadXmlFile ( const std::string &file )
{
  // Make the document.
  XmlTree::Document::ValidRefPtr document ( new XmlTree::Document );

  // Initialize the last error.
  Usul::System::LastError::init();

  // See if you can open the file.
  if ( false == Usul::Predicates::FileExists::test ( file ) )
  {
    XmlTree::Node::ValidRefPtr node
      ( document->append ( "p", Usul::Strings::format ( "Error 6470080260: File '", file, "' does not exist." ) ) );
    return node;
  }

  // Load the file.
  try { document->load ( file ); }
  USUL_DEFINE_SAFE_CALL_CATCH_BLOCKS ( "5613725640" );

  // Return the document.
  return XmlTree::Node::ValidRefPtr ( document.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the program.
//
///////////////////////////////////////////////////////////////////////////////

void Program::run()
{
  // Let the browser server know we're generating html.
  // See: http://library.thinkquest.org/16728/content/cgi/cplusplus.html
  std::cout << "Content-type: text/html" << std::endl << std::endl;

  // Manage the life-span of xerces.
  XmlTree::XercesLife life;

  // Read the main site into the registry.
  XmlTree::RegistryIO::read ( "site.xml", Usul::Registry::Database::instance() );

  // Make a new document for the html content.
  XmlTree::Document::ValidRefPtr root ( new XmlTree::Document ( "html" ) );

  // Add the main sections.
  root->append ( this->_makeHead() );
  root->append ( this->_makeBody() );

  // Write the html.
  root->write ( std::cout, false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Main function.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv, char **env )
{
  try
  {
    // Initialize mutex factory.
    Usul::Threads::Mutex::createFunction ( &Usul::Threads::newSingleThreadedMutexStub );

    // Instantiate the program.
    Program program ( argc, argv, env );

    // Run the program.
    program.run();
  }
  USUL_DEFINE_SAFE_CALL_CATCH_BLOCKS ( "6248437230" );

  // Success.
  return 0;
}
