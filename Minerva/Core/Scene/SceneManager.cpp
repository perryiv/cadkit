
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Scene/SceneManager.h"

#include "Usul/Interfaces/IVectorLayer.h"
#include "Usul/Interfaces/ITemporalData.h"
#include "Usul/Interfaces/IAddRowLegend.h"
#include "Usul/Trace/Trace.h"

#include "OsgTools/Animate/AnimationCallback.h"
#include "OsgTools/State/StateSet.h"
#include "OsgTools/Font.h"

#include "osg/Geode"
#include "osg/Material"
#include "osg/Sequence"
#include "osg/Version"
#include "osg/Light"

#include "osgText/Text"
#include "osgDB/WriteFile"

#include <algorithm>

using namespace Minerva::Core::Scene;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( SceneManager , SceneManager::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SceneManager::SceneManager() :
BaseClass(),
_mutex(),
_root ( new osg::Group ),
_static_root( new osg::Group ),
_projectionNode ( new osg::Projection ),
_animateNode ( new OsgTools::Animate::DateGroup ),
_layers(),
_dirty ( false ),
_width( 0 ),
_height( 0 ),
_legend( new OsgTools::Legend::Legend ),
_showLegend( true ),
_legendWidth ( 0.20f ),
_legendHeightPerItem ( 30 ),
_legendPadding ( 20.0f, 20.0f ),
_legendPosition ( LEGEND_BOTTOM_RIGHT )
{
  // Make sure it draws last and without depth testing.
  osg::ref_ptr< osg::StateSet > ss ( _projectionNode->getOrCreateStateSet() );
  ss->setRenderBinDetails( 1000, "RenderBin" );
  ss->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );

  osg::ref_ptr< osg::Light > light ( new osg::Light );
  light->setLightNum ( 1 );
  light->setDiffuse( osg::Vec4 ( 0.8, 0.8, 0.8, 1.0 ) );
  light->setDirection( osg::Vec3 ( 0.0, 0.0, -1.0f ) );

  {
    //osg::ref_ptr < osg::StateSet > ss ( _root->getOrCreateStateSet() );
    ss->setAttributeAndModes ( light.get(), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SceneManager::~SceneManager()
{
  this->clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

void SceneManager::buildScene( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;

  Guard guard ( _mutex );

  if ( this->dirty() )
  {
    _legend->clear();

    // Clear what we have
    _root->removeChild( 0, _root->getNumChildren() );
    _static_root->removeChild( 0, _static_root->getNumChildren() );
    _projectionNode->removeChild ( 0, _projectionNode->getNumChildren() );
    _animateNode->removeChild ( 0, _animateNode->getNumChildren() );

    // Set up animation node.
    this->_setUpAnimationNode();

    // Loop throught the layers
    for ( Layers::iterator iter = _layers.begin(); iter != _layers.end(); ++iter )
    {
      if( iter->second->showLayer() )
      {
        Usul::Interfaces::IVectorLayer::QueryPtr vector ( iter->second.get() );
        if( vector.valid() )
        {
          Usul::Interfaces::ITemporalData::QueryPtr temporal ( vector );

          if( temporal.valid() )
          {
            vector->buildScene ( _animateNode.get() );
          }
          else
          {
            osg::ref_ptr< osg::Group > group ( new osg::Group );
            vector->buildScene( group.get() );
            _static_root->addChild( group.get() );
          }
        }
      }
    }

    _root->addChild( _animateNode.get() );
    _root->addChild( _static_root.get() );
    _root->addChild ( _projectionNode.get() );

    /// Build the legend.
    this->_buildLegend();

    _root->dirtyBound();
    _static_root->dirtyBound();
    _projectionNode->dirtyBound();
  }

  this->dirty( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set up the animation node.
//
///////////////////////////////////////////////////////////////////////////////

void SceneManager::_setUpAnimationNode()
{
  osg::ref_ptr < osgText::Text > text ( new osgText::Text );

  text->setFont( OsgTools::Font::defaultFont() );
  text->setPosition ( osg::Vec3( 5.0, _height - 25.0, 0.0 ) );
  text->setCharacterSizeMode( osgText::Text::SCREEN_COORDS );
  text->setText ( "" );
  text->setColor( osg::Vec4 ( 1.0, 1.0, 1.0, 1.0 ) );

  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );
  geode->addDrawable( text.get() );

  osg::ref_ptr < osg::MatrixTransform > mt ( new osg::MatrixTransform );
  mt->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
  mt->addChild( geode.get() );

  _projectionNode->addChild ( mt.get() );

  _animateNode->setText( text.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the legend.
//
///////////////////////////////////////////////////////////////////////////////

void SceneManager::_buildLegend()
{
  if( this->showLegend() )
  {
    // Set the legend size.
    unsigned int legendWidth  ( static_cast < unsigned int > ( _width * _legendWidth ) );
    unsigned int legendHeight ( static_cast < unsigned int > ( _height - ( _legendPadding.y() * 2 ) ) );

    _legend->maximiumSize( legendWidth, legendHeight );
    _legend->heightPerItem( _legendHeightPerItem );

    for ( Layers::iterator iter = _layers.begin(); iter != _layers.end(); ++iter )
    {
      Usul::Interfaces::IAddRowLegend::QueryPtr addRow ( iter->second.get() );
      if( iter->second->showLayer() && addRow.valid() )
      {
        OsgTools::Legend::LegendObject::RefPtr row ( new OsgTools::Legend::LegendObject );
        addRow->addLegendRow( row.get() );
        _legend->addLegendObject( row.get() );
      }
    }

    // Must be called after rows are added to the legend.
    this->_setLegendPosition( legendWidth );

    // Build the legend.
    _projectionNode->addChild( _legend->buildScene() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the legend position.
//
///////////////////////////////////////////////////////////////////////////////

void SceneManager::_setLegendPosition ( unsigned int legendWidth )
{
  // Translate legend to correct location.
  unsigned int x ( 0 );
  unsigned int y ( 0 );

  unsigned legendHeight ( _legend->height() );

  switch ( _legendPosition )
  {
  case LEGEND_TOP_LEFT:
    {
      x = static_cast < unsigned int > ( _legendPadding.x() );
      y = static_cast < unsigned int > ( _height - legendHeight -_legendPadding.y() );
      _legend->growDirection( OsgTools::Legend::Legend::UP );
    }
    break;
  case LEGEND_TOP_RIGHT:
    {
      x = static_cast < unsigned int > ( _width - ( legendWidth + _legendPadding.x() ) );
      y = static_cast < unsigned int > ( _height - legendHeight - _legendPadding.y() );
      _legend->growDirection( OsgTools::Legend::Legend::UP );
    }
    break;
  case LEGEND_BOTTOM_RIGHT:
    {
      x = static_cast < unsigned int > ( _width - ( legendWidth + _legendPadding.x() ) );
      y = static_cast < unsigned int > ( _legendPadding.y() );
      _legend->growDirection( OsgTools::Legend::Legend::UP );
    }
    break;
  case LEGEND_BOTTOM_LEFT:
    {
      x = static_cast < unsigned int > ( _legendPadding.x() );
      y = static_cast < unsigned int > ( _legendPadding.y() );
      _legend->growDirection( OsgTools::Legend::Legend::UP );
    }
    break;
  }

  _legend->position( x, y );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the scene dirty?
//
///////////////////////////////////////////////////////////////////////////////

bool SceneManager::dirty() const
{
  Guard guard ( _mutex );

  return _dirty;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the dirty flag
//
///////////////////////////////////////////////////////////////////////////////

void SceneManager::dirty( bool b )
{
  Guard guard ( _mutex );

  _dirty = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the timestep type.
//
///////////////////////////////////////////////////////////////////////////////

void SceneManager::timestepType( Settings::TimestepType type )
{
  _animateNode->settings()->timestepType( type );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the timestep type.
//
///////////////////////////////////////////////////////////////////////////////

OsgTools::Animate::Settings::TimestepType SceneManager::timestepType( ) const
{
  return _animateNode->settings()->timestepType( );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show past events.
//
///////////////////////////////////////////////////////////////////////////////

void SceneManager::showPastEvents ( bool b )
{
  _animateNode->settings()->showPastDays ( b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show past events.
//
///////////////////////////////////////////////////////////////////////////////

bool SceneManager::showPastEvents () const
{
  return _animateNode->settings()->showPastDays ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the time window flag.
//
///////////////////////////////////////////////////////////////////////////////

void SceneManager::timeWindow ( bool b )
{
  _animateNode->settings()->timeWindow ( b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the time window flag.
//
///////////////////////////////////////////////////////////////////////////////

bool SceneManager::timeWindow () const
{
  return _animateNode->settings()->timeWindow ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of days in the time window.
//
///////////////////////////////////////////////////////////////////////////////

void SceneManager::timeWindowDuration ( unsigned int days )
{
  _animateNode->settings()->windowLength ( days );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of days in the time window.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int SceneManager::timeWindowDuration () const
{
  return _animateNode->settings()->windowLength ( );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the animation speed.
//
///////////////////////////////////////////////////////////////////////////////

void SceneManager::animationSpeed ( float speed )
{
  _animateNode->animationSpeed ( speed );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the animation speed.
//
///////////////////////////////////////////////////////////////////////////////

float SceneManager::animationSpeed () const
{
  return _animateNode->animationSpeed ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the animation.
//
///////////////////////////////////////////////////////////////////////////////

void SceneManager::startAnimation()
{
  _animateNode->settings()->animate ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Stop the animation.
//
///////////////////////////////////////////////////////////////////////////////

void SceneManager::stopAnimation()
{
  _animateNode->settings()->animate ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pause the animation.
//
///////////////////////////////////////////////////////////////////////////////

void SceneManager::pauseAnimation()
{
  _animateNode->settings()->pause ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear internal data.
//
///////////////////////////////////////////////////////////////////////////////

void SceneManager::clear()
{
  Guard guard ( _mutex );

  _layers.clear();
  _legend->clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Resize.
//
///////////////////////////////////////////////////////////////////////////////

void SceneManager::resize( unsigned int width, unsigned int height )
{
  Guard guard ( _mutex );

  _projectionNode->setMatrix( osg::Matrix::ortho2D( 0, width, 0, height ) );

  _width = width;
  _height = height;

  this->dirty( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a layer.
//
///////////////////////////////////////////////////////////////////////////////

void SceneManager::addLayer ( Layer *layer )
{
  USUL_TRACE_SCOPE;

  Guard guard ( _mutex );

  _layers[ layer->guid() ] = layer;

  this->dirty( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a layer.
//
///////////////////////////////////////////////////////////////////////////////

void SceneManager::removeLayer ( const std::string& guid )
{
  USUL_TRACE_SCOPE;

  Guard guard ( _mutex );

  Layers::iterator iter = _layers.find ( guid );

  if ( iter != _layers.end() )
  {
    _layers.erase ( iter );
    this->dirty ( true );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is this layer being rendered?
//
///////////////////////////////////////////////////////////////////////////////

bool SceneManager::hasLayer ( const std::string& guid ) const
{
  USUL_TRACE_SCOPE;

  Guard guard ( _mutex );

  Layers::const_iterator iter = _layers.find( guid );
  if( iter != _layers.end() )
    return true;

  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the data objects for this layer.
//
///////////////////////////////////////////////////////////////////////////////

SceneManager::Layer *  SceneManager::getLayer ( const std::string& guid )
{
  USUL_TRACE_SCOPE;

  Guard guard ( _mutex );

  Layers::const_iterator iter = _layers.find( guid );
  if( iter != _layers.end() )
  {
    return _layers[guid].get();
  }

  throw std::runtime_error( "Error 284022903: could not find layer id." );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *SceneManager::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::ISceneUpdate::IID:
    return static_cast < Usul::Interfaces::ISceneUpdate * > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the scene.
//
///////////////////////////////////////////////////////////////////////////////

void SceneManager::sceneUpdate( Usul::Interfaces::IUnknown* caller )
{
  this->buildScene( caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the show legend flag.
//
///////////////////////////////////////////////////////////////////////////////

void SceneManager::showLegend( bool b )
{
  if( b != _showLegend )
  {
    _showLegend = b;
    this->dirty( true );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the show legend flag.
//
///////////////////////////////////////////////////////////////////////////////

bool SceneManager::showLegend() const
{
  return _showLegend;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the legend width percentage.
//
///////////////////////////////////////////////////////////////////////////////

void SceneManager::legendWidth ( float p )
{
  _legendWidth = p;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the legend width percentage.
//
///////////////////////////////////////////////////////////////////////////////

float SceneManager::legendWidth() const
{
  return _legendWidth;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the legend padding.
//
///////////////////////////////////////////////////////////////////////////////

void SceneManager::legendPadding( const osg::Vec2& padding )
{
  _legendPadding = padding;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the legend padding.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec2& SceneManager::legendPadding () const
{
  return _legendPadding;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Height of each item showing in the legend.
//
///////////////////////////////////////////////////////////////////////////////

void SceneManager::legendHeightPerItem( unsigned int height )
{
  _legendHeightPerItem = height;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Height of each item showing in the legend.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int SceneManager::legendHeightPerItem() const
{
  return _legendHeightPerItem;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the legend position.
//
///////////////////////////////////////////////////////////////////////////////

void SceneManager::legendPosition ( LegendPosition position )
{
  _legendPosition = position;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the legend position.
//
///////////////////////////////////////////////////////////////////////////////

SceneManager::LegendPosition SceneManager::legendPosition () const
{
  return _legendPosition;
}
