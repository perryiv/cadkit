
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/Kml/ColorStyle.h"

#include "Usul/Adaptors/Random.h"

#include "XmlTree/Node.h"

#include <cstdlib>
#include <ctime>

using namespace Minerva::Layers::Kml;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ColorStyle::ColorStyle() : BaseClass(),
	_color( 1.0, 1.0, 1.0, 1.0 ),
  _mode ( NORMAL )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ColorStyle::ColorStyle ( const XmlTree::Node& node ) : BaseClass ( node ),
	_color( 1.0, 1.0, 1.0, 1.0 ),
  _mode ( NORMAL )
{
	typedef XmlTree::Node::Children Children;
  
  Children children ( node.children() );
  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( *iter );
    std::string name ( node->name() );
    
    if ( "color" == name )
    {
			const unsigned long c ( ::strtoul ( node->value().c_str(), 0x0, 16 ) );

			_color[3] = static_cast<float> ( ( ( c & 0xff000000 ) >> 24 ) / 255.0 );
			_color[0] = static_cast<float> ( ( ( c & 0x00ff0000 ) >> 16 ) / 255.0 );
			_color[1] = static_cast<float> ( ( ( c & 0x0000ff00 ) >>  8 ) / 255.0 );
			_color[2] = static_cast<float> ( ( ( c & 0x000000ff )       ) / 255.0 );
    }
    else if ( "colorMode" == name )
    {
      _mode = ( "random" == node->value() ? RANDOM : NORMAL );
    }
  }

  // Make a random color if we are suppose to.
  if ( RANDOM == _mode )
  {
    ::srand ( static_cast<unsigned int> ( ::time ( 0 ) ) );
    Usul::Adaptors::Random<float> random ( 0.0, 1.0 );
    _color[0] = _color[0] * random();
    _color[1] = _color[1] * random();
    _color[2] = _color[2] * random();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ColorStyle::~ColorStyle()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the color.
//
///////////////////////////////////////////////////////////////////////////////

void ColorStyle::color ( const Color& c )
{
	_color = c;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the color.
//
///////////////////////////////////////////////////////////////////////////////

const ColorStyle::Color& ColorStyle::color() const
{
	return _color;
}
