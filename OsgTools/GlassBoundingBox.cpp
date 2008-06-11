
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Mike Jackson
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////


#include "OsgTools/GlassBoundingBox.h"
#include "OsgTools/State/StateSet.h"
#include "OsgTools/Callbacks/SortBackToFront.h"
#include "OsgTools/Font.h"
#include "OsgTools/ShapeFactory.h"
#include "OsgTools/DisplayLists.h"

#include "osg/Group"
#include "osg/Geode"
#include "osg/Geometry"
#include "osg/Vec4"
#include "osg/LineWidth"
#include "osg/StateSet"
#include "osg/AlphaFunc"
#include "osg/StateSet"
#include "osg/Material"
#include "osg/MatrixTransform"

#include "osgText/Text"

#include <algorithm>
#include <functional>
#include <limits>
#include <iostream>

using namespace OsgTools;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

GlassBoundingBox::GlassBoundingBox() : BaseClass(),
_xMax (std::numeric_limits< float >::min()),
_xMin (std::numeric_limits< float >::max()),
_yMax (std::numeric_limits< float >::min()),
_yMin (std::numeric_limits< float >::max()),
_zMax (std::numeric_limits< float >::min()),
_zMin (std::numeric_limits< float >::max())
{
  
}

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

GlassBoundingBox::GlassBoundingBox(
        const float xMn, const float yMn, float const zMn,
        const float xMx, const float yMx, float const zMx): BaseClass()

{
  _xMax = xMx;
  _xMin = xMn;
  _yMax = yMx;
  _yMin = yMn;
  _zMax = zMx;
  _zMin = zMn;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

GlassBoundingBox::GlassBoundingBox( const osg::BoundingBox& bb ) : 
BaseClass(),
_xMax ( bb.xMax() ),
_xMin ( bb.xMin() ),
_yMax ( bb.yMax() ),
_yMin ( bb.yMin() ),
_zMax ( bb.zMax() ),
_zMin ( bb.zMin() )
{
  
}

///////////////////////////////////////////////////////////////////////////////
//
// Operator () overload
//
///////////////////////////////////////////////////////////////////////////////

void GlassBoundingBox::operator() ( osg::Group *root, bool outline, bool glass, bool numbers ) 
{
  osg::BoundingBox bb( _xMin,_yMin,_zMin,_xMax,_yMax,_zMax );

  // Make the vertices
  osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array );
  
  // Fill the vertices
  for( unsigned int i = 0; i < 8; ++i )
    vertices->push_back ( bb.corner( i ) );

  if( outline )
    root->addChild ( this->_makeBoxOutline ( *vertices ) );

  if( glass )
  {
    root->addChild ( this->_makeBoxGlass ( *vertices ) );
  }

  // Add numbers to the bounding box.  This is mainly for debugging.
  if( numbers )
    root->addChild( this->_makeNumbers() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the node
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* GlassBoundingBox::operator() ( bool outline, bool glass, bool numbers )
{
  osg::ref_ptr < osg::Group > group ( new osg::Group );
  ( *this ) ( group.get(), outline, glass, numbers );
  return group.release ();
}


///////////////////////////////////////////////////////////////////////////////
//
// Creates the "Box" outline
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* GlassBoundingBox::_makeBoxOutline ( osg::Vec3Array& vertices )
{
  osg::Vec4 yellow ( 1.0f, 1.0f, 0.0f, 1.0f );

#if 1
  typedef osg::DrawElementsUInt DrawElements;
  
  osg::ref_ptr< DrawElements > lines  ( new DrawElements ( osg::PrimitiveSet::LINES, 0 ) );
  
  lines->push_back ( 0 );
  lines->push_back ( 1 );
  lines->push_back ( 0 );
  lines->push_back ( 2 );
  lines->push_back ( 0 );
  lines->push_back ( 4 );
  
  lines->push_back ( 6 );
  lines->push_back ( 2 );
  lines->push_back ( 6 );
  lines->push_back ( 4 );
  lines->push_back ( 6 );
  lines->push_back ( 7 );
  
  lines->push_back ( 3 );
  lines->push_back ( 2 );
  lines->push_back ( 3 );
  lines->push_back ( 7 );
  lines->push_back ( 3 );
  lines->push_back ( 1 );
  
  lines->push_back ( 5 );
  lines->push_back ( 7 );
  lines->push_back ( 5 );
  lines->push_back ( 1 );
  lines->push_back ( 5 );
  lines->push_back ( 4 );
    
  osg::ref_ptr < osg::Geometry > geometry ( new osg::Geometry );

  // Set the vertices.
  geometry->setVertexArray  ( &vertices );

  // Add the primitive sets.
  geometry->addPrimitiveSet ( lines.get() );
  
  osg::ref_ptr < osg::Vec4Array > colors ( new osg::Vec4Array );
  
  osg::ref_ptr < osg::Geode > geode ( new osg::Geode );
  
  colors->resize ( 12 );

  std::fill( colors->begin(), colors->end(), yellow );
  
  geometry->setColorArray ( colors.get() );
  geometry->setColorBinding ( osg::Geometry::BIND_PER_PRIMITIVE );
  
  // Set the line-width.
  OsgTools::State::StateSet::setLineWidth ( geode.get(), 2.0f );

  // Turn off lighting.
  OsgTools::State::StateSet::setLighting ( geode.get(), false );
  
  geode->addDrawable ( geometry.get() );

  osg::ref_ptr < osg::StateSet > ss ( geode->getOrCreateStateSet() );

  // Turn off back face culling
  //ss->setMode ( GL_CULL_FACE, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF );

  return geode.release();
#else
  osg::BoundingBox bb( _xMin,_yMin,_zMin,_xMax,_yMax,_zMax );

  osg::ref_ptr < osg::Group > group ( new osg::Group );
  osg::StateSet *ss ( group->getOrCreateStateSet () );

  osg::Material *mat ( new osg::Material );
  //mat->setDiffuse ( osg::Material::FRONT_AND_BACK, osg::Vec4 ( 255.0 / 255.0, 164.5 / 255.0, 85.0 / 255.0, 1 ) );
  mat->setDiffuse ( osg::Material::FRONT_AND_BACK, yellow );
  mat->setAmbient ( osg::Material::FRONT_AND_BACK, yellow );

  ss->setAttribute( mat, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );

  // Turn on lighting.
  OsgTools::State::StateSet::setLighting ( group.get(), true );

  float radius ( 100.0f );

  osg::ref_ptr< OsgTools::ShapeFactory > sf ( new OsgTools::ShapeFactory );
  osg::Geode *geode ( new osg::Geode );
  geode->addDrawable ( sf->cylinder ( radius, 20, bb.corner( 4 ), bb.corner( 5 ) ) );
  geode->addDrawable ( sf->cylinder ( radius, 20, bb.corner( 5 ), bb.corner( 7 ) ) );
  geode->addDrawable ( sf->cylinder ( radius, 20, bb.corner( 7 ), bb.corner( 6 ) ) );
  geode->addDrawable ( sf->cylinder ( radius, 20, bb.corner( 6 ), bb.corner( 4 ) ) );
  geode->addDrawable ( sf->cylinder ( radius, 20, bb.corner( 3 ), bb.corner( 7 ) ) );
  geode->addDrawable ( sf->cylinder ( radius, 20, bb.corner( 1 ), bb.corner( 5 ) ) );
  geode->addDrawable ( sf->cylinder ( radius, 20, bb.corner( 3 ), bb.corner( 2 ) ) );
  geode->addDrawable ( sf->cylinder ( radius, 20, bb.corner( 1 ), bb.corner( 0 ) ) );
  geode->addDrawable ( sf->cylinder ( radius, 20, bb.corner( 3 ), bb.corner( 1 ) ) );
  geode->addDrawable ( sf->cylinder ( radius, 20, bb.corner( 2 ), bb.corner( 0 ) ) );
  geode->addDrawable ( sf->cylinder ( radius, 20, bb.corner( 2 ), bb.corner( 6 ) ) );
  geode->addDrawable ( sf->cylinder ( radius, 20, bb.corner( 4 ), bb.corner( 0 ) ) );

  group->addChild ( geode );
  
  for ( unsigned int i = 0; i < 8; ++i )
  {
    osg::ref_ptr < osg::Geode > g ( new osg::Geode );
    osg::ref_ptr < osg::MatrixTransform > mt ( new osg::MatrixTransform );
    mt->setMatrix ( osg::Matrix::translate ( bb.corner( i ) ) );

    mt->addChild ( g.get () );
    group->addChild ( mt.get() );

    OsgTools::ShapeFactory::MeshSize size ( 20, 20 );
    g->addDrawable ( sf->sphere ( radius * 1.2f, size ) );
  }
  
  OsgTools::State::StateSet::setLighting ( group.get(), true );

  // Turn off back face culling
  ss->setMode ( GL_CULL_FACE, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF );

  // Turn off display lits.
  //OsgTools::DisplayLists dl ( false );
  //dl ( group.get() );

  return group.release( );
#endif
}

///////////////////////////////////////////////////////////////////////////////
//
// Creates the transparent "sides" on the bounding box
//
///////////////////////////////////////////////////////////////////////////////
 
osg::Node* GlassBoundingBox::_makeBoxGlass   ( osg::Vec3Array& vertices )
{
  osg::ref_ptr < osg::Geode > geode ( new osg::Geode );

  // Set state for transparency
  osg::ref_ptr< osg::StateSet > stateset ( geode->getOrCreateStateSet() );
  stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
  osg::AlphaFunc* alphaFunc = new osg::AlphaFunc;
  alphaFunc->setFunction(osg::AlphaFunc::GEQUAL,0.05f);
  stateset->setAttributeAndModes( alphaFunc,osg::StateAttribute::ON );
  stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
  stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
  //stateset->setMode(GL_CULL_FACE,osg::StateAttribute::ON );
  //geode->setCullCallback( new OsgTools::SortBackToFrontCallback );

  typedef osg::DrawElementsUInt DrawElements;
  osg::ref_ptr< osg::Vec3Array > box ( new osg::Vec3Array );

  // Bottom quad. ( XZ min )
  box->push_back ( vertices.at ( 0 ) );
  box->push_back ( vertices.at ( 1 ) );
  box->push_back ( vertices.at ( 5 ) );
  box->push_back ( vertices.at ( 4 ) );

  // Top quad. ( XZ max )
  box->push_back ( vertices.at ( 2 ) );
  box->push_back ( vertices.at ( 3 ) );
  box->push_back ( vertices.at ( 7 ) );
  box->push_back ( vertices.at ( 6 ) );

  // Left quad. ( YZ min )
  box->push_back ( vertices.at ( 0 ) );
  box->push_back ( vertices.at ( 2 ) );
  box->push_back ( vertices.at ( 6 ) );
  box->push_back ( vertices.at ( 4 ) );

  // Right quad. ( YZ max )
  box->push_back ( vertices.at ( 5 ) );
  box->push_back ( vertices.at ( 7 ) );
  box->push_back ( vertices.at ( 3 ) );
  box->push_back ( vertices.at ( 1 ) );

  // Front quad. ( XY max )
  box->push_back ( vertices.at ( 6 ) );
  box->push_back ( vertices.at ( 7 ) );
  box->push_back ( vertices.at ( 5 ) );
  box->push_back ( vertices.at ( 4 ) );

  // Back quad. ( XY min )
  box->push_back ( vertices.at ( 2 ) );
  box->push_back ( vertices.at ( 3 ) );
  box->push_back ( vertices.at ( 1 ) );
  box->push_back ( vertices.at ( 0 ) );

  osg::ref_ptr< osg::Geometry > geometry ( new osg::Geometry );

  // Set the vertices.
  geometry->setVertexArray  ( box.get() );

  // Add the primitive set.
  geometry->addPrimitiveSet ( new osg::DrawArrays( osg::PrimitiveSet::QUADS, 0, box->size() ) );
  
  osg::ref_ptr < osg::Vec4Array > colors ( new osg::Vec4Array );

  const float alpha ( 0.25f );

  colors->push_back ( osg::Vec4 ( 0.20f, 0.0f,  0.0f, alpha ) );
  colors->push_back ( osg::Vec4 ( 0.20f, 0.0f,  0.0f, alpha ) );
  colors->push_back ( osg::Vec4 ( 0.05f, 0.05f, 0.05f, alpha ) );
  colors->push_back ( osg::Vec4 ( 0.05f, 0.05f, 0.05f, alpha ) );
  colors->push_back ( osg::Vec4 ( 0.0f,  0.0f,  0.20f, alpha ) );
  colors->push_back ( osg::Vec4 ( 0.0f,  0.0f,  0.20f, alpha ) );
  
  // Set colors
  geometry->setColorArray ( colors.get() );
  geometry->setColorBinding ( osg::Geometry::BIND_PER_PRIMITIVE );

  // Set the normals
  osg::ref_ptr< osg::Vec3Array > normals ( new osg::Vec3Array );

  normals->push_back( osg::Vec3 (  0.0f, -1.0f,  0.0f ) );
  normals->push_back( osg::Vec3 (  0.0f,  1.0f,  0.0f ) );
  normals->push_back( osg::Vec3 ( -1.0f,  0.0f,  0.0f ) );
  normals->push_back( osg::Vec3 (  1.0f,  0.0f,  0.0f ) );
  normals->push_back( osg::Vec3 (  0.0f,  0.0f,  1.0f ) );
  normals->push_back( osg::Vec3 (  0.0f,  0.0f, -1.0f ) );

  geometry->setNormalArray ( normals.get() );
  geometry->setNormalBinding ( osg::Geometry::BIND_PER_PRIMITIVE );

  geode->addDrawable( geometry.get() );

  return geode.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get/Set the Maximum value for X
//
///////////////////////////////////////////////////////////////////////////////

void  GlassBoundingBox::xMax ( const float x ) {  _xMax = x; }
float GlassBoundingBox::xMax ()  { return _xMax; }
void  GlassBoundingBox::xMin ( float x ) {  _xMin = x; }
float GlassBoundingBox::xMin ()  {  return _xMin; }

///////////////////////////////////////////////////////////////////////////////
//
//  Get/Set the Maximum value for Y
//
///////////////////////////////////////////////////////////////////////////////

void  GlassBoundingBox::yMax ( float y ) {  _yMax = y; }
float GlassBoundingBox::yMax ()  { return _yMax; }
void  GlassBoundingBox::yMin ( float y ) {  _yMin = y; }
float GlassBoundingBox::yMin ()  {  return _yMin; }

///////////////////////////////////////////////////////////////////////////////
//
//  Get/Set the Maximum value for Z
//
///////////////////////////////////////////////////////////////////////////////

void  GlassBoundingBox::zMax ( float z ) {  _zMax = z; }
float GlassBoundingBox::zMax ()  { return _zMax; }
void  GlassBoundingBox::zMin ( float z ) {  _zMin = z; }
float GlassBoundingBox::zMin ()  {  return _zMin; }


///////////////////////////////////////////////////////////////////////////////
//
// Creates the text numbers at the corners of the bounding box
//
///////////////////////////////////////////////////////////////////////////////


osg::Node* GlassBoundingBox::_makeNumbers    ( )
{
  osg::BoundingBox bb( _xMin,_yMin,_zMin,_xMax,_yMax,_zMax );

  // Make the vertices
  osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array );
  
  // Fill the vertices
  for( unsigned int i = 0; i < 8; ++i ) 
    vertices->push_back ( bb.corner( i ) );

  osg::ref_ptr< osg::Geode >   geode ( new osg::Geode );

  osg::Vec4 layoutColor(1.0f,0.0f,0.0f,1.0f);
  float layoutCharacterSize = 0.055f * bb.radius();    
  
  char name[75];
  sprintf(name, "%0.0f,%0.0f,%0.0f", _xMin, _yMin, _zMin);
 
#if 0
  geode->addDrawable( this->_makeNumber ( name,  bb.corner( 0 ) * 1.05, layoutColor, layoutCharacterSize  ) );
  geode->addDrawable( this->_makeNumber ( _xMax, bb.corner( 1 ) * 1.05, layoutColor, layoutCharacterSize  ) );
  geode->addDrawable( this->_makeNumber ( _yMax, bb.corner( 2 ) * 1.05, layoutColor, layoutCharacterSize  ) );
  geode->addDrawable( this->_makeNumber ( _zMax, bb.corner( 4 ) * 1.05, layoutColor, layoutCharacterSize  ) );
  geode->addDrawable( this->_makeNumber ( "Max", bb.corner( 7 ) * 1.05, layoutColor, layoutCharacterSize  ) );
#else
  geode->addDrawable( this->_makeNumber ( "0", bb.corner( 0 ), layoutColor, layoutCharacterSize  ) );
  geode->addDrawable( this->_makeNumber ( "1", bb.corner( 1 ), layoutColor, layoutCharacterSize  ) );
  geode->addDrawable( this->_makeNumber ( "2", bb.corner( 2 ), layoutColor, layoutCharacterSize  ) );
  geode->addDrawable( this->_makeNumber ( "3", bb.corner( 3 ), layoutColor, layoutCharacterSize  ) );
  geode->addDrawable( this->_makeNumber ( "4", bb.corner( 4 ), layoutColor, layoutCharacterSize  ) );
  geode->addDrawable( this->_makeNumber ( "5", bb.corner( 5 ), layoutColor, layoutCharacterSize  ) );
  geode->addDrawable( this->_makeNumber ( "6", bb.corner( 6 ), layoutColor, layoutCharacterSize  ) );
  geode->addDrawable( this->_makeNumber ( "7", bb.corner( 7 ), layoutColor, layoutCharacterSize  ) );
#endif

  return geode.release();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Creates the osg::Drawable that is the text to place at the bounding box corner
//
///////////////////////////////////////////////////////////////////////////////

osg::Drawable* GlassBoundingBox::_makeNumber     ( float number, const osg::Vec3& pos, const osg::Vec4& color, float size )
{
  char name[50];
  sprintf(name, "%0.3f", number);
  osg::ref_ptr< osgText::Text > text ( new osgText::Text );

  osg::ref_ptr<osgText::Font> font ( OsgTools::Font::defaultFont() );
  text->setFont( font.get() );
  text->setColor( color );
  text->setCharacterSize( size );
  text->setPosition ( pos );
  text->setText( name );
  text->setAutoRotateToScreen(true);
  text->setFontResolution(40,40);
  text->setUseDisplayList(false);
  text->update();

  return text.release();
}

///////////////////////////////////////////////////////////////////////////////
//
// Creates the osg::Drawable that is the text. This version takes a string
//
///////////////////////////////////////////////////////////////////////////////

osg::Drawable* GlassBoundingBox::_makeNumber     ( const std::string& name, const osg::Vec3& pos, const osg::Vec4& color, float size )
{
  osg::ref_ptr< osgText::Text > text ( new osgText::Text );

  osg::ref_ptr<osgText::Font> font ( OsgTools::Font::defaultFont() );
  text->setFont( font.get() );
  text->setColor( color );
  text->setCharacterSize( size );
  text->setPosition ( pos );
  text->setText( name );
  text->setAutoRotateToScreen(true);
  text->setFontResolution(40,40);
  text->setUseDisplayList(false);
  text->update();

  return text.release();
}

