
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Widgets/Text.h"

#include "OsgTools/Font.h"
#include "OsgTools/State/StateSet.h"

#include "osg/Geode"
#include "osgText/Text"

#include <iostream>
#include <stdexcept>

using namespace OsgTools::Widgets;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Text::Text() : 
  BaseClass(), 
  _text(),
  _alignment ( LEFT ),
  _alignmentVertical ( CENTER ),
  _wrapLine ( false ),
  _autoSize ( true ),
  _fontSize ( 25.0 ),
  _textColor ( 1.0, 1.0, 1.0, 1.0 )
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
  _alignment ( LEFT ),
  _alignmentVertical ( CENTER ),
  _wrapLine ( false ),
  _autoSize ( true ),
  _fontSize ( 25.0 ),
  _textColor ( 1.0, 1.0, 1.0, 1.0 )
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
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Text::buildScene ( unsigned int depth )
{
  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );

  osg::ref_ptr<osgText::Text> text ( this->_makeText ( depth ) );
  geode->addDrawable( text.get() );
  
  // Set our state.
  osg::ref_ptr<osg::StateSet> ss ( geode->getOrCreateStateSet() );
  //ss->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );
  ss->setMode( GL_BLEND, osg::StateAttribute::ON  | osg::StateAttribute::PROTECTED | osg::StateAttribute::OVERRIDE );

  // Turn off lighting.
  OsgTools::State::StateSet::setLighting ( ss.get(), false );

  // Set the correct render order.
  Item::_setState ( ss.get(), depth );

  // For debuging placements...
#if 0
  this->backgroundColor ( osg::Vec4 ( 1.0, 0.0, 0.0, 1.0 ) );
  osg::ref_ptr<osg::Group> group ( new osg::Group );
  group->addChild ( this->_buildBackground ( this->size()[0], this->size()[1] ) );
  group->addChild ( geode.get() );
  
  return group.release();
#else
  return geode.release();
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the alignment.
//
///////////////////////////////////////////////////////////////////////////////

void Text::alignmentHorizontal ( Alignment type )
{
  _alignment = type;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the alignment.
//
///////////////////////////////////////////////////////////////////////////////

Text::Alignment Text::alignmentHorizontal () const
{
  return _alignment;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the vertical alignment.
//
///////////////////////////////////////////////////////////////////////////////

void Text::alignmentVertical ( VAlignment type )
{
  _alignmentVertical = type;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the vertical alignment.
//
///////////////////////////////////////////////////////////////////////////////

Text::VAlignment Text::alignmentVertical() const
{
  return _alignmentVertical;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the line wrap state.
//
///////////////////////////////////////////////////////////////////////////////

void Text::wrapLine ( bool b )
{
  _wrapLine = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the line wrap state.
//
///////////////////////////////////////////////////////////////////////////////

bool Text::wrapLine() const
{
  return _wrapLine;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the font size.
//
///////////////////////////////////////////////////////////////////////////////

void Text::fontSize ( double size )
{
  _fontSize = size;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the font size.
//
///////////////////////////////////////////////////////////////////////////////

double Text::fontSize() const
{
  return _fontSize;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the auto size.
//
///////////////////////////////////////////////////////////////////////////////

void Text::autoSize ( bool b )
{
  _autoSize = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the auto size.
//
///////////////////////////////////////////////////////////////////////////////

bool Text::autoSize() const
{
  return _autoSize;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get x position on text based on alignment settings.
//
///////////////////////////////////////////////////////////////////////////////

Text::ValueType Text::_xTextPosition( unsigned int width ) const
{
  switch ( this->alignmentHorizontal() )
  {
  case Text::LEFT:
    return 0.0;
  case Text::RIGHT:
    return width;
  }
  
  return 0.0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get y position on text based on alignment settings.
//
///////////////////////////////////////////////////////////////////////////////

Text::ValueType Text::_yTextPosition( unsigned int height ) const
{
  switch ( this->alignmentVertical() )
  {
    case Text::TOP:
      return static_cast<ValueType> ( height ) - ( this->fontSize() / 2.0 );
    case Text::CENTER:
      return static_cast<ValueType> ( height ) / 4;
    case Text::BOTTOM:
      return this->fontSize() / 2.0;
  }
  
  return 0.0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Estimate the needed size for the item.
//
///////////////////////////////////////////////////////////////////////////////

Text::Size Text::estimateSizeForWidth ( unsigned int w ) const
{
  osg::ref_ptr<osgText::Text> text ( this->_makeText ( 0 ) );
  text->update();
  osg::BoundingBox bb ( text->computeBound() );
  
  //SizeType height ( text->getLineCount() * this->fontSize() );
  //SizeType width ( this->size()[0] );
  
  SizeType height ( bb.yMax() - bb.yMin() );
  SizeType width ( bb.xMax() - bb.xMin() );
  return Size ( width, height );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make an osgText::Text object.
//
///////////////////////////////////////////////////////////////////////////////

osgText::Text* Text::_makeText ( unsigned int depth ) const
{
  const SizeType width  ( this->size()[0] );
  SizeType height ( this->size()[1] );

  if ( ( 0 == width ) || ( 0 == height ) )
  {
    throw std::runtime_error ( "Error 2114860216: Cannot make text with height or width of zero" );
  }

  osg::ref_ptr < osgText::Text > text ( new osgText::Text );
  text->setFont( OsgTools::Font::defaultFont() );
  
  // Set the color.
  text->setColor ( this->textColor() );
  
  // Set the position.
  text->setPosition ( osg::Vec3 ( this->_xTextPosition ( width ), this->_yTextPosition ( height ), 0.0 ) );
  
  if ( RIGHT == this->alignmentHorizontal() )
  {
    text->setAlignment ( osgText::Text::RIGHT_BASE_LINE );
  }
  
  text->setCharacterSizeMode ( osgText::Text::OBJECT_COORDS );
  text->setCharacterSize ( this->fontSize() );
  text->setText ( this->text() );
  
  if ( this->wrapLine() )
  {
    text->setMaximumWidth ( width );
  }
  else if ( this->autoSize() )
  {
    // Resize the fit on the line.
    while ( ( OsgTools::Font::estimateTextWidth( text.get() ) * .75 ) > width )
    {
      height -= 2;
      text->setCharacterSize ( height );
    }
  }
  
  return text.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the border color.
//
///////////////////////////////////////////////////////////////////////////////

void Text::textColor ( const Color& c )
{
  _textColor = c;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the border color.
//
///////////////////////////////////////////////////////////////////////////////

Text::Color Text::textColor() const
{
  return _textColor;
}
