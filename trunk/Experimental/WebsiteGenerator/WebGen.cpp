
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
#include "Usul/Convert/Vector2.h"
#include "Usul/Math/Vector2.h"
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
  _site  ( Usul::Registry::Database::instance()["site"] ),
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
  typedef Usul::Math::Vec2ui CellIndex;

  // Add the body sections.
  XmlTree::Node::ValidRefPtr body ( new XmlTree::Node ( "body" ) );

  // Make the table structure.
  {
    const unsigned int numRows ( _site["tables"]["outer"]["num_rows"].get ( 3 ) );
    const unsigned int numCols ( _site["tables"]["outer"]["num_cols"].get ( 2 ) );
    XmlTree::Node::ValidRefPtr table ( this->_makeTable ( numRows, numCols, "outer", _matrix ) );
    body->append ( table.get() );
  }

  // Add the logo.
  {
    CellIndex index ( _site["images"]["logo"]["cell"].get<CellIndex> ( CellIndex ( 0, 0 ) ) );
    XmlTree::Node::ValidRefPtr logo ( this->_cell ( index[0], index[1] ) );
    logo->append ( this->_makeImage ( _site["images"]["logo"]["file"].get ( "logo.png" ), "Logo Image" ) );
  }

  // Add the page title and separator.
  {
    CellIndex index ( _site["subject"]["cell"].get<CellIndex> ( CellIndex ( 0, 1 ) ) );
    XmlTree::Node::ValidRefPtr subject ( this->_cell ( index[0], index[1] ) );
    XmlTree::Node::ValidRefPtr h1 ( subject->append ( "h1", _site["subject"]["long_name"].get ( "Subject" ) ) );
    h1->attributes()["class"] = "subject";
    subject->append ( "hr" );
  }

  // Add the main menu.
  {
    CellIndex index ( _site["menus"]["main"]["cell"].get<CellIndex> ( CellIndex ( 1, 0 ) ) );
    XmlTree::Node::ValidRefPtr menu ( this->_cell ( index[0], index[1] ) );
    for ( Pages::const_iterator i = _pages.begin(); i != _pages.end(); ++i )
    {
      const PageInfo info ( *i );
      XmlTree::Node::ValidRefPtr para ( menu->append ( "p" ) );
      const std::string site ( this->_queryValue ( "site" ) );
      const std::string page ( Usul::File::base ( info.second ) );
      para->append ( this->_link ( info.first, Usul::Strings::format ( this->_urlScript(), "?site=", site, "&page=", page ) ) );
    }
  }

  // Add the content.
  {
    CellIndex index ( _site["content"]["cell"].get<CellIndex> ( CellIndex ( 1, 1 ) ) );
    XmlTree::Node::ValidRefPtr content ( this->_cell ( index[0], index[1] ) );

    const std::string base ( this->_queryValue ( "page", "home" ) );
    const std::string dir ( this->_directory ( _site["pages"]["directory"] ) );
    const std::string path ( Usul::Strings::format ( dir, base, ".xml" ) );
    XmlTree::Node::ValidRefPtr page ( this->_loadXmlFile ( path ) );

    typedef XmlTree::Node::Children Children;
    Children kids ( page->find ( "section", false ) );
    for ( Children::iterator i = kids.begin(); i != kids.end(); ++i )
    {
      XmlTree::Node::ValidRefPtr section ( *i );
      this->_appendChildren ( section, content );
    }
  }

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
  const std::string dir ( this->_directory ( _site["images"]["directory"] ) );

  // Make the path name.
  std::string path ( dir + src );

  // Set the source attribute.
  img->attributes()["src"] = path;

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
  const std::string dir ( this->_directory ( _site["pages"]["directory"] ) );
  std::string file ( _site["pages"]["file"].get ( "" ) );
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
  const std::string dir ( this->_directory ( _site["scripts"]["directory"] ) );
  std::string file ( _site["scripts"]["file"].get ( "" ) );
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
  const std::string dir ( this->_directory ( _site["styles"]["directory"] ) );
  std::string file ( _site["styles"]["file"].get ( "" ) );
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
//  Return a link.
//
///////////////////////////////////////////////////////////////////////////////

XmlTree::Node::ValidRefPtr WebGen::_link ( const std::string &text, const std::string &href, bool internal )
{
  // Make the node and set the attributes.
  XmlTree::Node::ValidRefPtr anchor ( new XmlTree::Node ( "a", text ) );
  anchor->attributes()["href"] = href;

  // Open a new browser or tab?
  if ( false == internal )
  {
    anchor->attributes()["target"] = "blank";
  }

  // Return the node.
  return anchor;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the query value.
//
///////////////////////////////////////////////////////////////////////////////

std::string WebGen::_queryValue ( const std::string &name, const std::string &defaultValue ) const
{
  return _query[name].get ( defaultValue );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append the children.
//
///////////////////////////////////////////////////////////////////////////////

void WebGen::_appendChildren ( XmlTree::Node::ValidRefPtr from, XmlTree::Node::ValidRefPtr to )
{
  typedef XmlTree::Node::Children Children;
  Children kids ( from->children() );
  for ( Children::iterator i = kids.begin(); i != kids.end(); ++i )
  {
    XmlTree::Node::ValidRefPtr child ( *i );
    to->append ( child );
  }
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

  // Make a new document for the html content.
  XmlTree::Document::ValidRefPtr root ( new XmlTree::Document ( "html" ) );

  // Read the configuration file that holds all the sites.
  Usul::Registry::Node &sites ( Usul::Registry::Database::instance()["sites"] );
  XmlTree::RegistryIO::readNode ( "sites.xml", sites );

  // Determine the file to read for the site information.
  const std::string siteName ( this->_queryValue ( "site", "default" ) );
  const std::string siteFile ( sites[siteName].get ( "default" ) );

  // Read the main site into the registry.
  XmlTree::RegistryIO::readNode ( siteFile, _site );

  // Find the pages.
  this->_findPages();

  // Add the main sections.
  root->append ( this->_makeHead() );
  root->append ( this->_makeBody() );

  // Write the html.
  root->write ( std::cout, false );
}
