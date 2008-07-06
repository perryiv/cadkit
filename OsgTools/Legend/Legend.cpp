
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Legend/Legend.h"
#include "OsgTools/Legend/Text.h"
#include "OsgTools/State/StateSet.h"

#include "osg/Material"
#include "osg/Geode"
#include "osg/Group"
#include "osg/MatrixTransform"
#include "osg/Geometry"
#include "osg/PolygonMode"
#include "osg/PolygonOffset"

#include <algorithm>

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
//  Predicate to sort legend objects.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  struct SortLegend
  {
    SortLegend ( unsigned int column ) : _column ( column )
    {
    }
    
    template < class T >
    bool operator () ( const T& rhs, const T& lhs ) const
    {
      const Text* text0 ( dynamic_cast<const Text*> ( rhs->at( _column ) ) );
      const Text* text1 ( dynamic_cast<const Text*> ( lhs->at( _column ) ) );
      
      if ( 0x0 != text0 && 0x0 != text1 )
      {
        // Greater than because the first item is drawn on the bottom of the legend.
        // Sorting from Z to A will display A to Z on the legend.
        return text0->text() > text1->text();
      }
      
      return false;
    }
    
  private:
    unsigned int _column;
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Legend::buildScene()
{
  //const SizeType width  ( this->size()[0] );
  //const SizeType height ( this->size()[1] );
  
  // Sort the legend by name.
  std::sort( _legendObjects.begin(), _legendObjects.end(), Detail::SortLegend ( 1 ) );

  // Create the root for the legend.
  osg::ref_ptr< osg::MatrixTransform > root ( new osg::MatrixTransform );
  root->setName( "Legend" );
  root->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
  root->setMatrix ( osg::Matrix::translate ( _x, _y, 0.0 ) );

  osg::ref_ptr < osg::StateSet > ss ( root->getOrCreateStateSet () );
  ss->setMode ( GL_CULL_FACE, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );
  //ss->setMode ( GL_LIGHTING,  osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
  OsgTools::State::StateSet::setTwoSidedLighting ( ss.get(), true );

  osg::ref_ptr< osg::Group > group ( new osg::Group );

  root->addChild( group.get() );

  // Only build the legend if we have something...
  if( _legendObjects.size() > 0 )
  {
    // Calculate needed height values.
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
      // Transform for the row.
      osg::ref_ptr< osg::MatrixTransform > mt ( new osg::MatrixTransform );
      
      // Width and height of the row.
      const unsigned int rowWidth ( _width - ( padding * 2 ) );
      const unsigned int rowHeight ( heightPerObject );
      
      const unsigned int num ( iter - _legendObjects.begin() );

      /// Y position (Needs to be signed).
      int yTranslate ( heightPerObject * num + padding );

      // Invert the y translation if we are growing down.
      if( this->growDirection() == DOWN )
        yTranslate *= -1;

      // Position the row.
      mt->setMatrix( osg::Matrix::translate ( padding, yTranslate, -1.0 ) );

      // Build the row.
      (*iter)->size ( rowWidth, rowHeight );
      mt->addChild( (*iter)->buildScene() );

      // Turn off depth testing.
      osg::ref_ptr < osg::StateSet > ss ( mt->getOrCreateStateSet() );
      ss->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );

      // Add to the scene.
      group->addChild( mt.get() );
    }
  }

  return root.release();
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

osg::Node* Legend::_buildBackground( unsigned int width, unsigned int height )
{
  // For readabilty.
  const unsigned int on  ( osg::StateAttribute::ON  | osg::StateAttribute::PROTECTED );
  const unsigned int off ( osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );

  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );

  // Make the background.
  {
    osg::ref_ptr < osg::Geometry > geometry ( Detail::buildQuad ( width, height ) );
    geode->addDrawable( geometry.get() );

    // Get the state set.
    osg::ref_ptr< osg::StateSet > ss ( geometry->getOrCreateStateSet() );

    // Set the color.
    osg::ref_ptr<osg::Vec4Array> colors ( new osg::Vec4Array );
    colors->push_back ( osg::Vec4( 0.5, 0.5, 0.5, 0.3 ) );
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
  {
    osg::ref_ptr < osg::Geometry > geometry ( Detail::buildQuad ( width, height ) );
    geode->addDrawable( geometry.get() );

    // Get the state set.
    osg::ref_ptr< osg::StateSet > ss ( geometry->getOrCreateStateSet() );

    // Set the color.
    osg::ref_ptr<osg::Vec4Array> colors ( new osg::Vec4Array );
    colors->push_back ( osg::Vec4( 0.5, 0.5, 0.8, 1.0 ) );
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
