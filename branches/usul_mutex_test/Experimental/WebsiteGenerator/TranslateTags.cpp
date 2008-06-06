
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
//  Class to translate tags.
//
///////////////////////////////////////////////////////////////////////////////

#include "TranslateTags.h"
#include "Functions.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Scope/Caller.h"
#include "Usul/System/Environment.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

TranslateTags::TranslateTags ( Usul::Registry::Node &site ) :
  _site ( site ),
  _tags ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Desstructor.
//
///////////////////////////////////////////////////////////////////////////////

TranslateTags::~TranslateTags()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a tag.
//
///////////////////////////////////////////////////////////////////////////////

void TranslateTags::add ( const std::string &from, const std::string &to )
{
  _tags[from] = to;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to translate the node's value.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline void translate ( XmlTree::Node::ValidRefPtr node, const TranslateTags::Tags &tags )
  {
    // Look for the name.
    TranslateTags::Tags::const_iterator i ( tags.find ( node->name() ) );

    // Replace if we should.
    if ( tags.end() != i )
    {
      node->name ( i->second );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to translate the node's value.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline void makeImage ( XmlTree::Node::ValidRefPtr node, Usul::Registry::Node &site, const std::string &fileTagName = "file" )
  {
    if ( "image" != node->name() )
      return;

    // Always clear the children.
    typedef XmlTree::Node::Children Children;
    Usul::Scope::Caller::RefPtr clear
      ( Usul::Scope::makeCaller
        ( Usul::Adaptors::memberFunction ( &(node->children()), &Children::clear ) ) );

    // Get the name of the link.
    const std::string name ( node->child ( "name", true )->value() );
    if ( true == name.empty() )
      return;

    // Get the src.
    const std::string src ( site["images"][name][fileTagName].get ( "" ) );
    if ( true == src.empty() )
      return;

    // Get the alt text.
    const std::string alt ( site["images"][name]["alternative"].get ( "" ) );

    // Get the image directory.
    const std::string dir ( Functions::directory ( site["images"]["directory"] ) );

    // Make the node into an image.
    node->value ( "" );
    node->name ( "img" );
    node->attributes()["src"] = Functions::urlDomain ( true ) + dir + src;

    // Is there alternative text?
    if ( false == alt.empty() )
      node->attributes()["alt"] = alt;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to translate the node's value.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline void makeLink ( XmlTree::Node::ValidRefPtr node, Usul::Registry::Node &site )
  {
    if ( "link" != node->name() )
      return;

    // Always clear the children.
    typedef XmlTree::Node::Children Children;
    Usul::Scope::Caller::RefPtr clear
      ( Usul::Scope::makeCaller
        ( Usul::Adaptors::memberFunction ( &(node->children()), &Children::clear ) ) );

    // Get the name of the link.
    const std::string name ( node->child ( "name", true )->value() );
    if ( true == name.empty() )
      return;

    // Make the node into a link.
    node->name ( "a" );
    node->value ( node->child ( "text", true )->value() );

    // Set the url.
    {
      const std::string href ( site["links"][name]["href"].get ( "" ) );
      if ( false == href.empty() )
        node->attributes()["href"] = href;
    }

    // Are we linking to an image?
    {
      const std::string image ( site["links"][name]["image"].get ( "" ) );
      if ( false == image.empty() )
      {
        const std::string domain ( Functions::urlDomain ( true ) );
        const std::string dir  ( Functions::directory ( site["images"]["directory"].get ( "" ) ) );
        const std::string file ( site["images"][name]["file"].get ( "" ) );
        const std::string href ( Usul::Strings::format ( domain, dir, file ) );
        node->attributes()["href"] = href;
      }
    }

    // Is it an external link?
    {
      const std::string target ( site["links"][name]["target"].get ( "" ) );
      if ( false == target.empty() )
        node->attributes()["target"] = target;
    }

    // Is this an image link?
    {
      XmlTree::Node::ValidRefPtr image ( node->child ( "image", true ) );
      if ( false == image->value().empty() )
      {
        // Clear children now.
        clear = 0x0;

        // Make the correct xml tree.
        image->append ( "name", image->value() );
        image->value ( "" );

        // Make the image.
        Helper::makeImage ( image, site, "thumb" );

        // Add image back to node.
        node->append ( image );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to translate the node's value.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline void makeMovie ( XmlTree::Node::ValidRefPtr node, Usul::Registry::Node &site )
  {
    if ( "movie" != node->name() )
      return;

    // Always clear the children.
    typedef XmlTree::Node::Children Children;
    Usul::Scope::Caller::RefPtr clear
      ( Usul::Scope::makeCaller
        ( Usul::Adaptors::memberFunction ( &(node->children()), &Children::clear ) ) );

    // Get the name of the link.
    const std::string name ( node->child ( "name", true )->value() );
    if ( true == name.empty() )
      return;

    // Get the movie file.
    const std::string file ( site["movies"][name]["file"].get ( "" ) );
    if ( true == file.empty() )
      return;

    // Get the cover image file.
    const std::string cover ( site["movies"][name]["cover"].get ( "" ) );

    // Get the thumbnail image file.
    const std::string thumb ( site["movies"][name]["thumb"].get ( "" ) );

    // Get the movie directory.
    const std::string dir ( Functions::directory ( site["movies"]["directory"] ) );

    // Clear the children now.
    clear = 0x0;

    // Make the link.
    const std::string href
      ( Usul::Strings::format
        ( Functions::urlScript(), "?site=", site["name"].get ( "" ), 
          "&page=", site["movies"][name].name(), "&type=movie" ) );

    // Make the node into a link.
    node->value ( "" );
    node->name ( "a" );
    node->attributes()["href"] = href;

    // The image is the value of the link.
    XmlTree::Node::ValidRefPtr img ( node->append ( "img" ) );
    img->attributes()["src"] = Functions::urlDomain ( true ) + dir + thumb;
    img->attributes()["alt"] = file;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to translate the tags.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline void translate ( XmlTree::Node::ValidRefPtr node, const TranslateTags::Tags &tags, Usul::Registry::Node &site )
  {
    // Translate the given node.
    Helper::translate ( node, tags );

    // Is it a link?
    Helper::makeLink ( node, site );

    // Is it an image?
    Helper::makeImage ( node, site );

    // Is it a movie?
    Helper::makeMovie ( node, site );

    // Loop through the children.
    typedef XmlTree::Node::Children Children;
    Children kids ( node->children() );
    for ( Children::iterator i = kids.begin(); i != kids.end(); ++i )
    {
      // Translate this child.
      XmlTree::Node::ValidRefPtr child ( *i );
      Helper::translate ( child, tags, site );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Translate the tags.
//
///////////////////////////////////////////////////////////////////////////////

void TranslateTags::operator () ( XmlTree::Node::ValidRefPtr node )
{
  Helper::translate ( node, _tags, _site );
}
