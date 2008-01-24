
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Widgets/Axes.h"
#include "OsgTools/Builders/Arrow.h"
#include "OsgTools/State/StateSet.h"
#include "OsgTools/Font.h"

#include "osg/Geode"
#include "osg/PolygonMode"
#include "osg/Geometry"

#include "osgText/Text"
#include "osgText/Font"

#include <algorithm>

using namespace OsgTools::Widgets;


///////////////////////////////////////////////////////////////////////////////
//
//  Predicate to sort back to front.
//
///////////////////////////////////////////////////////////////////////////////

struct BackToFront
{
  BackToFront ( const osg::Matrix &matrix ) : _matrix ( matrix )
  {
  }

  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Return true if a is farther back than b
  //
  ///////////////////////////////////////////////////////////////////////////////

  template < class Primitive > bool operator () ( const Primitive &a, const Primitive &b ) const
  {
    osg::Geometry *geomZero ( static_cast < osg::Geometry* > ( a->getDrawable ( 0 ) ) );
    osg::Geometry *geomOne  ( static_cast < osg::Geometry* > ( b->getDrawable  ( 0 ) ) );

    osg::Vec3Array *vertsZero ( static_cast < osg::Vec3Array* > ( geomZero->getVertexArray() ) );
    osg::Vec3Array *vertsOne  ( static_cast < osg::Vec3Array* > ( geomOne->getVertexArray()  ) );

    osg::Vec3 vecZero ( vertsZero->at( 1 ) * _matrix );
    osg::Vec3 vecOne  ( vertsOne->at ( 1 ) * _matrix );

    return vecZero.z() < vecOne.z();
  }

private:
  const osg::Matrix& _matrix;

};


///////////////////////////////////////////////////////////////////////////////
//
//  Axes Sort.  This happens every frame, so trying to optimize for speed.
//
///////////////////////////////////////////////////////////////////////////////

void Axes::AxesSort::operator () ( osg::Node* node, osg::NodeVisitor* nv )
{
  osg::MatrixTransform *mt ( static_cast < osg::MatrixTransform* > ( node ) );
  osg::Group *group ( static_cast < osg::Group* > ( mt->getChild ( 0 ) ) );

  // Use ref counting on geodes, so they don't get deleted as we shuffle them in the group.
  osg::ref_ptr< osg::Geode > zero ( static_cast < osg::Geode* > ( group->getChild ( 0 ) ) );
  osg::ref_ptr< osg::Geode > one  ( static_cast < osg::Geode* > ( group->getChild ( 1 ) ) );
  osg::ref_ptr< osg::Geode > two  ( static_cast < osg::Geode* > ( group->getChild ( 2 ) ) );

  const osg::Matrix &matrix ( mt->getMatrix() );

  std::vector < osg::Geode* > geodes ( 3 );
  geodes[0] = zero.get();
  geodes[1] = one.get();
  geodes[2] = two.get();

  std::sort ( geodes.begin(), geodes.end(), BackToFront( matrix ) );

  group->setChild ( 0, geodes[0] );
  group->setChild ( 1, geodes[1] );
  group->setChild ( 2, geodes[2] );

  this->traverse(node,nv);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Axes::Axes() : BaseClass(),
  _xText ( 0x0 ),
  _yText ( 0x0 ),
  _zText ( 0x0 )
{
  // Length of the arrow.
  const double length ( 40.0 );

  osg::Vec3 origin ( 0.0, 0.0, 0.0 );
  osg::Vec3 x  ( length, 0.0, 0.0 );
  osg::Vec3 y  ( 0.0, length, 0.0 );
  osg::Vec3 z  ( 0.0, 0.0, length );

  // Radius and height for cones.
  const double radius ( 3.0 );
  const double height ( 10.0 );

  // Colors for arrows.
  osg::Vec4 colorX ( 1.0, 0.0, 0.0, 1.0 );
  osg::Vec4 colorY ( 0.15, 0.55, 0.15, 1.0 );
  osg::Vec4 colorZ ( 0.0, 0.0, 1.0, 1.0 );

  // The X arrow.
  OsgTools::Builders::Arrow arrowX;
  arrowX.start ( origin );
  arrowX.end ( x );
  arrowX.color ( colorX );
  arrowX.radius ( radius );
  arrowX.height ( height );

  // The Y arrow.
  OsgTools::Builders::Arrow arrowY;
  arrowY.start ( origin );
  arrowY.end ( y );
  arrowY.color ( colorY );
  arrowY.radius ( radius );
  arrowY.height ( height );

  // The Z arrow.
  OsgTools::Builders::Arrow arrowZ;
  arrowZ.start ( origin );
  arrowZ.end ( z );
  arrowZ.color ( colorZ );
  arrowZ.radius ( radius );
  arrowZ.height ( height );

  // Geode for the text labels.
  osg::ref_ptr< osg::Geode > texts ( new osg::Geode );

  // Turn off lighting.
  OsgTools::State::StateSet::setLighting ( texts.get(), false );

  const double size ( 15.0f );

  osg::ref_ptr<osgText::Font> font ( OsgTools::Font::defaultFont() );

  //Text for X
  _xText = this->_buildText ( colorX, osg::Vec3 ( length + height + 1, 0.0, 0.0 ), "X", size );
  texts->addDrawable( _xText.get() );

  //Text for Y
  _yText = this->_buildText ( colorY, osg::Vec3 ( 0.0, length + height + 1, 0.0 ), "Y", size );
  texts->addDrawable( _yText.get() );

  //Text for Z
  _zText = this->_buildText ( colorZ, osg::Vec3 ( 0.0, 0.0, length + height + 1 ), "Z", size );
  texts->addDrawable( _zText.get() );

  // Set the proper states.
  osg::ref_ptr< osg::StateSet > ss ( this->getOrCreateStateSet() );

  // Make sure it's drawn last ( on top )
  ss->setRenderBinDetails ( 100, "RenderBin" );
	ss->setMode ( GL_DEPTH_TEST, osg::StateAttribute::OFF );
  ss->setMode ( GL_CULL_FACE,  osg::StateAttribute::ON );

  // Turn on lighting.  Protected ignores state from above.
  ss->removeMode ( GL_LIGHTING );
  ss->setMode ( GL_LIGHTING,  osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );

  // Set the polygon-mode.  This makes sure everything stays filled
  osg::ref_ptr<osg::PolygonMode> mode ( new osg::PolygonMode );
  mode->setMode ( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL );
  ss->setAttributeAndModes ( mode.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );

  osg::ref_ptr< osg::Group > group ( new osg::Group );

  // Add all geodes under the group.
  group->addChild( arrowX() );
  group->addChild( arrowY() );
  group->addChild( arrowZ() );
  group->addChild( texts.get() );

  this->addChild( group.get() );
  
  this->setCullCallback ( new AxesSort );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Axes::~Axes()
{
}



///////////////////////////////////////////////////////////////////////////////
//
//  Set the text for the x label.
//
///////////////////////////////////////////////////////////////////////////////

void Axes::xText ( const std::string& label )
{
  if ( _xText.valid() )
  {
    _xText->setText ( label );
    _xText->update();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the text for the x label.
//
///////////////////////////////////////////////////////////////////////////////

std::string Axes::xText() const
{
  std::string text ( "" );

  // Copy the text.
  if ( _xText.valid() )
    text.assign ( _xText->getText().begin(), _xText->getText().end() );

  return text;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the text for the y label.
//
///////////////////////////////////////////////////////////////////////////////

void Axes::yText ( const std::string& label )
{
  if ( _yText.valid() )
  {
    _yText->setText ( label );
    _yText->update();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the text for the y label.
//
///////////////////////////////////////////////////////////////////////////////

std::string Axes::yText() const
{
  std::string text ( "" );

  // Copy the text.
  if ( _yText.valid() )
    text.assign ( _yText->getText().begin(), _yText->getText().end() );

  return text;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the text for the z label.
//
///////////////////////////////////////////////////////////////////////////////

void Axes::zText ( const std::string& label )
{
  if ( _zText.valid() )
  {
    _zText->setText ( label );
    _zText->update();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the text for the z label.
//
///////////////////////////////////////////////////////////////////////////////

std::string Axes::zText() const
{
  std::string text ( "" );

  // Copy the text.
  if ( _zText.valid() )
    text.assign ( _zText->getText().begin(), _zText->getText().end() );

  return text;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build an osgText::Text.
//
///////////////////////////////////////////////////////////////////////////////

osgText::Text* Axes::_buildText( const osg::Vec4& color, const osg::Vec3& position, const std::string& label, double size ) const
{
  // Get the default font.
  osg::ref_ptr<osgText::Font> font ( OsgTools::Font::defaultFont() );

  // Make the text.
  osg::ref_ptr< osgText::Text > text ( new osgText::Text );
  text->setFont( font.get() );
  text->setColor( color );
  text->setCharacterSize( size );
  text->setPosition ( position );
  text->setText( label );
  text->setAutoRotateToScreen( true );
  text->setFontResolution( 40,40 );
  text->update();

  text->setUseDisplayList( false );
  return text.release();
}
