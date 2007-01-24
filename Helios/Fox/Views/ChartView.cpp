
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#define NOMINMAX

#include "Helios/Fox/Views/ChartView.h"
#include "Helios/Fox/Views/FoxContext.h"

#include "Usul/Errors/Checker.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Bits/Bits.h"

#include "OsgTools/State/StateSet.h"
#include "OsgTools/Render/ClampProjection.h"
#include "OsgTools/Render/Defaults.h"

#include "osgUtil/UpdateVisitor"

#include "osg/GL"
#include "osg/Geode"
#include "osg/Geometry"

#include "osgText/Text"

#include <limits>

using namespace Helios::Views;


///////////////////////////////////////////////////////////////////////////////
//
//  Message maps.
//
///////////////////////////////////////////////////////////////////////////////

FXDEFMAP ( ChartView ) ChartViewMap[] =
{
  // Message type,                     id,                             handler.
  FXMAPFUNC ( FX::SEL_FOCUSIN,         0,                              ChartView::onFocusIn           ),
};

// This class, base class, array of message handlers, size of the array.
FOX_TOOLS_IMPLEMENT ( ChartView, BaseClass, ChartViewMap, ARRAYNUMBER ( ChartViewMap ) );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ChartView::ChartView( Usul::Interfaces::IDocument* document, FX::FXComposite *parent, FX::FXGLVisual *visual ) :
  BaseClass ( document, parent, visual ),
  _plots(),
  _colors(),
  _paddingPercent ( 0.05f ),
  _graphPercent ( 0.9f )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ChartView::~ChartView()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Render.
//
///////////////////////////////////////////////////////////////////////////////

void ChartView::render()
{
  if( this->viewer() )
    this->viewer()->render();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create
//
///////////////////////////////////////////////////////////////////////////////

void ChartView::create()
{
  // Call the base class's function first.
  BaseClass::create();

  if ( this->viewer() )
  {
    this->viewer()->backgroundColor ( osg::Vec4 ( 1.0f, 1.0f, 1.0f, 1.0f ) );

    this->viewer()->scene ( new osg::Group );

    this->viewer()->axes ( false );
  }

  // Pop this window to the front.
  this->setFocus();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for given interface id.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* ChartView::queryInterface( unsigned long iid )
{
  switch( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IBackground::IID:
    return static_cast < Usul::Interfaces::IBackground* > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  This window has the focus now.
//
///////////////////////////////////////////////////////////////////////////////

long ChartView::onFocusIn ( FX::FXObject *, FX::FXSelector, void * )
{
  this->render();

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Ref.
//
///////////////////////////////////////////////////////////////////////////////

void ChartView::ref()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unref.
//
///////////////////////////////////////////////////////////////////////////////

void ChartView::unref( bool )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find the maximium y value.
//
///////////////////////////////////////////////////////////////////////////////

float ChartView::_maxY ( const Plot& plot )
{
  float max ( std::numeric_limits< float >::min() );

  for( unsigned int i = 0; i < plot.size(); ++i )
  {
    max = Usul::Math::maximum <float> ( max, (float) plot.at(i)[1], std::greater<float>() );
  }

  return max;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find the maximium x value.
//
///////////////////////////////////////////////////////////////////////////////

float ChartView::_maxX ( const Plot& plot )
{
  float max ( std::numeric_limits< float >::min() );

  for( unsigned int i = 0; i < plot.size(); ++i )
  {
    max = Usul::Math::maximum <float> ( max, (float) plot.at(i)[0], std::greater<float>() );
  }

  return max;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add plot with color.
//
///////////////////////////////////////////////////////////////////////////////

void ChartView::addPlot( const Plot& plot, const Color& color )
{
  _plots.push_back( plot );

  ColorArray colors ( plot.size(), color );
  _colors.push_back( colors );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add plot with color.
//
///////////////////////////////////////////////////////////////////////////////

void ChartView::addPlot( const Plot& plot, const ColorArray& colors )
{
  _plots.push_back( plot );
  _colors.push_back( colors );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Resize the viewer.
//
///////////////////////////////////////////////////////////////////////////////

void ChartView::_resize ( unsigned int width, unsigned int height )
{
  this->viewer()->resize ( width, height );
  this->_rebuildScene ( *this->viewer()->sceneManager()->projectionGroupGet ( "2D_PLOT" ), width, height );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Rebuild the scene.
//
///////////////////////////////////////////////////////////////////////////////

void ChartView::_rebuildScene ( osg::Group& group, unsigned int width, unsigned int height )
{
  // Clear the root.
  group.removeChild ( 0, group.getNumChildren() );

  // Make a geode.
  osg::ref_ptr < osg::Geode > geode ( new osg::Geode );
  group.addChild ( geode.get() );

  // Turn of lighting
  OsgTools::State::StateSet::setLighting ( geode.get(), false );

  // Set the line width
  OsgTools::State::StateSet::setLineWidth ( geode.get(), 1.0f );

  // Calculate the x and y offset
  const float xOffset ( width * _paddingPercent );
  const float yOffset ( height * _paddingPercent );

  float widthMax  ( std::numeric_limits< float >::min() );
  float heightMax ( std::numeric_limits< float >::min() );

  // find the max width and height.
  for( unsigned int i = 0; i < _plots.size(); ++i )
  {
    heightMax = Usul::Math::maximum <float> ( heightMax, this->_maxY( _plots.at(i) ), std::greater<float>() );
    widthMax =  Usul::Math::maximum <float> ( widthMax,  this->_maxX( _plots.at(i) ), std::greater<float>() );
  }

  if ( heightMax < 0.0f )
    heightMax *= -1.0;

  const float heightNorm ( ( (float) height * _graphPercent ) / heightMax );  
  const float widthNorm  ( ( (float) width  * _graphPercent ) / widthMax );
  
  // Add each plot to geode.
  for( unsigned int i = 0; i < _plots.size(); ++i )
  {
    osg::ref_ptr < osg::Geometry > geometry ( new osg::Geometry );
    osg::ref_ptr < osg::Vec3Array > vertices ( new osg::Vec3Array );
    osg::ref_ptr < osg::Vec4Array > colors ( new osg::Vec4Array );

    colors->insert ( colors->end(), _colors.at(i).begin(), _colors.at(i).end() );
    
    this->_buildVertices( _plots.at(i), *vertices, xOffset, yOffset, widthNorm, heightNorm );

    geometry->setVertexArray ( vertices.get() );
    geometry->setColorArray ( colors.get() );
    geometry->setColorBinding ( osg::Geometry::BIND_PER_VERTEX );

    geometry->addPrimitiveSet ( new osg::DrawArrays ( GL_LINE_STRIP, 0, vertices->size() ) );

    geode->addDrawable ( geometry.get() );
  }

  // axis
  {
    osg::ref_ptr < osg::Geometry > geometry ( new osg::Geometry );
    osg::ref_ptr < osg::Vec3Array > vertices ( new osg::Vec3Array );
    osg::ref_ptr < osg::Vec4Array > colors ( new osg::Vec4Array );

    colors->push_back ( osg::Vec4 ( 0.0f, 0.0f, 0.0f, 1.0 ) );

    vertices->push_back ( osg::Vec3 ( xOffset, yOffset + ( this->getHeight() * _graphPercent ), -2.0f ) );
    vertices->push_back ( osg::Vec3 ( xOffset, yOffset, -2.0f ) );
    vertices->push_back ( osg::Vec3 ( xOffset + ( this->getWidth() * _graphPercent ), yOffset, -2.0f ) );

    geometry->setVertexArray ( vertices.get() );
    geometry->setColorArray ( colors.get() );
    geometry->setColorBinding ( osg::Geometry::BIND_OVERALL );

    geometry->addPrimitiveSet ( new osg::DrawArrays ( GL_LINE_STRIP, 0, vertices->size() ) );

    geode->addDrawable ( geometry.get() );
  }

  // Labels
  {

  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the osg::Vec3array to draw the plot.
//
///////////////////////////////////////////////////////////////////////////////

void ChartView::_buildVertices ( const Plot& plot, osg::Vec3Array& vertices, float xOffset, float yOffset, float widthNorm, float heightNorm )
{
  for( unsigned int i = 0; i < plot.size(); ++i )
  {
    float x ( (float)plot.at(i)[0] * widthNorm + xOffset );
    float y ( (float)plot.at(i)[1] * heightNorm + yOffset );

    vertices.push_back ( osg::Vec3 ( x, y, -3.0f ) );
  }
}


void ChartView::editBackground()
{
  this->viewer()->backgroundColor ( osg::Vec4 ( 1.0f, 1.0f, 1.0f, 1.0f ) );
  this->viewer()->render();
}

void ChartView::defaultBackground()
{
  this->viewer()->backgroundColor ( osg::Vec4 ( 1.0f, 1.0f, 1.0f, 1.0f ) );
  this->viewer()->render();
}
