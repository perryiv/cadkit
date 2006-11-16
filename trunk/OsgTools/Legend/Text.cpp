
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
_text()
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

osg::Node* Text::buildScene()
{
  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );

  osg::ref_ptr < osgText::Text > text ( new osgText::Text );

  text->setFont( OsgTools::Font::defaultFont() );
  text->setPosition ( osg::Vec3( 0.0,0.0,0.0 ) );
  text->setAutoRotateToScreen( true );
  text->setCharacterSizeMode( osgText::Text::SCREEN_COORDS );
  text->setCharacterSize(25);

  text->setText ( this->text() );

  geode->addDrawable( text.get() );

  return geode.release();
}
