
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Legend/Text.h"

#include "OsgTools/Font.h"

#include "osg/Geode"
#include "osgText/Text"

using namespace OsgTools::Legend;


Text::Text() : BaseClass(), 
_text(),
_width( 0 ),
_height( 0 )
{
}

Text::~Text()
{
}

void Text::text( const std::string& t )
{
  _text = t;
}

const std::string& Text::text() const
{
  return _text;
}

void Text::width( unsigned int w )
{
  _width = w;
}

unsigned int Text::width() const
{
  return _width;
}

void Text::height( unsigned int h )
{
  _height = h;
}

unsigned int Text::height() const
{
  return _height;
}


osg::Node* Text::buildScene()
{
  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );

  osg::ref_ptr < osgText::Text > text ( new osgText::Text );

  text->setFont( OsgTools::Font::defaultFont() );
  text->setPosition ( osg::Vec3( 0.0, _height / 4, 0.0 ) );
  text->setAutoRotateToScreen( true );
  text->setCharacterSizeMode( osgText::Text::SCREEN_COORDS );
  text->setCharacterSize( _height );
  //text->setBackdropType ( osgText::Text::DROP_SHADOW_BOTTOM_RIGHT );
  //text->setBackdropImplementation ( osgText::Text::STENCIL_BUFFER );
  //text->setMaximumWidth( _width );
  //text->setCharacterSize( _height );
  //text->setAlignment( osgText::Text::LEFT_CENTER );

  text->setText ( this->text() );

  geode->addDrawable( text.get() );

  return geode.release();
}
