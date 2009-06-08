
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Widgets/Item.h"
#include "OsgTools/State/StateSet.h"

#include "Usul/Bits/Bits.h"

#include "osg/Geode"
#include "osg/Geometry"
#include "osg/PolygonMode"
#include "osg/PolygonOffset"

#include <limits>

using namespace OsgTools::Widgets;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Item::Item() : BaseClass(),
  _size ( 0, 0 ),
  _maximiumSize ( std::numeric_limits<SizeType>::max(), std::numeric_limits<SizeType>::max() ),
  _minimiumSize ( 0, 0 ),
  _position ( 0, 0 ),
  _backgroundColor ( 0.5f, 0.5f, 0.5f, 0.3f ),
  _borderColor( 0.5f, 0.5f, 0.8f, 1.0f ),
  _styleFlags ( STYLE_BACKGROUND | STYLE_BORDER )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Item::~Item()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the suggested size.
//
///////////////////////////////////////////////////////////////////////////////

void Item::size ( SizeType width, SizeType height )
{
  _size.set ( width, height );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the suggested size.
//
///////////////////////////////////////////////////////////////////////////////

void Item::size ( const Size& s )
{
  _size = s;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the suggested size.
//
///////////////////////////////////////////////////////////////////////////////

Item::Size Item::size() const
{
  return _size;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the position of the item.
//
///////////////////////////////////////////////////////////////////////////////

void Item::position( SizeType x, SizeType y )
{
  _position.set ( x, y );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get x position.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Item::x() const
{
  return _position[0];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get y position.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Item::y() const
{
  return _position[1];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the background color.
//
///////////////////////////////////////////////////////////////////////////////

void Item::backgroundColor ( const Color& c )
{
  _backgroundColor = c;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the background color.
//
///////////////////////////////////////////////////////////////////////////////

Item::Color Item::backgroundColor() const
{
  return _backgroundColor;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the border color.
//
///////////////////////////////////////////////////////////////////////////////

void Item::borderColor ( const Color& c )
{
  _borderColor = c;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the border color.
//
///////////////////////////////////////////////////////////////////////////////

Item::Color Item::borderColor() const
{
  return _borderColor;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a quad.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  inline osg::Geometry* buildQuad ( unsigned int width, unsigned int height )
  {
    osg::ref_ptr < osg::Geometry > geometry ( new osg::Geometry );
    
    osg::ref_ptr < osg::Vec3Array > vertices ( new osg::Vec3Array );
    vertices->push_back( osg::Vec3 ( 0.0, 0.0, 0.0 ) );
    vertices->push_back( osg::Vec3 ( 0.0, height, 0.0 ) );
    vertices->push_back( osg::Vec3 ( width, height, 0.0 ) );
    vertices->push_back( osg::Vec3 ( width, 0.0, 0.0 ) );
    
    geometry->setVertexArray ( vertices.get() );
    
    osg::ref_ptr < osg::Vec3Array > normals ( new osg::Vec3Array );
    normals->push_back ( osg::Vec3 ( 0.0, 0.0, 1.0 ) );
    
    geometry->setNormalArray ( normals.get() );
    geometry->setNormalBinding ( osg::Geometry::BIND_OVERALL );
    
    geometry->addPrimitiveSet ( new osg::DrawArrays ( GL_QUADS, 0, vertices->size() ) );
    return geometry.release();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the background.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Item::_buildBackground ( unsigned int width, unsigned int height )
{
  // For readabilty.
  const unsigned int on  ( osg::StateAttribute::ON  | osg::StateAttribute::PROTECTED );
  const unsigned int off ( osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );
  
  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );

  // Get the style flags.
  Flags styleFlags ( this->styleFlags() );
  
  // Make the background.
  if ( Usul::Bits::has ( styleFlags, STYLE_BACKGROUND ) )
  {
    osg::ref_ptr < osg::Geometry > geometry ( Detail::buildQuad ( width, height ) );
    geode->addDrawable( geometry.get() );
    
    // Get the state set.
    osg::ref_ptr< osg::StateSet > ss ( geometry->getOrCreateStateSet() );
    
    // Set the color.
    osg::ref_ptr<osg::Vec4Array> colors ( new osg::Vec4Array );
    colors->push_back ( this->backgroundColor() );
    geometry->setColorArray ( colors.get() );
    geometry->setColorBinding ( osg::Geometry::BIND_OVERALL );
    
    // Set the needed modes.
    ss->setMode( GL_DEPTH_TEST, off );
    ss->setMode( GL_BLEND, on );
    ss->setMode( GL_POLYGON_OFFSET_FILL, on );
    
    osg::ref_ptr< osg::PolygonOffset > offset ( new osg::PolygonOffset ( 1.0, 4.0 ) );
    ss->setAttributeAndModes ( offset.get(), on );
  }
  
  // Make the outline.
  if ( Usul::Bits::has ( styleFlags, STYLE_BORDER ) )
  {
    osg::ref_ptr < osg::Geometry > geometry ( Detail::buildQuad ( width, height ) );
    geode->addDrawable( geometry.get() );
    
    // Get the state set.
    osg::ref_ptr< osg::StateSet > ss ( geometry->getOrCreateStateSet() );
    
    // Set the color.
    osg::ref_ptr<osg::Vec4Array> colors ( new osg::Vec4Array );
    colors->push_back ( this->borderColor() );
    geometry->setColorArray ( colors.get() );
    geometry->setColorBinding ( osg::Geometry::BIND_OVERALL );
    
    // Turn on wire frame.
    osg::ref_ptr< osg::PolygonMode > mode ( new osg::PolygonMode( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE ) );
    ss->setAttribute( mode.get(), on );
  }
  
  // Turn off lighting.
  OsgTools::State::StateSet::setLighting ( geode.get(), false );
  
  // Return the background.
  return geode.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Estimate the needed size for the item.  By default it returns the suggested size.
//
///////////////////////////////////////////////////////////////////////////////

Item::Size Item::estimateSizeForWidth ( unsigned int w ) const
{
  return this->size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the maximium size.
//
///////////////////////////////////////////////////////////////////////////////

void Item::maximiumSize ( SizeType width, SizeType height )
{
  _maximiumSize.set ( width, height );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the maximium size.
//
///////////////////////////////////////////////////////////////////////////////

void Item::maximiumSize ( const Size& s )
{
  _maximiumSize = s;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the maximium size.
//
///////////////////////////////////////////////////////////////////////////////

Item::Size Item::maximiumSize() const
{
  return _maximiumSize;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the minimium size.
//
///////////////////////////////////////////////////////////////////////////////

void Item::minimiumSize ( SizeType width, SizeType height )
{
  _minimiumSize.set ( width, height );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the minimium size.
//
///////////////////////////////////////////////////////////////////////////////

void Item::minimiumSize ( const Size& s )
{
  _minimiumSize = s;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the minimium size.
//
///////////////////////////////////////////////////////////////////////////////

Item::Size Item::minimiumSize() const
{
  return _minimiumSize;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Item was clicked.
//
///////////////////////////////////////////////////////////////////////////////

void Item::onClick ( double x, double y )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the style flags.
//
///////////////////////////////////////////////////////////////////////////////

void Item::styleFlags ( Flags flags )
{
  _styleFlags = flags;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the style flags.
//
///////////////////////////////////////////////////////////////////////////////

Item::Flags Item::styleFlags() const
{
  return _styleFlags;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the render order for the given depth.
//
///////////////////////////////////////////////////////////////////////////////

void Item::_setState ( osg::StateSet* ss, unsigned int depth )
{
  if ( 0x0 != ss )
  {
    // Does these need to be set?
    //ss->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED | osg::StateAttribute::OVERRIDE );
    //ss->setMode( GL_BLEND, osg::StateAttribute::ON  | osg::StateAttribute::PROTECTED | osg::StateAttribute::OVERRIDE );

    // Set the render bin based off of the depth.
    ss->setRenderBinDetails ( 100 + depth, "RenderBin" );
  }
}
