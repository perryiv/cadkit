
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/Kml/Feature.h"

#include "XmlTree/Node.h"

using namespace Minerva::Layers::Kml;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Feature::Feature() : 
  BaseClass(),
  _name(),
  _visiblity ( true ),
  _lookAt ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Feature::Feature( const XmlTree::Node& node ) : 
  BaseClass( node ),
  _name(),
  _visiblity ( true ),
  _lookAt ( 0x0 )
{
  typedef XmlTree::Node::Children Children;
  
  Children children ( node.children() );
  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( *iter );
    std::string name ( node->name() );
    
    if ( "name" == name )
    {
      _name = node->value();
    }
    else if ( "visibility" == name )
    {
      bool visible ( "0" != node->value() );
      _visiblity = visible;
    }
    else if ( "LookAt" == name )
    {
      _lookAt = new LookAt ( *node );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Feature::~Feature()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name.
//
///////////////////////////////////////////////////////////////////////////////

const std::string& Feature::name() const
{
  return _name;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the name.
//
///////////////////////////////////////////////////////////////////////////////

void Feature::name ( const std::string& s )
{
  _name = s;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the visiblity.
//
///////////////////////////////////////////////////////////////////////////////

bool Feature::visiblity() const
{
  return _visiblity;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the visiblity.
//
///////////////////////////////////////////////////////////////////////////////

void Feature::visiblity( bool b )
{
  _visiblity = b;
}
