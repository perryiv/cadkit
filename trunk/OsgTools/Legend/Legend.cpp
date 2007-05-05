
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Legend/Legend.h"

#include "osg/Material"
#include "osg/Geode"
#include "osg/Group"
#include "osg/MatrixTransform"
#include "osg/Geometry"
#include "osg/PolygonMode"
#include "osg/PolygonOffset"

using namespace OsgTools::Legend;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Legend::Legend() : 
BaseClass(),
_legendObjects(),
_x ( 0 ),
_y ( 0 ),
_width ( 0 ),
_height ( 0 ),
_heightPerItem ( 0 ),
_growDirection ( UP )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Legend::~Legend()
{
  _legendObjects.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear.
//
///////////////////////////////////////////////////////////////////////////////

void Legend::clear()
{
  _legendObjects.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a legend object.
//
///////////////////////////////////////////////////////////////////////////////

void Legend::addLegendObject( OsgTools::Legend::LegendObject *legendObject )
{
  _legendObjects.push_back( legendObject );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Legend::buildScene()
{
  osg::ref_ptr< osg::MatrixTransform > root ( new osg::MatrixTransform );
  root->setName( "Legend" );
  root->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
  root->setMatrix ( osg::Matrix::translate ( _x, _y, 0.0 ) );

  osg::ref_ptr< osg::Group > group ( new osg::Group );

  root->addChild( group.get() );

  if( _legendObjects.size() > 0 )
  {
    unsigned int heightPerObject ( _heightPerItem );

    unsigned int height ( heightPerObject * _legendObjects.size() );

    // Change the height per object to fit in the size paramaters given.
    if( height > _height )
    {
      heightPerObject = this->_height / _legendObjects.size();
      height = _height;
    }

    // Build the background
    group->addChild( this->_buildBackground( _width, height ) );

    unsigned int padding ( 5 );

    for( LegendObjects::iterator iter = _legendObjects.begin(); iter != _legendObjects.end(); ++iter )
    {
      (*iter)->size( _width - ( padding * 2 ), heightPerObject );
      unsigned int num ( iter - _legendObjects.begin() );
      osg::ref_ptr< osg::MatrixTransform > mt ( new osg::MatrixTransform );

      /// Needs to be signed.
      int yTranslate ( heightPerObject * num + padding );

      // Invert the y translation if we are growing down.
      if( this->growDirection() == DOWN )
        yTranslate *= -1;

      // WHY?
#ifdef _MSC_VER
      osg::Matrix m ( osg::Matrix::translate ( padding, yTranslate, -1.0 ) );
#else
      osg::Matrix m ( osg::Matrix::translate ( padding, yTranslate, 1.0 ) );
#endif
      mt->setMatrix( m );
      mt->addChild( (*iter)->buildScene() );

      // Turn off depth testing.
      osg::ref_ptr < osg::StateSet > ss ( mt->getOrCreateStateSet() );
      ss->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );

      group->addChild( mt.get() );
    }
  }

  return root.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the background.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Legend::_buildBackground( unsigned int width, unsigned int height )
{
  osg::ref_ptr< osg::Group > group ( new osg::Group );

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

  {
    osg::ref_ptr< osg::Geode > geode ( new osg::Geode );
    geode->addDrawable( geometry.get() );

    osg::ref_ptr< osg::Material > material ( new osg::Material );
    material->setDiffuse( osg::Material::FRONT_AND_BACK, osg::Vec4( 0.5, 0.5, 0.5, 1.0 ) );
    material->setAlpha( osg::Material::FRONT_AND_BACK, 0.3f );

    osg::ref_ptr< osg::StateSet > ss ( geode->getOrCreateStateSet() );
    ss->setAttribute( material.get(), osg::StateAttribute::ON );
    ss->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );
    ss->setMode( GL_BLEND, osg::StateAttribute::ON );
    ss->setMode( GL_POLYGON_OFFSET_FILL, osg::StateAttribute::ON );

    osg::ref_ptr< osg::PolygonOffset > offset ( new osg::PolygonOffset ( -1.0, -1.0 ) );

    group->addChild( geode.get() );
  }

  {
    osg::ref_ptr< osg::Geode > geode ( new osg::Geode );
    geode->addDrawable( geometry.get() );

    osg::ref_ptr< osg::Material > material ( new osg::Material );
    material->setDiffuse( osg::Material::FRONT_AND_BACK, osg::Vec4( 0.5, 0.5, 0.8, 1.0 ) );
    material->setAlpha( osg::Material::FRONT_AND_BACK, 0.8f );

    osg::ref_ptr< osg::StateSet > ss ( geode->getOrCreateStateSet() );
    ss->setAttribute( material.get(), osg::StateAttribute::ON );

    osg::ref_ptr< osg::PolygonMode > mode ( new osg::PolygonMode( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE ) );
    ss->setAttribute( mode.get(), osg::StateAttribute::ON );

    group->addChild( geode.get() );
  }

  return group.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the size of the legend.
//
///////////////////////////////////////////////////////////////////////////////

void Legend::maximiumSize( unsigned int width, unsigned int height )
{
  _width = width;
  _height = height;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the grow direction mode.
//
///////////////////////////////////////////////////////////////////////////////

void Legend::growDirection( OsgTools::Legend::Legend::GrowDirectionMode mode )
{
  _growDirection = mode;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the grow direction mode.
//
///////////////////////////////////////////////////////////////////////////////

OsgTools::Legend::Legend::GrowDirectionMode Legend::growDirection() const
{
  return _growDirection;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the height per item.
//
///////////////////////////////////////////////////////////////////////////////

void Legend::heightPerItem( unsigned int height )
{
  _heightPerItem = height;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the height per item.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Legend::heightPerItem () const
{
  return _heightPerItem;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the position of the legend.
//
///////////////////////////////////////////////////////////////////////////////

void Legend::position( unsigned int x, unsigned int y )
{
  _x = x;
  _y = y;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the height.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Legend::height() const
{
  unsigned int height ( _heightPerItem * _legendObjects.size() );

  return ( ( height > _height ) ? _height : height );
}