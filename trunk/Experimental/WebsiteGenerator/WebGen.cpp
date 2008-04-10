
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
//  Class to generate an html page.
//
///////////////////////////////////////////////////////////////////////////////

#include "WebGen.h"

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
#include "Usul/Strings/Case.h"
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
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

WebGen::WebGen ( int argc, char **argv, char **env ) : 
  _query ( Usul::Registry::Database::instance()["query_string"] ),
  _env   ( Usul::Registry::Database::instance()["environment"] ),
  _chars (),
  _pages (),
  _matrix()
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

WebGen::~WebGen()
{
  // Make the log directory.
  const std::string dir ( Usul::File::fullPath ( "log" ) + "/" );
  try
  {
    // Make the directory.
    Usul::File::make ( dir );

    // Write the registry.
    const std::string file ( dir + "registry.xml" );
    try { XmlTree::RegistryIO::write ( file, Usul::Registry::Database::instance() ); }
    catch ( ... ){}
  }
  catch ( ... ){}

  // Reset the mutex factory.
  Usul::Threads::Mutex::createFunction ( 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return a string holding the special character(s).
//
///////////////////////////////////////////////////////////////////////////////

std::string WebGen::_specialChar ( const std::string &name, unsigned int num ) const
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

XmlTree::Node::ValidRefPtr WebGen::_makeHead()
{
  // Add the header section.
  XmlTree::Node::ValidRefPtr head ( new XmlTree::Node ( "head" ) );

  // Add any cascading style sheets.
  this->_addStyles ( head );

  // Add any scripts.
  this->_addScripts ( head );

  // Return it.
  return head;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the page body.
//
///////////////////////////////////////////////////////////////////////////////

XmlTree::Node::ValidRefPtr WebGen::_makeBody()
{
  // Add the body sections.
  XmlTree::Node::ValidRefPtr body ( new XmlTree::Node ( "body" ) );

  // Make the table structure.
  XmlTree::Node::ValidRefPtr table ( this->_makeTable ( 3, 2, "outer", _matrix ) );
  body->append ( table.get() );

  // Add the logo.
  XmlTree::Node::ValidRefPtr logo ( this->_cell ( 0, 0 ) );
  logo->append ( this->_makeImage ( "logo", "Logo Image" ) );

  // Add the page title and separator.
  XmlTree::Node::ValidRefPtr title ( this->_cell ( 0, 1 ) );
  title->append ( "h1", Usul::Registry::Database::instance()["subject"]["long_name"].get ( "Title" ) );
  title->append ( "hr" );
  
  // Return it.
  return body;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the image tag.
//
///////////////////////////////////////////////////////////////////////////////

XmlTree::Node::ValidRefPtr WebGen::_makeImage ( const std::string &src, const std::string &alt )
{
  XmlTree::Node::ValidRefPtr img ( new XmlTree::Node ( "img" ) );
  img->attributes()["alt"] = alt;

  // Get the image directory.
  const std::string dir ( this->_directory ( Usul::Registry::Database::instance()["images"]["directory"] ) );

  // Is the given src string a file?
  std::string file ( dir + src );
  if ( true == Usul::Predicates::FileExists::test ( file ) )
  {
    img->attributes()["src"] = src;
  }

  // Otherwise, try the registry.
  else
  {
    file = Usul::Registry::Database::instance()["images"][src].get ( "" );
    if ( false == file.empty() )
    {
      file = dir + file;
      if ( true == Usul::Predicates::FileExists::test ( file ) )
      {
        img->attributes()["src"] = file;
      }
    }
  }

  // Return it.
  return img;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the table.
//
///////////////////////////////////////////////////////////////////////////////

XmlTree::Node::ValidRefPtr WebGen::_makeTable ( unsigned int numRows, unsigned int numCols, const std::string &cssClassName, NodeMatrix &matrix ) const
{
  XmlTree::Node::ValidRefPtr table ( new XmlTree::Node ( "table" ) );
  table->attributes()["class"] = cssClassName;

  matrix.clear();
  matrix.reserve ( numRows );

  for ( unsigned int i = 0; i < numRows; ++i )
  {
    XmlTree::Node::ValidRefPtr tr ( table->append ( "tr" ) );
    tr->attributes()["class"] = Usul::Strings::format ( "outer_row_", i );

    matrix.push_back ( NodeVector() );
    NodeVector &row ( matrix.back() );
    row.reserve ( numCols );

    for ( unsigned int j = 0; j < numCols; ++j )
    {
      XmlTree::Node::ValidRefPtr td ( tr->append ( "td" ) );
      td->attributes()["class"] = Usul::Strings::format ( "outer_row_", i, "_column_", j );
      row.push_back ( td );
    }
  }

  return table;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the xml file.
//
///////////////////////////////////////////////////////////////////////////////

XmlTree::Node::ValidRefPtr WebGen::_loadXmlFile ( const std::string &file ) const
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
//  Find the pages.
//
///////////////////////////////////////////////////////////////////////////////

void WebGen::_findPages()
{
  // Get the file for the pages.
  const std::string dir ( this->_directory ( Usul::Registry::Database::instance()["pages"]["directory"] ) );
  std::string file ( Usul::Registry::Database::instance()["pages"]["file"].get ( "" ) );
  if ( true == file.empty() )
    return;
  file = dir + file;
  if ( false == Usul::Predicates::FileExists::test ( file ) )
    return;

  // Read the pages file.
  XmlTree::Node::ValidRefPtr pages ( this->_loadXmlFile ( file ) );

  // Loop through the pages.
  typedef XmlTree::Node::Children Children;
  Children kids ( pages->find ( "page", false ) );
  _pages.reserve ( kids.size() );
  for ( Children::iterator i = kids.begin(); i != kids.end(); ++i )
  {
    XmlTree::Node::ValidRefPtr page ( *i );
    XmlTree::Node::ValidRefPtr menu ( page->child ( "menu", true ) );
    XmlTree::Node::ValidRefPtr file ( page->child ( "file", true ) );
    _pages.push_back ( PageInfo ( menu->value(), file->value() ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  FinReturn the cell.
//
///////////////////////////////////////////////////////////////////////////////

XmlTree::Node::ValidRefPtr WebGen::_cell ( unsigned int r, unsigned int c )
{
  NodeVector &row ( _matrix.at ( r ) );
  XmlTree::Node::ValidRefPtr node ( row.at ( c ) );
  return node;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find the pages.
//
///////////////////////////////////////////////////////////////////////////////

std::string WebGen::_directory ( Usul::Registry::Node &node )
{
  // Get the directory.
  std::string dir ( node.get ( "./" ) );

  // Fix the slashes.
  std::replace ( dir.begin(), dir.end(), '\\', '/' );

  // Make sure we have something.
  dir = ( ( true == dir.empty() ) ? std::string ( "./" ) : dir );

  // Make sure there is a trailing slash.
  const unsigned int last ( dir.size() - 1 );
  if ( '/' != dir.at(last) )
    dir += "/";

  // Return the string.
  return dir;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return URL of the domain. Ex: http://www.mysite.com/
//
///////////////////////////////////////////////////////////////////////////////

std::string WebGen::_urlDomain ( bool wantSlash ) const
{
  const std::string protocol ( this->_protocol() );
  const std::string serverName ( _env["SERVER_NAME"].get ( "localhost" ) );
  return Usul::Strings::format ( protocol + "://" + serverName + ( ( wantSlash ) ? "/" : "" ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return URL of the script. Ex: http://www.mysite.com/cgi-bin/my_dir/my_script
//
///////////////////////////////////////////////////////////////////////////////

std::string WebGen::_urlScript() const
{
  const std::string domain ( this->_urlDomain ( false ) );
  const std::string program ( Usul::File::base ( Usul::CommandLine::Arguments::instance().argv ( 0 ) ) );
  std::string script ( _env["SCRIPT_NAME"].get ( program ) );
  return Usul::Strings::format ( domain, script );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return URL of the script. Ex: http://www.mysite.com/cgi-bin/my_dir/
//
///////////////////////////////////////////////////////////////////////////////

std::string WebGen::_urlScriptDir ( bool wantSlash ) const
{
  std::string script ( this->_urlScript() );
  std::vector<char> temp ( script.rbegin(), script.rend() );
  temp.erase ( temp.begin(), std::find ( temp.begin(), temp.end(), '/' ) );
  return std::string ( temp.rbegin(), temp.rend() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the protocol, either "http" or "https".
//
///////////////////////////////////////////////////////////////////////////////

std::string WebGen::_protocol() const
{
  return ( ( "on" == Usul::Strings::lowerCase ( Usul::System::Environment::get ( "HTTPS" ) ) ) ? "https" : "http" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the scripts.
//
///////////////////////////////////////////////////////////////////////////////

void WebGen::_addScripts ( XmlTree::Node::ValidRefPtr parent )
{
  // Get the file name.
  const std::string dir ( this->_directory ( Usul::Registry::Database::instance()["scripts"]["directory"] ) );
  std::string file ( Usul::Registry::Database::instance()["scripts"]["file"].get ( "" ) );
  if ( true == file.empty() )
    return;
  file = dir + file;
  if ( false == Usul::Predicates::FileExists::test ( file ) )
    return;

  // Read the file.
  XmlTree::Node::ValidRefPtr scripts ( this->_loadXmlFile ( file ) );

  // Loop through the correct children.
  typedef XmlTree::Node::Children Children;
  Children kids ( scripts->find ( "script", false ) );
  for ( Children::iterator i = kids.begin(); i != kids.end(); ++i )
  {
    XmlTree::Node::ValidRefPtr script ( *i );
    parent->append ( script );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the style sheets.
//
///////////////////////////////////////////////////////////////////////////////

void WebGen::_addStyles ( XmlTree::Node::ValidRefPtr parent )
{
  // Get the file name.
  const std::string dir ( this->_directory ( Usul::Registry::Database::instance()["styles"]["directory"] ) );
  std::string file ( Usul::Registry::Database::instance()["styles"]["file"].get ( "" ) );
  if ( true == file.empty() )
    return;
  file = dir + file;
  if ( false == Usul::Predicates::FileExists::test ( file ) )
    return;

  // Make the node.
  XmlTree::Node::ValidRefPtr link ( new XmlTree::Node ( "link" ) );
  link->attributes()["href"] = file;
  link->attributes()["rel"]  = "stylesheet";
  link->attributes()["type"] = "text/css";

  // Add the file.
  parent->append ( link );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the program.
//
///////////////////////////////////////////////////////////////////////////////

void WebGen::run()
{
  // Let the browser server know we're generating html.
  // See: http://library.thinkquest.org/16728/content/cgi/cplusplus.html
  std::cout << "Content-type: text/html" << std::endl << std::endl;

  // Manage the life-span of xerces.
  XmlTree::XercesLife life;

  // Read the main site into the registry.
  const std::string site ( Usul::Strings::format ( _query["site"].get ( "default" ), ".xml" ) );
  XmlTree::RegistryIO::read ( site, Usul::Registry::Database::instance() );

  // Find the pages.
  this->_findPages();

  // Make a new document for the html content.
  XmlTree::Document::ValidRefPtr root ( new XmlTree::Document ( "html" ) );

  // Add the main sections.
  root->append ( this->_makeHead() );
  root->append ( this->_makeBody() );

  // Write the html.
  root->write ( std::cout, false );
}
