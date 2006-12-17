
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Makes a gradient background.
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Builders/GradientBackground.h"

#include "Usul/Bits/Bits.h"

#include "osg/Viewport"
#include "osg/Geode"

using namespace OsgTools::Builders;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

GradientBackground::GradientBackground() :
  _root     ( new osg::MatrixTransform ),
  _geom     ( new osg::Geometry ),
  _colors   ( new osg::Vec4Array ),
  _vertices ( new osg::Vec3Array )
{
  this->init();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

GradientBackground::~GradientBackground()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the vertices.
//
///////////////////////////////////////////////////////////////////////////////

void GradientBackground::update ( unsigned int w, unsigned int h )
{
  if ( true == _vertices.valid() && true == _geom.valid() )
  {
    // Set vertices.
    _vertices->at(0) = osg::Vec3 ( 0.0f,  0.0f, 0.0f );
    _vertices->at(1) = osg::Vec3 (    w,  0.0f, 0.0f );
    _vertices->at(2) = osg::Vec3 (    w,     h, 0.0f );
    _vertices->at(3) = osg::Vec3 ( 0.0f,     h, 0.0f );

    _geom->dirtyDisplayList();
    _geom->dirtyBound();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear internal scene.
//
///////////////////////////////////////////////////////////////////////////////

void GradientBackground::clear()
{
  _root     = new osg::MatrixTransform;
  _geom     = new osg::Geometry;
  _colors   = new osg::Vec4Array;
  _vertices = new osg::Vec3Array;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the internal scene.
//
///////////////////////////////////////////////////////////////////////////////

void GradientBackground::init()
{
  // Declare new objects.
  _root = new osg::MatrixTransform;
  osg::ref_ptr<osg::Geode> geode ( new osg::Geode );
  _geom = new osg::Geometry;
  _vertices = new osg::Vec3Array ( 4 );
  _colors = new osg::Vec4Array ( 4 );

  // Set default vertices.
  this->update ( 100, 100 );

  // Set default colors.
  osg::Vec4 color ( 0.0f, 0.0f, 0.0f, 1.0f );
  _colors->at(0) = color;
  _colors->at(1) = color;
  _colors->at(2) = color;
  _colors->at(3) = color;

  // Set transform properties.
  _root->setReferenceFrame ( osg::Transform::ABSOLUTE_RF );
  _root->setMatrix ( osg::Matrix::identity() );

  // Set geometry properties.
  _geom->setVertexArray ( _vertices.get() );
  _geom->setColorArray ( _colors.get() );
  _geom->setColorBinding ( osg::Geometry::BIND_PER_VERTEX );
  _geom->addPrimitiveSet ( new osg::DrawArrays ( GL_QUADS, 0, _vertices->size() ) );
  _geom->setUseDisplayList ( false );

  // Set geode properties.
  osg::ref_ptr<osg::StateSet> state ( geode->getOrCreateStateSet() );
  state->setMode ( GL_LIGHTING,   osg::StateAttribute::OFF );
  state->setMode ( GL_DEPTH_TEST, osg::StateAttribute::OFF );
  state->setRenderBinDetails ( -1, "RenderBin" );

  // Hook up scene.
  _root->addChild ( geode.get() );
  geode->addDrawable ( _geom.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the root of the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *GradientBackground::root()
{
  return _root.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the index.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  using namespace OsgTools::Builders;
  template < unsigned int T > struct Pair;
  template <> struct Pair < GradientBackground::Corners::BOTTOM_LEFT >  { enum { Value = 0 }; };
  template <> struct Pair < GradientBackground::Corners::BOTTOM_RIGHT > { enum { Value = 1 }; };
  template <> struct Pair < GradientBackground::Corners::TOP_RIGHT >    { enum { Value = 2 }; };
  template <> struct Pair < GradientBackground::Corners::TOP_LEFT >     { enum { Value = 3 }; };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the color.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  using namespace OsgTools::Builders;
  template < unsigned int T > struct Color
  {
    static void set ( const osg::Vec4 &color, unsigned int corners, osg::Vec4Array &colors )
    {
      if ( true == Usul::Bits::has ( corners, T ) )
      {
        colors.at(Detail::Pair<T>::Value) = color;
      }
    }
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the colors.
//
///////////////////////////////////////////////////////////////////////////////

void GradientBackground::color ( const osg::Vec4 &color, unsigned int corners )
{
  Detail::Color<Corners::BOTTOM_LEFT >::set ( color, corners, *_colors );
  Detail::Color<Corners::BOTTOM_RIGHT>::set ( color, corners, *_colors );
  Detail::Color<Corners::TOP_RIGHT   >::set ( color, corners, *_colors );
  Detail::Color<Corners::TOP_LEFT    >::set ( color, corners, *_colors );
  _geom->dirtyDisplayList();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the color. Asking for more than one will average them.
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec4 GradientBackground::color ( unsigned int corners ) const
{
  switch ( corners )
  {
    case Corners::BOTTOM_LEFT:
      return _colors->at(Detail::Pair<Corners::BOTTOM_LEFT>::Value);
    case Corners::BOTTOM_RIGHT:
      return _colors->at(Detail::Pair<Corners::BOTTOM_RIGHT>::Value);
    case Corners::TOP_LEFT:
      return _colors->at(Detail::Pair<Corners::TOP_LEFT>::Value);
    case Corners::TOP_RIGHT:
      return _colors->at(Detail::Pair<Corners::TOP_RIGHT>::Value);
  }

  osg::Vec4 color ( 0, 0, 0, 0 );
  unsigned int count ( 0 );

  if ( true == Usul::Bits::has ( corners, Corners::BOTTOM_LEFT ) )
  {
    color += this->color ( Corners::BOTTOM_LEFT );
    count += 1;
  }
  if ( true == Usul::Bits::has ( corners, Corners::BOTTOM_RIGHT ) )
  {
    color += this->color ( Corners::BOTTOM_RIGHT );
    count += 1;
  }
  if ( true == Usul::Bits::has ( corners, Corners::TOP_RIGHT ) )
  {
    color += this->color ( Corners::TOP_RIGHT );
    count += 1;
  }
  if ( true == Usul::Bits::has ( corners, Corners::TOP_LEFT ) )
  {
    color += this->color ( Corners::TOP_LEFT );
    count += 1;
  }

  if ( count > 0 )
    color /= count;
  return color;
}
