
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class to make text.
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Text.h"

#include "Usul/Errors/Checker.h"

#include "osg/Geode"
#include "osg/Projection"
#include "osg/MatrixTransform"

#include "osgText/Text"

using namespace OsgTools;

typedef Usul::Errors::Checker ErrorChecker;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Text::Text() :
  _color    ( 1, 0, 0, 1 ),
  _font     ( "arial.ttf" ),
  _text     ( new osgText::Text )
{
  ErrorChecker ( 1071443291, 0x0 != _text );
  _text->ref();
}


///////////////////////////////////////////////////////////////////////////////
//
// Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Text::~Text()
{
  ErrorChecker ( 1071443292, 0x0 != _text );
  _text->unref();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the Text.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Text::operator()() const
{
  ErrorChecker ( 1071443909, 0x0 != _text );

  // The parent node.
  osg::ref_ptr<osg::Geode> geode ( new osg::Geode() );
  ErrorChecker ( 1071443907, geode.valid() );

  // Make sure it's visible and on top.
  osg::ref_ptr<osg::StateSet> ss ( geode->getOrCreateStateSet() );
  ErrorChecker ( 1071443908, ss.valid() );
  ss->setMode ( GL_LIGHTING, osg::StateAttribute::OFF );
  ss->setMode ( GL_DEPTH_TEST, osg::StateAttribute::OFF );
  ss->setRenderBinDetails ( 11, "RenderBin" );

  // Add the text to the geode.
  geode->addDrawable ( _text );

  // Return the geode.
  return geode.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the position.
//
///////////////////////////////////////////////////////////////////////////////

void Text::position ( const osg::Vec3 &p )
{
  ErrorChecker ( 1071447532, 0x0 != _text );
  _text->setPosition ( p );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the position.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec3& Text::position() const
{
  ErrorChecker ( 1071447533, 0x0 != _text );
  return _text->getPosition();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the font.
//
///////////////////////////////////////////////////////////////////////////////

void Text::font ( const std::string &f )
{
  ErrorChecker ( 1071447534, 0x0 != _text );
  _text->setFont ( f );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the font.
//
///////////////////////////////////////////////////////////////////////////////

std::string Text::font() const
{
  ErrorChecker ( 1071447535, 0x0 != _text );
  return _text->getFont()->getFileName();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the text.
//
///////////////////////////////////////////////////////////////////////////////

void Text::text ( const std::string &t )
{
  ErrorChecker ( 1071447536, 0x0 != _text );

  // Have to tell it to update too.
  _text->setText ( t );
  _text->update();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the text.
//
///////////////////////////////////////////////////////////////////////////////

std::string Text::text() const
{
  ErrorChecker ( 1071447537, 0x0 != _text );
  return std::string ( _text->getText().begin(), _text->getText().end() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Compare text.
//
///////////////////////////////////////////////////////////////////////////////

bool Text::equal ( const std::string &s ) const
{
  ErrorChecker ( 1071456494, 0x0 != _text );

  // See if the sizes are different.
  if ( _text->getText().size() != s.size() )
    return false;

  // Compare the characters. The text holds a vector of chars.
  return std::equal ( _text->getText().begin(), _text->getText().end(), s.begin() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the font height.
//
///////////////////////////////////////////////////////////////////////////////

float Text::height() const
{
  ErrorChecker ( 1071459441, 0x0 != _text );
  return _text->getCharacterHeight();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the font height.
//
///////////////////////////////////////////////////////////////////////////////

void Text::height ( float h )
{
  ErrorChecker ( 1071459438, 0x0 != _text );
  _text->setCharacterSize ( h );
}