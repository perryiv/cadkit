
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
  inline void makeLink ( XmlTree::Node::ValidRefPtr node, Usul::Registry::Node &site )
  {
    if ( "link" != node->name() )
      return;

    // Always clear the children.
    typedef XmlTree::Node::Children Children;
    Usul::Scope::Caller::RefPtr clear ( Usul::Scope::makeCaller ( Usul::Adaptors::memberFunction ( &(node->children()), &Children::clear ) ) );

    // Get the name of the link.
    const std::string name ( node->child ( "name", true )->value() );
    if ( true == name.empty() )
      return;

    // Get the text inside the link.
    const std::string text ( node->child ( "text", true )->value() );
    if ( true == text.empty() )
      return;

    // Get the url.
    const std::string href ( site["links"][name]["href"].get ( "" ) );
    if ( true == href.empty() )
      return;

    const std::string target ( site["links"][name]["target"].get ( "" ) );

    // Make the node into a link.
    node->value ( text );
    node->name ( "a" );
    node->attributes()["href"] = href;

    // Is it an external link?
    if ( false == target.empty() )
      node->attributes()["target"] = target;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to translate the node's value.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline void makeImage ( XmlTree::Node::ValidRefPtr node, Usul::Registry::Node &site )
  {
    if ( "image" != node->name() )
      return;

    // Always clear the children.
    typedef XmlTree::Node::Children Children;
    Usul::Scope::Caller::RefPtr clear ( Usul::Scope::makeCaller ( Usul::Adaptors::memberFunction ( &(node->children()), &Children::clear ) ) );

    // Get the name of the link.
    const std::string name ( node->child ( "name", true )->value() );
    if ( true == name.empty() )
      return;

    // Get the src.
    const std::string src ( site["images"][name]["file"].get ( "" ) );
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
