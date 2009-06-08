
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
#include "Functions.h"
#include "TranslateTags.h"

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
#include "Usul/System/DateTime.h"
#include "Usul/System/Environment.h"
#include "Usul/System/LastError.h"

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
  _query    ( Usul::Registry::Database::instance()["query_string"] ),
  _env      ( Usul::Registry::Database::instance()["environment"] ),
  _site     ( Usul::Registry::Database::instance()["site"] ),
  _chars    (),
  _pageList (),
  _pageMap  (),
  _matrix   ()
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
  _chars["space"]         = " ";
  _chars["short_dash"]    = "–";
  _chars["long_dash"]     = "—";
  _chars["copyright"]     = "©";
  _chars["divide"]        = "÷";
  _chars["paragraph"]     = "¶";
  _chars["plus_or_minus"] = "±";
  _chars["registered"]    = "®";

  // For convenience, put the site from the query string into the member.
  _site["name"] = _query["site"].get<std::string> ( "" );
}



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

  // Add the title.
  const std::string title ( _site["subject"]["long_name"].get<std::string> ( "Title" ) );
  const std::string page ( this->_pageName() );
  head->append ( "title", Usul::Strings::format ( title, " - ", page ) );

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

  // For translating tags.
  TranslateTags translateTags ( _site );
  translateTags.add ( "para", "p" );
  translateTags.add ( "text", ""  );

  // Add the body sections.
  XmlTree::Node::ValidRefPtr body ( new XmlTree::Node ( "body" ) );

  // Is this a movie?
  if ( "movie" == _query["type"].get<std::string> ( "" ) )
  {
    // Get the movie directory.
    const std::string dir ( Functions::directory ( _site["movies"]["directory"] ) );

    // The page name is the movie name.
    const std::string name ( _query["page"].get<std::string> ( "" ) );

    // Get the url for the movie and the cover image.
    const std::string cover ( Functions::urlDomain ( true ) + dir + _site["movies"][name]["cover"].get<std::string> ( "" ) );
    const std::string movie ( Functions::urlDomain ( true ) + dir + _site["movies"][name]["file" ].get<std::string> ( "" ) );

    // Get the url for the player.
    const std::string player ( Functions::urlDomain ( true ) + dir + _site["movies"]["player"]["file" ].get<std::string> ( "" ) );

    // Make the movie object.
    XmlTree::Node::ValidRefPtr center ( body->append ( "center" ) );
    XmlTree::Node::ValidRefPtr object ( center->append ( "object" ) );

    // Add object attributes.
    object->attributes()["type"]   = "application/x-shockwave-flash";
    object->attributes()["data"]   = player;
    object->attributes()["width"]  = _site["movies"][name]["width"].get<std::string> ( "" );
    object->attributes()["height"] = _site["movies"][name]["height"].get<std::string> ( "" );
    object->attributes()["id"]     = "FlowPlayer";

    // Add the param children.
    Functions::addMovieParam ( *object, "allowScriptAccess", "always" );
    Functions::addMovieParam ( *object, "movie",              player );
    Functions::addMovieParam ( *object, "quality",           "high" );
    Functions::addMovieParam ( *object, "scaleMode",         "showAll" );
    Functions::addMovieParam ( *object, "allowfullscreen",   "true" );
    Functions::addMovieParam ( *object, "wmode",             "transparent" );
    Functions::addMovieParam ( *object, "allowNetworking",   "all" );

    // Add the flash variables.
    const std::string flashVars
      ( Usul::Strings::format
        ( "config={ autoPlay: false, loop: false, initialScale: 'scale', ",
          "showLoopButton: true, showPlayListButtons: false, ",
          "playList: [ { url: '", cover, "' }, { url: '", movie, "' } ] }" ) );
    Functions::addMovieParam ( *object, "flashvars", flashVars );
  }

  // Otherwise, it's a regular page.
  else
  {
    // Make the table structure.
    {
      const Usul::Math::Vec2ui size ( _site["tables"]["outer"]["size"].get ( Usul::Math::Vec2ui ( 3, 2 ) ) );
      XmlTree::Node::ValidRefPtr table ( this->_makeTable ( size[0], size[1], "outer", _matrix ) );
      body->append ( table.get() );
    }

    // Add the logo.
    {
      CellIndex index ( _site["images"]["logo"]["cell"].get<CellIndex> ( CellIndex ( 0, 0 ) ) );
      XmlTree::Node::ValidRefPtr logo ( this->_cell ( index[0], index[1] ) );
      XmlTree::Node::ValidRefPtr link ( logo->append ( "a" ) );

      // The image gets the logo attribute.
      XmlTree::Node::ValidRefPtr img
        ( this->_makeImage ( _site["images"]["logo"]["file"].get<std::string> ( "logo.png" ), "Logo Image" ) );
      img->attributes()["class"] = "logo_link";
      link->append ( img );

      // Set link attributes.
      link->attributes()["href"] = 
        Usul::Strings::format ( Functions::urlScript(), "?site=", _site["name"].get<std::string> ( "" ) );
    }

    // Add the page title and separator.
    {
      CellIndex index ( _site["subject"]["cell"].get<CellIndex> ( CellIndex ( 0, 1 ) ) );
      XmlTree::Node::ValidRefPtr subject ( this->_cell ( index[0], index[1] ) );
      const std::string longName ( _site["subject"]["long_name"].get<std::string> ( "Subject" ) );
      const std::string pageName ( this->_pageName() );
      XmlTree::Node::ValidRefPtr h1 ( subject->append ( "h1", longName ) );
      h1->attributes()["class"] = "subject";
      subject->append ( "hr" );

      // If we have a section specified then the page name is a link.
      const std::string sectionName ( this->_queryValue ( "section", "" ) );
      if ( false == sectionName.empty() )
      {
        XmlTree::Node::ValidRefPtr para ( subject->append ( "p" ) );
        para->attributes()["class"] = "page_name";
        const std::string site ( this->_queryValue ( "site" ) );
        const std::string page ( this->_queryValue ( "page" ) );
        para->append ( this->_link ( pageName, Usul::Strings::format ( Functions::urlScript(), "?site=", site, "&page=", page ) ) );
        para->append ( "text", Usul::Strings::format ( " --> ", sectionName ) );
      }

      // No section...
      else
      {
        XmlTree::Node::ValidRefPtr page ( subject->append ( "p", pageName ) );
        page->attributes()["class"] = "page_name";
      }
    }
    
    // Add the main menu.
    {
      CellIndex index ( _site["menus"]["main"]["cell"].get<CellIndex> ( CellIndex ( 1, 0 ) ) );
      XmlTree::Node::ValidRefPtr menu ( this->_cell ( index[0], index[1] ) );
      for ( PageList::const_iterator i = _pageList.begin(); i != _pageList.end(); ++i )
      {
        const PageInfo info ( *i );
        XmlTree::Node::ValidRefPtr para ( menu->append ( "p" ) );
        const std::string site ( this->_queryValue ( "site" ) );
        const std::string page ( Usul::File::base ( info.second ) );
        para->append ( this->_link ( info.first, Usul::Strings::format 
          ( Functions::urlScript(), "?site=", site, "&page=", page ) ) );
      }
    }

    // Add the content.
    {
      CellIndex index ( _site["content"]["cell"].get<CellIndex> ( CellIndex ( 1, 1 ) ) );
      XmlTree::Node::ValidRefPtr content ( this->_cell ( index[0], index[1] ) );

      const std::string base ( this->_queryValue ( "page", "home" ) );
      const std::string root ( Functions::directory ( Usul::System::Environment::get ( "DOCUMENT_ROOT" ) ) );
      const std::string dir ( Functions::directory ( _site["pages"]["directory"] ) );
      const std::string path ( Usul::Strings::format ( root, dir, base, ".xml" ) );
      XmlTree::Node::ValidRefPtr page ( this->_loadXmlFile ( path ) );

      typedef XmlTree::Node::Children Children;
      Children sections ( page->find ( "section", false ) );
      Children definitions ( page->find ( "define", false ) );

      bool makeSectionMenu ( true );

      // If there is only one section...
      if ( 1 == sections.size() )
      {
        XmlTree::Node::ValidRefPtr section ( sections.at ( 0 ) );
        this->_replaceDefinitions ( definitions, section );
        this->_appendChildren ( section, content );
        makeSectionMenu = false;
      }

      // If there are multiple sections, or none at all...
      else
      {
        // See if a section has been specified.
        const std::string sectionName ( this->_queryValue ( "section", "" ) );
        if ( false == sectionName.empty() )
        {
          // Loop through the children.
          for ( Children::iterator i = sections.begin(); i != sections.end(); ++i )
          {
            XmlTree::Node::ValidRefPtr section ( *i );
            if ( sectionName == section->attribute ( "name" ) )
            {
              this->_replaceDefinitions ( definitions, section );
              this->_appendChildren ( section, content );
              makeSectionMenu = false;
            }
          }
        }
      }

      // Need to make a menu?
      if ( true == makeSectionMenu )
      {
        // Loop through the children.
        for ( Children::iterator i = sections.begin(); i != sections.end(); ++i )
        {
          XmlTree::Node::ValidRefPtr section ( *i );
          const std::string sectionName ( section->attribute ( "name" ) );
          if ( false == sectionName.empty() )
          {
            XmlTree::Node::ValidRefPtr para ( content->append ( "p" ) );
            XmlTree::Node::ValidRefPtr div ( para->append ( "div" ) );

            // Add link.
            const std::string site ( this->_queryValue ( "site" ) );
            const std::string page ( this->_queryValue ( "page" ) );
            div->append ( this->_link ( sectionName, Usul::Strings::format 
                                        ( Functions::urlScript(), "?site=", site, 
                                          "&page=", page, "&section=", sectionName ) ) );

            // Is there a description?
            const std::string description ( section->attribute ( "description" ) );
            if ( false == description.empty() )
            {
              div = para->append ( "div" );
              div->append ( "text", description );
            }
          }
        }
      }

      translateTags ( content );
    }

    // Add the legal section.
    {
      CellIndex index ( _site["legal"]["cell"].get<CellIndex> ( CellIndex ( 2, 1 ) ) );
      XmlTree::Node::ValidRefPtr legal ( this->_cell ( index[0], index[1] ) );
      const std::string ownerLong ( _site["owner"]["long_name"].get<std::string> ( "" ) );
      if ( false == ownerLong.empty() )
      {
        const std::string year ( Usul::System::DateTime::format ( "%Y" ) );
        legal->append ( "hr" );
        const std::string ownerShort ( _site["owner"]["short_name"].get<std::string> ( "" ) );
        if ( true == ownerShort.empty() )
        {
          legal->append ( "p", Usul::Strings::format ( "Copyright © ", year, ", ", ownerLong ) );
        }
        else
        {
          XmlTree::Node::ValidRefPtr para ( legal->append ( "p", Usul::Strings::format ( "Copyright © ", year, ", " ) ) );
          XmlTree::Node::ValidRefPtr link ( para->append ( "link" ) );
          link->append ( "name", ownerShort );
          link->append ( "text", ownerLong );
          translateTags ( para );
        }
      }
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

XmlTree::Node::ValidRefPtr WebGen::_makeImage ( const std::string &file, const std::string &alt )
{
  XmlTree::Node::ValidRefPtr img ( new XmlTree::Node ( "img" ) );
  img->attributes()["alt"] = alt;

  // Get the image directory.
  const std::string dir ( Functions::directory ( _site["images"]["directory"] ) );

  // Make the src.
  std::string src ( Usul::Strings::format ( Functions::urlDomain ( true ), dir, file ) );

  // Set the source attribute.
  img->attributes()["src"] = src;

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
  const std::string root ( Functions::directory ( Usul::System::Environment::get ( "DOCUMENT_ROOT" ) ) );
  const std::string dir ( Functions::directory ( _site["pages"]["directory"] ) );
  std::string file ( _site["pages"]["file"].get<std::string> ( "" ) );
  if ( true == file.empty() )
  {
    std::cout << "No pages specified" << std::endl;
    return;
  }
  file = Usul::Strings::format ( root, dir, file );
  if ( false == Usul::Predicates::FileExists::test ( file ) )
  {
    std::cout << "File '" << file << "' does not exists" << std::endl;
    return;
  }

  // Read the pages file.
  XmlTree::Node::ValidRefPtr pages ( this->_loadXmlFile ( file ) );

  // Loop through the pages.
  typedef XmlTree::Node::Children Children;
  Children kids ( pages->find ( "page", false ) );
  _pageList.reserve ( kids.size() );
  for ( Children::iterator i = kids.begin(); i != kids.end(); ++i )
  {
    XmlTree::Node::ValidRefPtr page ( *i );
    XmlTree::Node::ValidRefPtr menu ( page->child ( "menu", true ) );
    XmlTree::Node::ValidRefPtr file ( page->child ( "file", true ) );
    _pageList.push_back ( PageInfo ( menu->value(), file->value() ) );

    const std::string base ( Usul::File::base ( file->value() ) );
    if ( ( false == base.empty() ) && ( false == menu->value().empty() ) )
    {
      _pageMap[base] = menu->value();
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the cell.
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
//  Add the scripts.
//
///////////////////////////////////////////////////////////////////////////////

void WebGen::_addScripts ( XmlTree::Node::ValidRefPtr parent )
{
  // Get the file name.
  const std::string root ( Functions::directory ( Usul::System::Environment::get ( "DOCUMENT_ROOT" ) ) );
  const std::string dir ( Functions::directory ( _site["scripts"]["directory"] ) );
  std::string file ( _site["scripts"]["file"].get<std::string> ( "" ) );
  if ( true == file.empty() )
    return;
  file = Usul::Strings::format ( root, dir, file );
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
  const std::string root ( Functions::directory ( Usul::System::Environment::get ( "DOCUMENT_ROOT" ) ) );
  const std::string dir ( Functions::directory ( _site["styles"]["directory"] ) );
  std::string file ( _site["styles"]["file"].get<std::string> ( "" ) );
  if ( true == file.empty() )
    return;

  // Make the url to the style sheet.
  const std::string url ( Usul::Strings::format ( Functions::urlDomain ( true ), dir, file ) );

  // Make the node.
  XmlTree::Node::ValidRefPtr link ( new XmlTree::Node ( "link" ) );
  link->attributes()["href"] = url;
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
//  Replace any "use" tags with the corresponding definition.
//
///////////////////////////////////////////////////////////////////////////////

void WebGen::_replaceDefinitions ( XmlTree::Node::Children &from, XmlTree::Node::ValidRefPtr to )
{
  typedef XmlTree::Node::Children Children;
  Children use ( to->find ( "use", true ) );

  for ( Children::iterator i = use.begin(); i != use.end(); ++i )
  {
    XmlTree::Node::ValidRefPtr child ( *i );
    const std::string defName ( child->attribute ( "name" ) );
    if ( false == defName.empty() )
    {
      for ( Children::iterator j = from.begin(); j != from.end(); ++j )
      {
        XmlTree::Node::ValidRefPtr node ( *j );
        if ( defName == node->attribute ( "name" ) )
        {
          child->clear();
          this->_appendChildren ( node, child );
        }
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return this page's name.
//
///////////////////////////////////////////////////////////////////////////////

std::string WebGen::_pageName() const
{
  const std::string page ( this->_queryValue ( "page" ) );
  PageMap::const_iterator i ( _pageMap.find ( page ) );
  return ( ( _pageMap.end() == i ) ? "Home" : i->second );
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
  const std::string siteFile ( sites[siteName].get<std::string> ( "default" ) );

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
