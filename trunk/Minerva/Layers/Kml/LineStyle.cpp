
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/Kml/LineStyle.h"

#include "Usul/Convert/Convert.h"

#include "XmlTree/Node.h"

using namespace Minerva::Layers::Kml;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

LineStyle::LineStyle() : BaseClass(),
	_width ( 1.0f )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

LineStyle::LineStyle( const XmlTree::Node &node ) : BaseClass( node ),
	_width ( 1.0f )
{
  typedef XmlTree::Node::Children Children;
  
  Children children ( node.children() );
  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( *iter );
    std::string name ( node->name() );
    
    if ( "width" == name )
    {
      _width = Usul::Convert::Type<std::string, float>::convert ( node->value() );
    }
  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

LineStyle::~LineStyle()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the width.
//
///////////////////////////////////////////////////////////////////////////////

void LineStyle::width ( float w )
{
  _width = w;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the width.
//
///////////////////////////////////////////////////////////////////////////////

float LineStyle::width() const
{
  return _width;
}
