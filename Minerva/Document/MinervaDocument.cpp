
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Document/MinervaDocument.h"
#include "Minerva/Document/MinervaReader.h"
#include "Minerva/Document/MinervaWriter.h"

#include "Usul/File/Path.h"
#include "Usul/Strings/Case.h"

#include "Minerva/Core/Layers/LineLayer.h"
#include "Minerva/Core/Layers/PolygonLayer.h"
#include "Minerva/Core/Layers/PointLayer.h"
#include "Minerva/Core/Layers/PointTimeLayer.h"
#include "Minerva/Core/Layers/RLayer.h"
#include "Minerva/Core/Layers/PolygonTimeLayer.h"

using namespace Minerva::Document;


USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( MinervaDocument, MinervaDocument::BaseClass );
USUL_IMPLEMENT_TYPE_ID ( MinervaDocument );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MinervaDocument::MinervaDocument() : BaseClass( "Minerva Document" ),
_favorites(),
_sceneManager ( new Minerva::Core::Scene::SceneManager ),
_planet ( new Magrathea::Planet )
{
  _planet->init();
  _planet->root()->addChild( _sceneManager->root() );

  SERIALIZE_XML_ADD_MEMBER ( _favorites );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MinervaDocument::~MinervaDocument()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *MinervaDocument::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IBuildScene::IID:
    return static_cast < Usul::Interfaces::IBuildScene* > ( this );
  case Usul::Interfaces::ISceneUpdate::IID:
    return static_cast < Usul::Interfaces::ISceneUpdate* > ( this );
  case Usul::Interfaces::IDatabasePager::IID:
    return static_cast < Usul::Interfaces::IDatabasePager* > ( this );
  case Usul::Interfaces::IMatrixManipulator::IID:
    return static_cast < Usul::Interfaces::IMatrixManipulator* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::canExport ( const std::string &file ) const
{
  return this->canSave ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::canInsert ( const std::string &file ) const
{
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::canOpen ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "minerva" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::canSave ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "minerva" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can export.
//
///////////////////////////////////////////////////////////////////////////////

MinervaDocument::Filters MinervaDocument::filtersExport() const
{
  return this->filtersSave();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can insert.
//
///////////////////////////////////////////////////////////////////////////////

MinervaDocument::Filters MinervaDocument::filtersInsert() const
{
  Filters filters;
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can open.
//
///////////////////////////////////////////////////////////////////////////////

MinervaDocument::Filters MinervaDocument::filtersOpen()   const
{
  Filters filters;
  filters.push_back ( Filter ( "Minerva (*.minerva)", "*.minerva" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can save.
//
///////////////////////////////////////////////////////////////////////////////

MinervaDocument::Filters MinervaDocument::filtersSave()   const
{
  Filters filters;
  filters.push_back ( Filter ( "Minerva (*.minerva)", "*.minerva" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file and add it to existing document's data.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::read ( const std::string &filename, Unknown *caller )
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( filename ) ) );

  if( "minerva" == ext )
  {
    MinervaReader reader ( filename, caller, this );
    reader();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the document to given file name. Does not rename this document.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::write ( const std::string &filename, Unknown *caller ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( filename ) ) );

  if( "minerva" == ext )
  {
    MinervaWriter writer ( filename, caller, this );
    writer();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear any existing data.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::clear ( Unknown *caller )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node * MinervaDocument::buildScene ( const BaseClass::Options &options, Unknown *caller )
{
  return _planet->root();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the layer.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::removeLayer ( Minerva::Core::Layers::Layer * layer )
{
  _sceneManager->removeLayer( layer->guid() );
  this->modified( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Hide the layer.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::hideLayer   ( Minerva::Core::Layers::Layer * layer )
{
  layer->showLayer ( false );
  _sceneManager->dirty( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show the layer.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::showLayer   ( Minerva::Core::Layers::Layer * layer )
{
  layer->showLayer ( true );
  _sceneManager->dirty( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the layer.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::addLayer   ( Minerva::Core::Layers::Layer * layer, Unknown *caller )
{
  layer->buildDataObjects( caller );
  _sceneManager->addLayer( layer );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Modify the layer.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::modifyLayer ( Minerva::Core::Layers::Layer * layer, Unknown *caller )
{
  layer->modify( caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  View the layer extents.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::viewLayerExtents ( ossimPlanetTextureLayer * layer )
{
  if( 0x0 != layer )
  {
    double lat ( 0.0 ), lon ( 0.0 ), height ( 0.0 );

    layer->getCenterLatLonLength( lat, lon, height );
    _planet->gotoLocation ( lat, lon, height );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add an ossimPlanet texture layer.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::addLayer       ( ossimPlanetTextureLayer * layer )
{
  _planet->addLayer( layer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove an ossimPlanet texture layer.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::removeLayer    ( ossimPlanetTextureLayer * layer )
{
  _planet->removeLayer( layer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the layer operation.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::setLayerOperation( const std::string& optype, int val, ossimPlanetTextureLayer * layer )
{
  osg::ref_ptr < ossimPlanetTextureLayer > texture ( layer );

  if( texture.valid() )
  { 
    if( !_planet->hasLayerOperation( texture.get() ) )
    {
      _planet->addLayerOperation( texture.get() );
      _planet->removeLayer( texture.get() );
    }             

    if( optype == "Opacity"  )
    {            
      float oVal = static_cast< float >( ( static_cast< float >( val ) ) / ( 100.0f ) );
      _planet->opacity( oVal );   
    }
    else if( optype == "Top" )
    {
      _planet->top();
    }
    else if( optype == "Reference" )
    {
      _planet->reference();
    }
    else if( optype == "AbsoluteDifference" )
    {
      _planet->absoluteDifference();
    }
    else if( optype == "FalseColorReplacement" )
    {
      _planet->falseColorReplacement();
    }
    else if( optype == "HorizontalSwipe" )
    {
      float hVal = static_cast< float >( ( static_cast< float >( val ) ) / ( 100.0f ) );
      _planet->horizontalSwipe( hVal );
    }
    else if( optype == "VerticalSwipe" )
    {
      float vVal = static_cast< float >( ( static_cast< float >( val ) ) / ( 100.0f ) );
      _planet->verticalSwipe( vVal );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the timestep type.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::timestepType( Settings::TimestepType type )
{
  _sceneManager->timestepType( type );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the timestep type.
//
///////////////////////////////////////////////////////////////////////////////

OsgTools::Animate::Settings::TimestepType MinervaDocument::timestepType( ) const
{
  return _sceneManager->timestepType();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the animation.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::startAnimation( float speed, bool accumulate, bool timeWindow, int numDays )
{
  _sceneManager->animate(true, accumulate, speed, timeWindow, numDays);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Stop the animation.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::stopAnimation()
{
  _sceneManager->animate(false, false, 0.0, false, 0);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Resize.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::resize ( unsigned int w, unsigned int h )
{
  _sceneManager->resize( w, h );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is elevation enabled?
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::elevationEnabled() const
{
  return _planet->elevationEnabled();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the elevation enabled flag.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::elevationEnabled( bool val )
{
  _planet->elevationEnabled( val );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the hud enabled?
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::hudEnabled() const
{
  return _planet->hudEnabled();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the hud enabled flag.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::hudEnabled( bool val )
{
  _planet->hudEnabled ( val );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is ephemeris enabled?
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::ephemerisFlag() const
{
  return _planet->ephemerisFlag();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the ephemeris flag.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::ephemerisFlag( bool val )
{
  _planet->ephemerisFlag ( val );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the elevation exageration.
//
///////////////////////////////////////////////////////////////////////////////

float MinervaDocument::elevationExag() const
{
  return _planet->elevationExag();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the elevation exageration.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::elevationExag( float elevExagVal )
{
  _planet->elevationExag( elevExagVal );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the elevation patch size.
//
///////////////////////////////////////////////////////////////////////////////

int MinervaDocument::elevationPatchSize() const
{
  return _planet->elevationPatchSize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the elevation patch size.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::elevationPatchSize( float elevEstimateVal )
{
  _planet->elevationPatchSize( elevEstimateVal );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the level of detail.
//
///////////////////////////////////////////////////////////////////////////////

int MinervaDocument::levelDetail() const
{
  return _planet->levelDetail();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the level of detail.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::levelDetail( float levelDetailVal )
{
  _planet->levelDetail( levelDetailVal );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the elevation cache directory.
//
///////////////////////////////////////////////////////////////////////////////

std::string MinervaDocument::elevationCacheDir() const
{
  return _planet->elevationCacheDir();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the elevation cache directory.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::elevationCacheDir( const std::string& directory )
{
  _planet->elevationCacheDir( directory );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the lat long grid showing.
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::latLongGrid() const
{
  return _planet->showLatLongGrid();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set whether the lat long grid should be shown.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::latLongGrid( bool b )
{
  _planet->showLatLongGrid( b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the legend showing?
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::showLegend() const
{
  return _sceneManager->showLegend();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the show legend flag.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::showLegend( bool b )
{
  _sceneManager->showLegend( b );
}

///////////////////////////////////////////////////////////////////////////////
//
// Set the movie mode on / off. 
//
///////////////////////////////////////////////////////////////////////////////
void MinervaDocument::setMovieMode( bool b )
{
  std::cout << " Test movie mode: " << std::endl;
  

}

///////////////////////////////////////////////////////////////////////////////
//
//  Get the matrix manipulator.
//
///////////////////////////////////////////////////////////////////////////////

osgGA::MatrixManipulator * MinervaDocument::getMatrixManipulator ()
{
  return _planet->manipulator();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the database pager.
//
///////////////////////////////////////////////////////////////////////////////

osgDB::DatabasePager * MinervaDocument::getDatabasePager ()
{
  return _planet->databasePager();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the scene.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::sceneUpdate( )
{
  _sceneManager->buildScene();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the viewer.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::viewer( Usul::Interfaces::IUnknown *viewer )
{
  _planet->viewer( viewer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add layer to the favorites.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::addToFavorites( Minerva::Core::Layers::Layer* layer )
{
  _favorites[layer->name()] = layer;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a favorite from the string.
//
///////////////////////////////////////////////////////////////////////////////

Minerva::Core::Layers::Layer * MinervaDocument::createFavorite( const std::string& name ) const
{
  Favorites::const_iterator iter = _favorites.find( name );
  if( iter != _favorites.end() )
    return iter->second->clone();
  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
// Get the list of favorites.
//
///////////////////////////////////////////////////////////////////////////////

MinervaDocument::Names MinervaDocument::favorites() const
{
  Names names;

  for( Favorites::const_iterator iter = _favorites.begin(); iter != _favorites.end(); ++iter )
  {
    names.push_back ( iter->first );
  }

  return names;
}
