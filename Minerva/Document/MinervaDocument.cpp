
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
#include "Minerva/Document/KmlWriter.h"
#include "Minerva/Core/Commands/StopAnimation.h"
#include "Minerva/Core/Commands/StartAnimation.h"
#include "Minerva/Core/Commands/PauseAnimation.h"
#include "Minerva/Core/Commands/AnimationSpeed.h"

#include "Usul/File/Path.h"
#include "Usul/Strings/Case.h"
#include "Usul/Interfaces/IOssimPlanetLayer.h"
#include "Usul/Interfaces/IPlayMovie.h"
#include "Usul/Interfaces/ILayerExtents.h"
#include "Usul/Interfaces/IClonable.h"
#include "Usul/Interfaces/ICommand.h"

using namespace Minerva::Document;


USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( MinervaDocument, MinervaDocument::BaseClass );
USUL_IMPLEMENT_TYPE_ID ( MinervaDocument );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MinervaDocument::MinervaDocument() : BaseClass( "Minerva Document" ),
_layers(),
_favorites(),
_sceneManager ( new Minerva::Core::Scene::SceneManager ),
_planet ( new Magrathea::Planet ),
_useDistributed ( false ),
_sessionName(),
_distributed ( new Minerva::Core::GUI::Controller ),
_groupMap(),
SERIALIZE_XML_INITIALIZER_LIST
{
  // Initialize the planet.
  _planet->init();
  _planet->root()->addChild( _sceneManager->root() );

  SERIALIZE_XML_ADD_MEMBER ( _layers );
  SERIALIZE_XML_ADD_MEMBER ( _favorites );
  SERIALIZE_XML_ADD_MEMBER ( _useDistributed );
  SERIALIZE_XML_ADD_MEMBER ( _sessionName );
  SERIALIZE_XML_ADD_MEMBER ( _distributed );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MinervaDocument::~MinervaDocument()
{
  _distributed->deleteSession();
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
  case Usul::Interfaces::IDistributedVR::IID:
    return static_cast < Usul::Interfaces::IDistributedVR* > ( this );
  case Usul::Interfaces::IGroup::IID:
    return static_cast < Usul::Interfaces::IGroup* > ( this );
  case Minerva::Interfaces::IAnimationControl::IID:
    return static_cast < Minerva::Interfaces::IAnimationControl * > ( this );
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
  return ( ext == "minerva" || ext == "kwl" );
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
  if( "kwl" == ext )
  {
    _planet->readKWL( filename );
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
  else if( "kml" == ext || "kmz" == ext )
  {
    KmlWriter writer ( filename, caller, this );
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
  _favorites.clear();
  _sceneManager->clear();
  this->_connectToDistributedSession();
  _distributed->deleteSession();
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
//  View the layer extents.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::viewLayerExtents ( Usul::Interfaces::IUnknown * layer )
{
  Usul::Interfaces::ILayerExtents::QueryPtr extents ( layer );
  if ( extents.valid() )
  {
    double lat ( 0.0 ), lon ( 0.0 ), height ( 0.0 );

    extents->layerExtents( lat, lon, height );
    _planet->gotoLocation ( lat, lon, height );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the layer operation.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::setLayerOperation( const std::string& optype, int val, Usul::Interfaces::IUnknown * layer )
{
  Usul::Interfaces::IOssimPlanetLayer::QueryPtr ossim ( layer );

  if( ossim.valid() )
  {
    osg::ref_ptr < ossimPlanetTextureLayer > texture ( ossim->ossimPlanetLayer() );

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
  _sceneManager->showPastEvents ( accumulate );
  _sceneManager->animationSpeed ( speed );
  _sceneManager->timeWindow ( timeWindow );
  _sceneManager->timeWindowDuration ( numDays );
  _sceneManager->startAnimation();

  this->_startAnimationDistributed( speed, accumulate, true, timeWindow, numDays );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create and execute StopAnimation command.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::animationSpeedCommand( double value )
{
  Minerva::Core::Commands::AnimationSpeed::RefPtr command ( new Minerva::Core::Commands::AnimationSpeed ( value ) );
  this->_executeCommand ( command.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create and execute StartAnimation command.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::startAnimationCommand()
{
  Minerva::Core::Commands::StartAnimation::RefPtr command ( new Minerva::Core::Commands::StartAnimation );
  this->_executeCommand ( command.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create and execute StopAnimation command.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::stopAnimationCommand()
{
  Minerva::Core::Commands::StopAnimation::RefPtr command ( new Minerva::Core::Commands::StopAnimation );
  this->_executeCommand ( command.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create and execute PauseAnimation command.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::pauseAnimationCommand()
{
  Minerva::Core::Commands::PauseAnimation::RefPtr command ( new Minerva::Core::Commands::PauseAnimation );
  this->_executeCommand ( command.get() );
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
//  Set percent screen width for legend.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::percentScreenWidth ( float percent )
{
  _sceneManager->legendWidth ( percent );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set percent screen width for legend.
//
///////////////////////////////////////////////////////////////////////////////

float MinervaDocument::percentScreenWidth()
{
  return _sceneManager->legendWidth();
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

void MinervaDocument::sceneUpdate( Usul::Interfaces::IUnknown *caller )
{
  _sceneManager->buildScene( caller );
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

void MinervaDocument::addToFavorites( Usul::Interfaces::IUnknown* unknown )
{
  Usul::Interfaces::ILayer::QueryPtr layer ( unknown );

  if( layer.valid() )
    _favorites[layer->name()] = layer;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a favorite from the string.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown * MinervaDocument::createFavorite( const std::string& name ) const
{
  Favorites::const_iterator iter = _favorites.find( name );
  if( iter != _favorites.end() )
  {
    Usul::Interfaces::IClonable::QueryPtr clonable ( const_cast < Usul::Interfaces::ILayer* > ( iter->second.get() ) );

    if( clonable.valid() )
    {
      /// Clone the "template"
      Usul::Interfaces::ILayer::QueryPtr layer ( clonable->clone() );

      /// Make sure that the favorite is shown.
      if( layer.valid() )
        layer->showLayer( true );

      /// Return the layer we created.
      return layer.release();
    }
  }

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


///////////////////////////////////////////////////////////////////////////////
//
//  Connect the the distributed session.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::_connectToDistributedSession()
{
  if ( _useDistributed && !_distributed->connected() )
  {
    _distributed->connectToSession ( _sessionName );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove layer with given id.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::_removeLayerDistributed( Usul::Interfaces::ILayer *layer )
{
  if( _useDistributed )
  {
    // Lazy connection.
    this->_connectToDistributedSession();

    _distributed->removeLayer( layer );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show layer.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::_showLayerDistributed ( Usul::Interfaces::ILayer *Layer )
{
  if ( _useDistributed )
  {
    // Lazy connection.
    this->_connectToDistributedSession();

    _distributed->showLayer( Layer );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Modify layer.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::_modifyLayerDistributed ( Usul::Interfaces::ILayer *layer )
{
  if ( _useDistributed )
  {
    // Lazy connection.
    this->_connectToDistributedSession();

    _distributed->modifyLayer( layer );  
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start animation.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::_startAnimationDistributed ( float speed, bool accumulate, bool dateTimeStep, bool timeWindow, unsigned int numDays )
{
  if( _useDistributed )
  {
    // Lazy connection.
    this->_connectToDistributedSession();

    _distributed->startAnimation( speed, accumulate, true, timeWindow, numDays, _sceneManager->timestepType() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Play a movie.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::playMovie ( const osg::Vec3f& position, const osg::Vec3f& width, const osg::Vec3f& height, const std::string& path )
{
  if( _useDistributed )
  {
    // Lazy connection.
    this->_connectToDistributedSession();

    _distributed->playMovie ( position, width, height, path );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pause a movie.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::pause( )
{
  if( _useDistributed )
  {
    // Lazy connection.
    this->_connectToDistributedSession();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get group with given key.  Creates one if doesn't exist
//
///////////////////////////////////////////////////////////////////////////////

osg::Group* MinervaDocument::getGroup ( const std::string &key )
{
  osg::ref_ptr<osg::Group> &group = _groupMap[ key ];

  // Has the group been created
  if ( !group.valid() )
  {
    // Make a new group
    group = new osg::Group;

    // Set the name
    group->setName( key );

    // Add the group to the scene
    _planet->root()->addChild( group.get() );
  }

  return group.get();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Remove group with given key
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::removeGroup( const std::string &key )
{
  osg::ref_ptr<osg::Group> &group = _groupMap[key];
  _planet->root()->removeChild ( group.get() );
  group = 0x0;

  // Remove key from group map.
  _groupMap.erase ( key );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the group created?
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::hasGroup ( const std::string& key )
{
  GroupMap::const_iterator i = _groupMap.find ( key );
  return i != _groupMap.end();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a layer.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::removeLayer ( Usul::Interfaces::ILayer * layer )
{
  Usul::Interfaces::IOssimPlanetLayer::QueryPtr ossim ( layer );
  if( ossim.valid() )
  {
    _planet->removeLayer( ossim->ossimPlanetLayer() );
  }
  else
  {
    this->_removeLayerDistributed( layer );
    _sceneManager->removeLayer( layer->guid() );
  }

  Layers::iterator doomed ( std::find( _layers.begin(), _layers.end(), Usul::Interfaces::ILayer::QueryPtr ( layer ) ) );
  if( doomed != _layers.end() )
    _layers.erase( doomed );

  this->modified( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Hide a layer.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::hideLayer   ( Usul::Interfaces::ILayer * layer )
{
  this->_removeLayerDistributed( layer );
  layer->showLayer( false );
  _sceneManager->dirty( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show a layer.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::showLayer   ( Usul::Interfaces::ILayer * layer )
{
  this->_showLayerDistributed( layer );
  layer->showLayer ( true );
  _sceneManager->dirty( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a layer.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::addLayer ( Usul::Interfaces::ILayer * layer, Unknown *caller )
{
  this->_addLayer( layer, caller );
  _layers.push_back( layer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a layer.
//
///////////////////////////////////////////////////////////////////////////////

void  MinervaDocument::_addLayer ( Usul::Interfaces::ILayer * layer, Unknown *caller )
{
  try
  {
    Usul::Interfaces::IOssimPlanetLayer::QueryPtr ossim ( layer );
    if( ossim.valid() )
    {
      _planet->addLayer( ossim->ossimPlanetLayer() );
    }
    else
    {
      this->_showLayerDistributed( layer );

      Usul::Interfaces::IVectorLayer::QueryPtr vector ( layer );
      if( vector.valid() )
      {
        vector->buildVectorData( caller );
        _sceneManager->addLayer( layer );  
      }
    }
  }
  catch ( const std::exception& e )
  {
    std::cout << "Error 2006879022: " << e.what() << std::endl;
  }
  catch ( ... )
  {
    std::cout << "Error 4156147184: Unknown exception caught while trying to add a layer." << std::endl;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Modify a layer.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::modifyLayer ( Usul::Interfaces::ILayer * layer, Unknown *caller )
{
  Usul::Interfaces::IVectorLayer::QueryPtr vector ( layer );
  if( vector.valid() )
  {
    this->_modifyLayerDistributed( layer );
    vector->modifyVectorData ( caller );
    _sceneManager->dirty( true );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Modify a layer.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::deserialize ( const XmlTree::Node &node )
{
  _dataMemberMap.deserialize ( node );

  for( Layers::iterator iter = _layers.begin(); iter != _layers.end(); ++iter )
  {
    this->_addLayer( (*iter).get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the layers.
//
///////////////////////////////////////////////////////////////////////////////

MinervaDocument::Layers& MinervaDocument::layers()
{
  return _layers;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the layers.
//
///////////////////////////////////////////////////////////////////////////////

const MinervaDocument::Layers& MinervaDocument::layers() const
{
  return _layers;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute a command.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::_executeCommand ( Usul::Interfaces::ICommand* command )
{
  if ( 0x0 != command )
  { 
    // Send the command to the distributed client.
    if ( _distributed.valid() && _useDistributed )
    {
      // Lazy connection.
      this->_connectToDistributedSession();

      // Send the command.
      _distributed->sendCommand ( command );
    }

    // Execute the command.
    command->execute( this->queryInterface( Usul::Interfaces::IUnknown::IID ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the animation.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::startAnimation()
{
  _sceneManager->startAnimation();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Stop the animation.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::stopAnimation()
{
  _sceneManager->stopAnimation();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Stop the animation.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::pauseAnimation()
{
  _sceneManager->pauseAnimation();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the animate speed.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::animateSpeed ( double speed )
{
  _sceneManager->animationSpeed ( speed );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the animate speed.
//
///////////////////////////////////////////////////////////////////////////////

double MinervaDocument::animateSpeed () const
{
  return _sceneManager->animationSpeed ();
}
