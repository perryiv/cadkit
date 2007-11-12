
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


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Text::Text() : 
  BaseClass(), 
  _text(),
  _width( 0 ),
  _height( 0 ),
  _alignment ( LEFT )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Text::Text( const std::string& text ) : 
  BaseClass(), 
  _text( text ),
  _width( 0 ),
  _height( 0 ),
  _alignment ( LEFT )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Text::~Text()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the text.
//
///////////////////////////////////////////////////////////////////////////////

void Text::text( const std::string& t )
{
  _text = t;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the text.
//
///////////////////////////////////////////////////////////////////////////////

const std::string& Text::text() const
{
  return _text;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the width.
//
///////////////////////////////////////////////////////////////////////////////

void Text::width( unsigned int w )
{
  _width = w;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the width.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Text::width() const
{
  return _width;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the height.
//
///////////////////////////////////////////////////////////////////////////////

void Text::height( unsigned int h )
{
  _height = h;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the height.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Text::height() const
{
  return _height;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Text::buildScene()
{
  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );

  osg::ref_ptr < osgText::Text > text ( new osgText::Text );

  text->setFont( OsgTools::Font::defaultFont() );

  if ( LEFT == _alignment )
    text->setPosition ( osg::Vec3( 0.0, _height / 4, 0.0 ) );
  else if ( RIGHT == _alignment )
  {
    text->setPosition ( osg::Vec3( _width, _height / 4, 0.0 ) );
    text->setAlignment ( osgText::Text::RIGHT_BASE_LINE );
  }

  text->setAutoRotateToScreen( true );
  text->setCharacterSizeMode( osgText::Text::SCREEN_COORDS );
  //text->setCharacterSizeMode( osgText::Text::OBJECT_COORDS );
  text->setCharacterSize( _height );
  text->setText ( this->text() );
  

  while ( ( OsgTools::Font::estimateTextWidth( text.get() ) * .75 ) > _width )
  {
    _height -= 2;
    text->setCharacterSize ( _height );
  }

  geode->addDrawable( text.get() );

  return geode.release();
}


/// Get/Set the alignment.
void Text::alignment ( Alignment type )
{
  _alignment = type;
}

Text::Alignment Text::alignment () const
{
  return _alignment;
}
