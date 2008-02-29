
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/Kml/LookAt.h"

#include "Usul/Convert/Convert.h"

#include "XmlTree/Node.h"

using namespace Minerva::Layers::Kml;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

LookAt::LookAt() : 
  BaseClass(),
  _longitude ( 0.0 ),
  _latitude ( 0.0 ),
  _altitude ( 0.0 ),
  _range ( 0.0 ),
  _heading ( 0.0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

LookAt::LookAt( const XmlTree::Node& node ) : 
  BaseClass( node ),
  _longitude ( 0.0 ),
  _latitude ( 0.0 ),
  _altitude ( 0.0 ),
  _range ( 0.0 ),
  _heading ( 0.0 )
{
  typedef XmlTree::Node::Children Children;
  typedef Usul::Convert::Type<std::string,double> ToDouble;
  
  Children children ( node.children() );
  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( *iter );
    std::string name ( node->name() );
    
    if ( "longitude" == name )
    {
      _longitude = ToDouble::convert ( node->value() );
    }
    else if ( "latitude" == name )
    {
      _latitude = ToDouble::convert ( node->value() );
    }
    else if ( "altitude" == name )
    {
      _altitude = ToDouble::convert ( node->value() );
    }
    else if ( "range" == name )
    {
      _range = ToDouble::convert ( node->value() );
    }
    else if ( "heading" == name )
    {
      _heading = ToDouble::convert ( node->value() );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

LookAt::~LookAt()
{
}
