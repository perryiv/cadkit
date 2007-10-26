
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
#include "Minerva/Core/Commands/AddLayer.h"
#include "Minerva/Core/Commands/RemoveLayer.h"
#include "Minerva/Core/Commands/ShowLayer.h"
#include "Minerva/Core/Commands/HideLayer.h"
#include "Minerva/Core/Commands/ToggleShown.h"
#include "Minerva/Core/Commands/ShowPastEvents.h"
#include "Minerva/Core/Visitors/TemporalAnimation.h"
#include "Minerva/Core/Visitors/FindMinMaxDates.h"

#include "Usul/Adaptors/Bind.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Commands/GenericCheckCommand.h"
#include "Usul/File/Path.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Strings/Case.h"
#include "Usul/Interfaces/IOssimPlanetLayer.h"
#include "Usul/Interfaces/ILayerExtents.h"
#include "Usul/Interfaces/IClonable.h"
#include "Usul/Interfaces/ICommand.h"
#include "Usul/Interfaces/IFrameStamp.h"
#include "Usul/Interfaces/ITemporalData.h"
#include "Usul/Interfaces/IClippingDistance.h"
#include "Usul/Trace/Trace.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/System/Host.h"

#include "MenuKit/Button.h"
#include "MenuKit/ToggleButton.h"
#include "MenuKit/RadioButton.h"

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
_commandsSend ( false ),
_commandsReceive ( false ),
_sessionName(),
_sender ( new CommandSender ),
_receiver ( new CommandReceiver ),
_connection ( 0x0 ),
_commandUpdate ( 5000 ),
_commandJob ( 0x0 ),
_animateSettings ( new Minerva::Core::Animate::Settings ),
_datesDirty ( false ),
_lastDate ( boost::date_time::min_date_time ),
_global ( new TimeSpan ),
_current ( new TimeSpan ),
_timeSpans (),
_lastTime ( -1.0 ),
_animationSpeed ( 0.1f ),
_timeSpanMenu ( new MenuKit::Menu ( "Time Spans" ) ),
SERIALIZE_XML_INITIALIZER_LIST
{
  // Initialize the planet.
  _planet->init();
  _planet->root()->addChild( _sceneManager->root() );

  SERIALIZE_XML_ADD_MEMBER ( _layers );
  SERIALIZE_XML_ADD_MEMBER ( _favorites );
  SERIALIZE_XML_ADD_MEMBER ( _commandsSend );
  SERIALIZE_XML_ADD_MEMBER ( _commandsReceive );
  SERIALIZE_XML_ADD_MEMBER ( _sessionName );
  SERIALIZE_XML_ADD_MEMBER ( _connection );
  SERIALIZE_XML_ADD_MEMBER ( _timeSpans );

#ifndef _MSC_VER
  //this->elevationEnabled ( false );
  this->showLegend ( false );

    
  if( Usul::System::Host::name() == "viz2" )
  {
    this->showLegend ( true );
    this->sceneManager()->legendWidth ( 0.75 );
    this->sceneManager()->legendPadding ( osg::Vec2 ( 20.0, 40.0 ) );
    this->sceneManager()->legendHeightPerItem ( 60 );
    this->sceneManager()->legendPosition( Minerva::Core::Scene::SceneManager::LEGEND_TOP_LEFT );
  }
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MinervaDocument::~MinervaDocument()
{
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( _sender, &CommandSender::deleteSession ), "5415547030" );

  _sceneManager = 0x0;
  _planet = 0x0;

  _sender = 0x0;
  _receiver = 0x0;

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
  case Usul::Interfaces::IMatrixManipulator::IID:
    return static_cast < Usul::Interfaces::IMatrixManipulator* > ( this );
  case Usul::Interfaces::IUpdateListener::IID:
    return static_cast < Usul::Interfaces::IUpdateListener * > ( this );
  case Minerva::Interfaces::IAnimationControl::IID:
    return static_cast < Minerva::Interfaces::IAnimationControl * > ( this );
  case Minerva::Interfaces::IAddLayer::IID:
    return static_cast < Minerva::Interfaces::IAddLayer * > ( this );
  case Minerva::Interfaces::IRemoveLayer::IID:
    return static_cast < Minerva::Interfaces::IRemoveLayer * > ( this );
  case Minerva::Interfaces::IDirtyScene::IID:
    return static_cast < Minerva::Interfaces::IDirtyScene * > ( this );
  case Usul::Interfaces::ILayerList::IID:
    return static_cast < Usul::Interfaces::ILayerList * > ( this );
  case Usul::Interfaces::IMenuAdd::IID:
    return static_cast < Usul::Interfaces::IMenuAdd * > ( this );
  case Usul::Interfaces::ICommandExecuteListener::IID:
    return static_cast < Usul::Interfaces::ICommandExecuteListener * > ( this );
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
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "minerva" || ext == "kml" || ext == "kmz" );
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
  Filters filters;
  filters.push_back ( Filter ( "Minerva (*.minerva)", "*.minerva" ) );
  filters.push_back ( Filter ( "Google Earth (*.kml)", "*.kml" ) );
  filters.push_back ( Filter ( "Google Earth Archive (*.kmz)", "*.kmz" ) );
  return filters;
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
//////////////////////////////////////////////////////////////////////////////

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

void MinervaDocument::read ( const std::string &filename, Unknown *caller, Unknown *progress )
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
  _sender->deleteSession();
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
  Guard guard ( this->mutex () );
  _animateSettings->timestepType( type );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the timestep type.
//
///////////////////////////////////////////////////////////////////////////////

Minerva::Core::Animate::Settings::TimestepType MinervaDocument::timestepType( ) const
{
  Guard guard ( this->mutex () );
  return _animateSettings->timestepType();
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
  if ( _commandsSend && !_sender->connected() )
  {
    _sender->connection ( _connection.get() );
    _sender->connectToSession ( _sessionName );
  }

  if ( _commandsReceive && !_receiver->connected() )
  {
    _receiver->connection ( _connection.get() );
    _receiver->connectToSession ( _sessionName );
  }
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
    _sceneManager->removeLayer( layer->guid() );
    _sceneManager->dirty ( true );
  }

  Layers::iterator doomed ( std::find( _layers.begin(), _layers.end(), Usul::Interfaces::ILayer::QueryPtr ( layer ) ) );
  if( doomed != _layers.end() )
    _layers.erase( doomed );

  // If it's temporal, we need to find the min and max dates again.
  Usul::Interfaces::ITemporalData::QueryPtr temporal ( layer );
  if ( temporal.valid () )
  {
    Guard guard ( this->mutex() );
    _datesDirty = true;
  }

  this->modified( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a layer.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::addLayer ( Usul::Interfaces::ILayer * layer )
{
  USUL_TRACE_SCOPE;

  this->_addLayer ( layer );

  {
    Guard guard ( this->mutex() );

    // Add the layer to our list.
    _layers.push_back( layer );
  }

  // If it's temporal, we need to find the min and max dates again.
  Usul::Interfaces::ITemporalData::QueryPtr temporal ( layer );
  if ( temporal.valid () )
  {
    Guard guard ( this->mutex() );
    _datesDirty = true;
  }

  // We are modified.
  this->modified ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a layer.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::_addLayer ( Usul::Interfaces::ILayer * layer )
{
  USUL_TRACE_SCOPE;

  try
  {
    Usul::Interfaces::IOssimPlanetLayer::QueryPtr ossim ( layer );
    if( ossim.valid() )
    {
      _planet->addLayer( ossim->ossimPlanetLayer() );
    }
    else
    {
      if ( _sceneManager.valid () )
      {
        _sceneManager->addLayer( layer );
        _sceneManager->dirty ( true );
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
//  Add a layer.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::addLayerCommand ( Usul::Interfaces::ILayer * layer )
{
  USUL_TRACE_SCOPE;

  Minerva::Core::Commands::AddLayer::RefPtr command ( new Minerva::Core::Commands::AddLayer ( layer ) );
  this->_executeCommand ( command.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a layer.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::removeLayerCommand ( Usul::Interfaces::ILayer * layer )
{
  USUL_TRACE_SCOPE;

  Minerva::Core::Commands::RemoveLayer::RefPtr command ( new Minerva::Core::Commands::RemoveLayer ( layer ) );
  this->_executeCommand ( command.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Modify a layer.  Does a remove and then an add.  This isn't the most
//  efficient way of doing things, but it ensures that the scene reflects 
//  the state of the layer.  This is especially true on the VR side.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::modifyLayerCommand ( Usul::Interfaces::ILayer * layer )
{
  USUL_TRACE_SCOPE;

  // Remove.
  this->removeLayerCommand ( layer );

  // Add.
  this->addLayerCommand ( layer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show the layer.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::showLayerCommand ( Usul::Interfaces::ILayer * layer )
{
  USUL_TRACE_SCOPE;

  Minerva::Core::Commands::ShowLayer::RefPtr command ( new Minerva::Core::Commands::ShowLayer ( layer ) );
  this->_executeCommand ( command.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Hide the layer.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::hideLayerCommand ( Usul::Interfaces::ILayer * layer )
{
  USUL_TRACE_SCOPE;

  Minerva::Core::Commands::HideLayer::RefPtr command ( new Minerva::Core::Commands::HideLayer ( layer ) );
  this->_executeCommand ( command.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Modify a layer.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::deserialize ( const XmlTree::Node &node )
{
  USUL_TRACE_SCOPE;

  _dataMemberMap.deserialize ( node );

  // Connect.
  this->_connectToDistributedSession ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the layers.
//
///////////////////////////////////////////////////////////////////////////////

MinervaDocument::Layers& MinervaDocument::layers()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex () );
  return _layers;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the layers.
//
///////////////////////////////////////////////////////////////////////////////

const MinervaDocument::Layers& MinervaDocument::layers() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex () );
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
    // Execute the command.
    command->execute( this->queryInterface( Usul::Interfaces::IUnknown::IID ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Command has executed..
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::commandExecuteNotify ( Usul::Commands::Command* command )
{
  if ( 0x0 != command )
  { 
    // Send the command to the distributed client.
    if ( _sender.valid() && _commandsSend )
    {
      // Send the command.
      _sender->sendCommand ( command );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Start the animation.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::startAnimation()
{
  Guard guard ( this->mutex () );
  _animateSettings->animate ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Stop the animation.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::stopAnimation()
{
  {
    Guard guard ( this->mutex () );
    _animateSettings->animate ( false );

    // Reset dates.
    if ( _global.valid () )
    {
      typedef Minerva::Core::Visitors::TemporalAnimation Visitor;
      Visitor::RefPtr visitor ( new Visitor ( _global->firstDate(), _global->lastDate() ) );
      this->accept ( *visitor );
    }
  }

  // Update the text.
  this->sceneManager()->dateText().setText( "" );
  this->sceneManager()->dateText().update ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Stop the animation.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::pauseAnimation()
{
  Guard guard ( this->mutex () );

  // Stop animation, but don't send the visitor to reset what nodes are shown.
  _animateSettings->animate ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the animate speed.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::animateSpeed ( double speed )
{
  Guard guard ( this->mutex () );
  _animationSpeed = speed;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the animate speed.
//
///////////////////////////////////////////////////////////////////////////////

double MinervaDocument::animateSpeed () const
{
  Guard guard ( this->mutex () );
  return _animationSpeed;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show past events.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::showPastEvents ( bool b )
{
  Guard guard ( this->mutex () );
  _animateSettings->showPastDays( b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show past events.
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::showPastEvents () const
{
  Guard guard ( this->mutex () );
  return _animateSettings->showPastDays( );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notification that a renderer is about to render.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::preRenderNotify ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  _planet->preRender ( caller );
  BaseClass::preRenderNotify ( caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notification that a renderer just rendered.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::postRenderNotify ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  _planet->postRender ( caller );
  BaseClass::postRenderNotify ( caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a view to this document.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::addView ( Usul::Interfaces::IView *view )
{
  USUL_TRACE_SCOPE;

  // Call the base classes on first.
  BaseClass::addView ( view );

  Usul::Interfaces::IClippingDistance::QueryPtr cd ( view );
  if ( cd.valid () )
    cd->setClippingDistances ( 0.001, 15 );

  // Initalize the OSG visitors.
  _planet->initVisitors ( view );
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Job to check for new commands.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  class CheckForCommands : public Usul::Jobs::Job
  {
  public:
    typedef Usul::Jobs::Job BaseClass;

    CheckForCommands ( Usul::Interfaces::IUnknown* caller, CommandReceiver *receiver ) : 
      BaseClass ( caller, false ),
      _caller ( caller ),
      _receiver ( receiver )
    {
      Usul::Interfaces::IProgressBar::QueryPtr pb ( this->progress () );
      if ( pb.valid () )
        pb->hideProgressBar ();
    }
  protected:

    virtual ~CheckForCommands () { }

    virtual void _started ()
    {
      if ( _receiver.valid () )
      {
        std::cout << "Processing Commands" << std::endl;
        _receiver->processCommands ( _caller.get () );
      }
    }

  private:
    Usul::Interfaces::IUnknown::QueryPtr _caller;
    CommandReceiver::RefPtr _receiver;
  };
}

///////////////////////////////////////////////////////////////////////////////
//
//  Update.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::updateNotify ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;

  // Rebuild the scene if it's dirty.
  if ( _sceneManager->dirty () )
  {
    _sceneManager->buildScene ( caller );
    _sceneManager->dirty ( false );
  }

  bool jobFinished ( _commandJob.valid() ? _commandJob->isDone () : true );

  // Check to see if we should receive commands...
  if ( _commandsReceive && _commandUpdate () && jobFinished )
  {
    Usul::Jobs::Job::RefPtr job ( new Detail::CheckForCommands ( caller, _receiver.get() ) );
    Usul::Jobs::Manager::instance().add ( job.get () );
    _commandJob = job;
  }

  // Animate.
  this->_animate ( caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Dirty the scene.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::dirtyScene ()
{
  _sceneManager->dirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the connection.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::connection ( Minerva::Core::DB::Connection* connection )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _connection = connection;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the connection.
//
///////////////////////////////////////////////////////////////////////////////

Minerva::Core::DB::Connection* MinervaDocument::connection ()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _connection;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the session.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::session ( const std::string& session )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _sessionName = session;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the session.
//
///////////////////////////////////////////////////////////////////////////////

const std::string & MinervaDocument::session () const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _sessionName;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Connect to the session.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::connectToSession ()
{
  this->_connectToDistributedSession ();
}

 
///////////////////////////////////////////////////////////////////////////////
//
//  Set the commands receive flag.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::commandsReceive ( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _commandsReceive = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the commands receive flag.
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::commandsReceive () const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _commandsReceive;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scene manager.
//
///////////////////////////////////////////////////////////////////////////////

Minerva::Core::Scene::SceneManager * MinervaDocument::sceneManager ()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _sceneManager.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scene manager.
//
///////////////////////////////////////////////////////////////////////////////

const Minerva::Core::Scene::SceneManager * MinervaDocument::sceneManager () const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _sceneManager.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the planet.
//
///////////////////////////////////////////////////////////////////////////////

Magrathea::Planet* MinervaDocument::planet ()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _planet.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Animate.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::_animate ( Usul::Interfaces::IUnknown *caller )
{
  Guard guard ( this->mutex () );

  // If we are suppose to animate...
  if ( _animateSettings->animate () )
  {
    if ( _datesDirty )
    {
      this->_findFirstLastDate();

      _datesDirty = false;

      // Rebuild the time spans menu.
      this->_buildTimeSpanMenu();
    }

    if ( false == _animateSettings->pause () && _current.valid() )
    {
      Usul::Interfaces::IFrameStamp::QueryPtr fs ( caller );
      double time ( fs.valid () ? fs->frameStamp()->getReferenceTime () : 0.0 );

      // Set the last time if it hasn't been set.
      if ( -1.0 == _lastTime )
        _lastTime = time;

      // Duration between last time the date was incremented.
      double duration ( time - _lastTime );

      Minerva::Core::Animate::Date lastDate ( _lastDate );

      //std::cout << "Time: " << time << " Duration: " << duration << " Speed: " << _animationSpeed <<  std::endl;

      // Animate if we should.
      if ( duration > 0.1f /*_animationSpeed*/ )
      {
        if( _animateSettings->timestepType() == Settings::DAY )
          lastDate.incrementDay();
        else if ( _animateSettings->timestepType() == Settings::MONTH )
          lastDate.incrementMonth();
        else if ( _animateSettings->timestepType() == Settings::YEAR )
          lastDate.incrementYear();
        _lastTime = time;

        // Wrap if we've gone beyond the last date.
        if( lastDate > _current->lastDate() )
        {
          lastDate = _current->firstDate();
        }

        Minerva::Core::Animate::Date firstDate ( lastDate );

        if ( _animateSettings->showPastDays() )
        {
          firstDate = _current->firstDate();
        }

        if( _animateSettings->timeWindow() )
        {
          firstDate = lastDate;
          firstDate.moveBackNumDays( _animateSettings->windowLength() );
        }

        // Update the text.
        this->sceneManager()->dateText().setText( lastDate.toString() );
        this->sceneManager()->dateText().update ();
        _lastDate = lastDate;

        // Set the dates to show.
        Minerva::Core::Visitors::TemporalAnimation::RefPtr visitor ( new Minerva::Core::Visitors::TemporalAnimation ( firstDate, lastDate ) );
        this->accept ( *visitor );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Have visitor visit all layes.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::accept ( Minerva::Core::Visitor& visitor )
{
  for ( Layers::iterator iter = _layers.begin(); iter != _layers.end(); ++iter )
  {
    if ( Minerva::Core::Layers::Layer *layer = dynamic_cast < Minerva::Core::Layers::Layer * > ( (*iter).get() ) )
      layer->accept ( visitor );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of layers.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int MinervaDocument::numberLayers () const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex () );
  return _layers.size ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the layer at position i.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::ILayer* MinervaDocument::layer ( unsigned int i )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex () );
  return _layers.at ( i );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add to the menu.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::menuAdd ( MenuKit::Menu& menu )
{
  typedef MenuKit::ToggleButton ToggleButton;
  typedef MenuKit::Button       Button;

  MenuKit::Menu::RefPtr m ( new MenuKit::Menu ( "Minerva" ) );

  Usul::Interfaces::IUnknown::QueryPtr me ( this );

  m->append ( new Button       ( new Minerva::Core::Commands::StartAnimation ( me ) ) );
  m->append ( new Button       ( new Minerva::Core::Commands::StopAnimation  ( me ) ) );
  m->append ( new Button       ( new Minerva::Core::Commands::PauseAnimation ( me ) ) );
  m->append ( new ToggleButton ( new Minerva::Core::Commands::ShowPastEvents ( me ) ) );

  {
    MenuKit::Menu::RefPtr layerMenu ( new MenuKit::Menu ( "Layers", MenuKit::Menu::VERTICAL ) );
    for ( Layers::iterator iter = _layers.begin(); iter != _layers.end (); ++ iter )
      layerMenu->append ( new ToggleButton ( new Minerva::Core::Commands::ToggleShown ( *iter ) ) );
    m->append ( layerMenu.get() );
  }

  // Time spans.
  this->_buildTimeSpanMenu();
  m->append ( _timeSpanMenu.get() );

  menu.append ( m );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build time span menu.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::_buildTimeSpanMenu()
{
  typedef MenuKit::RadioButton  RadioButton;

  _timeSpanMenu->clear();

  if ( _global.valid() )
    _timeSpanMenu->append ( new RadioButton ( Usul::Commands::genericCheckCommand ( _global->name(), 
                                                                          Usul::Adaptors::bind1<void> ( _global, Usul::Adaptors::memberFunction<void> ( this, &MinervaDocument::currentTimeSpan ) ), 
                                                                          Usul::Adaptors::bind1<bool> ( _global, Usul::Adaptors::memberFunction<bool> ( this, &MinervaDocument::isCurrentTimeSpan ) ) ) ) );

  for ( TimeSpans::iterator iter = _timeSpans.begin(); iter != _timeSpans.end(); ++iter )
  {
    _timeSpanMenu->append ( new RadioButton ( Usul::Commands::genericCheckCommand ( (*iter)->name(), 
                                                                          Usul::Adaptors::bind1<void> ( *iter, Usul::Adaptors::memberFunction<void> ( this, &MinervaDocument::currentTimeSpan ) ), 
                                                                          Usul::Adaptors::bind1<bool> ( *iter, Usul::Adaptors::memberFunction<bool> ( this, &MinervaDocument::isCurrentTimeSpan ) ) ) ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the current time span.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::currentTimeSpan ( TimeSpan::RefPtr span )
{
  Guard guard ( this );
  _current = span;

  if ( _current.valid() )
    _lastDate = _current->firstDate();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the current time span.
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::isCurrentTimeSpan ( TimeSpan::RefPtr span ) const
{
  Guard guard ( this );
  return span.get() ==  _current.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find first and last date.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::_findFirstLastDate()
{
  Guard guard ( this );

  Minerva::Core::Visitors::FindMinMaxDates::RefPtr findMinMax ( new Minerva::Core::Visitors::FindMinMaxDates );
  this->accept ( *findMinMax );

  TimeSpan::RefPtr global ( new TimeSpan );

  global->firstDate ( findMinMax->first() );
  global->lastDate ( findMinMax->last() );

  // Set the current to the new global.
  if ( _current == _global )
    _current = global;
  
  _lastDate = findMinMax->first();

  _global = global;
}
