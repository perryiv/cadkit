
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
#include "Minerva/Core/Commands/ChangeTimestepType.h"
#include "Minerva/Core/Factory/Readers.h"
#include "Minerva/Core/Visitors/TemporalAnimation.h"
#include "Minerva/Core/Visitors/FindMinMaxDates.h"
#include "Minerva/Core/Visitors/StackPoints.h"

#include "Usul/Adaptors/Bind.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Commands/GenericCheckCommand.h"
#include "Usul/File/Path.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Strings/Case.h"
#include "Usul/Interfaces/IAddRowLegend.h"
#include "Usul/Interfaces/ILayerExtents.h"
#include "Usul/Interfaces/ICommand.h"
#include "Usul/Interfaces/IFrameStamp.h"
#include "Usul/Interfaces/ITemporalData.h"
#include "Usul/Interfaces/IClippingDistance.h"
#include "Usul/Interfaces/IVectorLayer.h"
#include "Usul/Interfaces/IViewport.h"
#include "Usul/Trace/Trace.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/System/Host.h"

#include "MenuKit/Button.h"
#include "MenuKit/ToggleButton.h"
#include "MenuKit/RadioButton.h"

#include "OsgTools/Font.h"

#include "osg/Geode"
#include "osg/Light"

#include <sstream>

using namespace Minerva::Document;


USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( MinervaDocument, MinervaDocument::BaseClass );
USUL_IMPLEMENT_TYPE_ID ( MinervaDocument );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MinervaDocument::MinervaDocument() : 
  BaseClass( "Minerva Document" ),
  _dirty ( false ),
  _layers(),
  _sceneManager ( new Minerva::Core::Scene::SceneManager ),
  _root ( new osg::Group ),
  _camera ( new osg::Camera ),
  _dateText ( new osgText::Text ),
  _planet ( 0x0 ),
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
  _current ( _global ),
  _timeSpans (),
  _lastTime ( -1.0 ),
  _animationSpeed ( 0.1f ),
  _timeSpanMenu ( new MenuKit::Menu ( "Time Spans" ) ),
  _legend( new OsgTools::Legend::Legend ),
  _showLegend( true ),
  _legendWidth ( 0.40f ),
  _legendHeightPerItem ( 30 ),
  _legendPadding ( 20.0f, 20.0f ),
  _legendPosition ( LEGEND_BOTTOM_RIGHT ),
  _width( 0 ),
  _height( 0 ),
  SERIALIZE_XML_INITIALIZER_LIST
{
  SERIALIZE_XML_ADD_MEMBER ( _layers );
  SERIALIZE_XML_ADD_MEMBER ( _commandsSend );
  SERIALIZE_XML_ADD_MEMBER ( _commandsReceive );
  SERIALIZE_XML_ADD_MEMBER ( _sessionName );
  SERIALIZE_XML_ADD_MEMBER ( _connection );
  SERIALIZE_XML_ADD_MEMBER ( _timeSpans );

  _camera->setName ( "Minerva_Camera" );
  _camera->setRenderOrder ( osg::Camera::POST_RENDER );
  _camera->setReferenceFrame ( osg::Camera::ABSOLUTE_RF );
  _camera->setClearMask( GL_DEPTH_BUFFER_BIT );
  _camera->setViewMatrix( osg::Matrix::identity() );
  
  osg::ref_ptr<osg::StateSet> ss ( _camera->getOrCreateStateSet() );
  
  {
    osg::ref_ptr< osg::Light > light ( new osg::Light );
    light->setLightNum ( 1 );
    light->setDiffuse( osg::Vec4 ( 0.8, 0.8, 0.8, 1.0 ) );
    light->setDirection( osg::Vec3 ( 0.0, 0.0, -1.0f ) );
    
    ss->setAttributeAndModes ( light.get(), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
  }
  
  {
    osg::ref_ptr< osg::Light > light ( new osg::Light );
    light->setLightNum ( 0 );
    
    ss->setAttributeAndModes ( light.get(), osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );
  }
  
  osg::ref_ptr<osgText::Font> font ( OsgTools::Font::defaultFont() );
  _dateText->setFont( font.get() );
  _dateText->setCharacterSizeMode( osgText::Text::SCREEN_COORDS );
  _dateText->setText ( "" );
  _dateText->setColor( osg::Vec4 ( 0.0, 0.0, 0.0, 1.0 ) );
  
#ifndef _MSC_VER
#ifndef __APPLE__

  this->showLegend ( false );

    
  if( Usul::System::Host::name() == "viz4" )
  {
    this->showLegend ( true );
    this->sceneManager()->legendWidth ( 0.75 );
    this->sceneManager()->legendPadding ( osg::Vec2 ( 20.0, 40.0 ) );
    this->sceneManager()->legendHeightPerItem ( 60 );
    this->sceneManager()->legendPosition( Minerva::Core::Scene::SceneManager::LEGEND_TOP_LEFT );
  }
#endif
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

  _sender = 0x0;
  _receiver = 0x0;

  _planet = 0x0;
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
  case Usul::Interfaces::IPlanetCoordinates::IID:
    return static_cast < Usul::Interfaces::IPlanetCoordinates* > ( this );
  case Usul::Interfaces::IElevationDatabase::IID:
    return static_cast < Usul::Interfaces::IElevationDatabase * > ( this );
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
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  
  Filters filters ( this->filtersInsert() );
  for ( Filters::const_iterator iter = filters.begin(); iter != filters.end(); ++iter )
  {
    if ( Usul::Strings::lowerCase ( Usul::File::extension ( iter->second ) ) == ext )
      return true;
  }
  
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
  return Minerva::Core::Factory::Readers::instance().filters();
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
  this->_makePlanet();
  
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( filename ) ) );

  if( "minerva" == ext )
  {
    MinervaReader reader ( filename, caller, this );
    reader();
  }
  else
  {
    Usul::Interfaces::IUnknown::RefPtr unknown ( Minerva::Core::Factory::Readers::instance().create ( ext ) );
    Usul::Interfaces::IRead::QueryPtr read ( unknown );
    
    if ( read.valid() )
      read->read ( filename, caller, progress );
    
    Usul::Interfaces::ILayer::QueryPtr layer ( read );
    this->addLayer ( layer.get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the document to given file name. Does not rename this document.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::write ( const std::string &filename, Unknown *caller, Unknown *progress ) const
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
  _sceneManager->clear();
  this->_connectToDistributedSession();
  _sender->deleteSession();
  _legend->clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node * MinervaDocument::buildScene ( const BaseClass::Options &options, Unknown *caller )
{
  this->_makePlanet();
  
  osg::ref_ptr<osg::Group> group ( _planet->buildScene() );
  group->addChild ( _sceneManager->root() );
  group->addChild ( _root.get() );
  return group.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  View the layer extents.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::viewLayerExtents ( Usul::Interfaces::IUnknown * layer )
{
#if 0
  Usul::Interfaces::ILayerExtents::QueryPtr extents ( layer );
  if ( extents.valid() )
  {
    double lat ( 0.0 ), lon ( 0.0 ), height ( 0.0 );

    extents->layerExtents( lat, lon, height );
    _planet->gotoLocation ( lat, lon, height );
  }
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the timestep type.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::timestepType( IAnimationControl::TimestepType type )
{
  Guard guard ( this->mutex () );
  _animateSettings->timestepType( static_cast < Minerva::Core::Animate::Settings::TimestepType > ( type ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the timestep type.
//
///////////////////////////////////////////////////////////////////////////////

Minerva::Interfaces::IAnimationControl::TimestepType MinervaDocument::timestepType( ) const
{
  Guard guard ( this->mutex () );
  return static_cast < Minerva::Interfaces::IAnimationControl::TimestepType > ( _animateSettings->timestepType() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the matrix manipulator.
//
///////////////////////////////////////////////////////////////////////////////

osgGA::MatrixManipulator * MinervaDocument::getMatrixManipulator ()
{
  return 0x0;
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
  _planet->removeLayer ( layer );
  
  Usul::Interfaces::IVectorLayer::QueryPtr vector ( layer );
  if( vector.valid() )
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
  this->dirty( true );
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
  this->dirty( true );
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
    _planet->addLayer ( layer );
    
    if ( _sceneManager.valid () && 0x0 != layer )
    {
      _sceneManager->addLayer( layer );
      _sceneManager->dirty ( true );
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
  _dateText->setText( "" );
  _dateText->update ();
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

#if 0
  Usul::Interfaces::IClippingDistance::QueryPtr cd ( view );
  if ( cd.valid () )
    cd->setClippingDistances ( 0.001, 15 );
#endif
  
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
  
  if ( this->dirty() )
  {
    Minerva::Core::Visitors::StackPoints::RefPtr visitor ( new Minerva::Core::Visitors::StackPoints );
    this->accept ( *visitor );
  }

  // Build the scene.
  this->_buildScene ( caller );

  bool jobFinished ( _commandJob.valid() ? _commandJob->isDone () : true );

  // Check to see if we should receive commands...
  if ( _commandsReceive && _commandUpdate () && jobFinished )
  {
    Usul::Jobs::Job::RefPtr job ( new Detail::CheckForCommands ( caller, _receiver.get() ) );
    Usul::Jobs::Manager::instance().addJob ( job.get () );
    _commandJob = job;
  }

  // Animate.
  this->_animate ( caller );
  
  this->dirty ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Dirty the scene.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::dirtyScene ()
{
  Guard guard ( this );
  _dirty = true;
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

      // Animate if we should.
      if ( duration > _animationSpeed )
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
        _dateText->setText( lastDate.toString() );
        _dateText->update ();
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

void MinervaDocument::menuAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown * caller )
{
  typedef MenuKit::ToggleButton ToggleButton;
  typedef MenuKit::Button       Button;
  typedef MenuKit::RadioButton  RadioButton;

  MenuKit::Menu::RefPtr m ( new MenuKit::Menu ( "Minerva" ) );

  Usul::Interfaces::IUnknown::QueryPtr me ( this );

  m->append ( new Button       ( new Minerva::Core::Commands::StartAnimation ( me ) ) );
  m->append ( new Button       ( new Minerva::Core::Commands::StopAnimation  ( me ) ) );
  m->append ( new Button       ( new Minerva::Core::Commands::PauseAnimation ( me ) ) );
  m->append ( new ToggleButton ( new Minerva::Core::Commands::ShowPastEvents ( me ) ) );

  MenuKit::Menu::RefPtr speed ( new MenuKit::Menu ( "Speed" ) );
  speed->append ( new RadioButton ( new Minerva::Core::Commands::AnimationSpeed ( 0.1, me ) ) );
  speed->append ( new RadioButton ( new Minerva::Core::Commands::AnimationSpeed ( 0.5, me ) ) );
  speed->append ( new RadioButton ( new Minerva::Core::Commands::AnimationSpeed ( 1.0, me ) ) );
  speed->append ( new RadioButton ( new Minerva::Core::Commands::AnimationSpeed ( 2.0, me ) ) );
  m->append ( speed );

  MenuKit::Menu::RefPtr type ( new MenuKit::Menu ( "Timestep" ) );
  type->append ( new RadioButton ( new Minerva::Core::Commands::ChangeTimestepType ( Minerva::Interfaces::IAnimationControl::DAY,   me ) ) );
  type->append ( new RadioButton ( new Minerva::Core::Commands::ChangeTimestepType ( Minerva::Interfaces::IAnimationControl::MONTH, me ) ) );
  type->append ( new RadioButton ( new Minerva::Core::Commands::ChangeTimestepType ( Minerva::Interfaces::IAnimationControl::YEAR,  me ) ) );
  m->append ( type );

  {
    MenuKit::Menu::RefPtr layerMenu ( new MenuKit::Menu ( "Layers", MenuKit::Menu::VERTICAL ) );
    for ( Layers::iterator iter = _layers.begin(); iter != _layers.end (); ++ iter )
      layerMenu->append ( new ToggleButton ( new Minerva::Core::Commands::ToggleShown ( *iter ) ) );
    m->append ( layerMenu.get() );
  }

  // Time spans.
  this->_buildTimeSpanMenu();
  m->append ( _timeSpanMenu.get() );

#if USE_STAR_SYSTEM == 0
  MenuKit::Menu::RefPtr split ( new MenuKit::Menu ( "Split Metric" ) );
  split->append ( new RadioButton ( Usul::Commands::genericCheckCommand ( "1", 
                                                                          Usul::Adaptors::bind1<void> ( 1.0, Usul::Adaptors::memberFunction<void> ( this, &MinervaDocument::splitMetric ) ), 
                                                                          Usul::Adaptors::bind1<bool> ( 1.0, Usul::Adaptors::memberFunction<bool> ( this, &MinervaDocument::isSplitMetric ) ) ) ) );
  split->append ( new RadioButton ( Usul::Commands::genericCheckCommand ( "2", 
                                                                          Usul::Adaptors::bind1<void> ( 2.0, Usul::Adaptors::memberFunction<void> ( this, &MinervaDocument::splitMetric ) ), 
                                                                          Usul::Adaptors::bind1<bool> ( 2.0, Usul::Adaptors::memberFunction<bool> ( this, &MinervaDocument::isSplitMetric ) ) ) ) );
  split->append ( new RadioButton ( Usul::Commands::genericCheckCommand ( "3", 
                                                                          Usul::Adaptors::bind1<void> ( 3.0, Usul::Adaptors::memberFunction<void> ( this, &MinervaDocument::splitMetric ) ), 
                                                                          Usul::Adaptors::bind1<bool> ( 3.0, Usul::Adaptors::memberFunction<bool> ( this, &MinervaDocument::isSplitMetric ) ) ) ) );
  split->append ( new RadioButton ( Usul::Commands::genericCheckCommand ( "6", 
                                                                          Usul::Adaptors::bind1<void> ( 6.0, Usul::Adaptors::memberFunction<void> ( this, &MinervaDocument::splitMetric ) ), 
                                                                          Usul::Adaptors::bind1<bool> ( 6.0, Usul::Adaptors::memberFunction<bool> ( this, &MinervaDocument::isSplitMetric ) ) ) ) );
  split->append ( new RadioButton ( Usul::Commands::genericCheckCommand ( "9", 
                                                                          Usul::Adaptors::bind1<void> ( 9.0, Usul::Adaptors::memberFunction<void> ( this, &MinervaDocument::splitMetric ) ), 
                                                                          Usul::Adaptors::bind1<bool> ( 9.0, Usul::Adaptors::memberFunction<bool> ( this, &MinervaDocument::isSplitMetric ) ) ) ) );

  m->append ( split.get() );
#endif
  
  m->append ( new ToggleButton ( Usul::Commands::genericToggleCommand ( "Show Legend", 
                                                                          Usul::Adaptors::memberFunction<void> ( this, &MinervaDocument::showLegend ), 
                                                                          Usul::Adaptors::memberFunction<bool> ( this, &MinervaDocument::isShowLegend ) ) ) );

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


#if USE_STAR_SYSTEM == 0
///////////////////////////////////////////////////////////////////////////////
//
//  Set the split metric.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::splitMetric ( double value )
{
  _planet->splitMetric ( value );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the given number the current split metric?
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::isSplitMetric ( double value ) const
{
  return value == _planet->splitMetric();
}
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::_buildScene ( Usul::Interfaces::IUnknown *caller )
{
  Guard guard ( this );
  
  // Clear what we have.
  _root->removeChild( 0, _root->getNumChildren() );
  
  bool viewportChanged ( false );
  bool buildLegend ( false );
  
  Usul::Interfaces::IViewport::QueryPtr vp ( caller );
  if ( vp.valid () )
  {
    const unsigned int width  ( static_cast < unsigned int > ( vp->width () ) );
    const unsigned int height ( static_cast < unsigned int > ( vp->height () ) );
    
    if ( width != _width || height != _height )
    {
      _width = width;
      _height = height;
      viewportChanged = true;
      
      // Set the date text's position.
      _dateText->setPosition ( osg::Vec3( 5.0, _height - 25.0, 0.0 ) );
      
      // Set the build legend flag.
      buildLegend = true;
    }
  }
  
  if ( viewportChanged )
  {
    _camera->setViewport ( 0, 0, _width, _height );
    _camera->setProjectionMatrixAsOrtho ( 0, _width, 0, _height, -40.0, 40.0 );
    
    // Build the legend.
    this->_buildLegend( caller );
  }
  
  // Rebuild the scene if it's dirty.
  if ( _sceneManager->dirty () )
  {
    _sceneManager->buildScene ( Usul::Interfaces::IUnknown::QueryPtr ( this ) );
    _sceneManager->dirty ( false );
    
    // Set the build legend flag.
    buildLegend = true;
    
  }

  if ( buildLegend )
  {
    _camera->removeChild ( 0, _camera->getNumChildren() );
    
    // Build the legend.
    this->_buildLegend( caller );
    
    // Add the date text.
    osg::ref_ptr< osg::Geode > geode ( new osg::Geode );
    geode->addDrawable( _dateText.get() );
    
    _camera->addChild ( geode.get() );
  }
  
  _root->addChild ( _camera.get() );

  _planet->updateScene( caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert to planet coordinates.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::convertToPlanet ( const Usul::Math::Vec3d& orginal, Usul::Math::Vec3d& planetPoint ) const
{
  _planet->convertToPlanet ( orginal, planetPoint );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert from planet coordinates.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::convertFromPlanet ( const Usul::Math::Vec3d& planetPoint, Usul::Math::Vec3d& latLonPoint ) const
{
  _planet->convertFromPlanet ( planetPoint, latLonPoint );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the elevation at a lat, lon (IElevationDatabase).
//
///////////////////////////////////////////////////////////////////////////////

double MinervaDocument::elevationAtLatLong ( double lat, double lon ) const
{
  return _planet->elevationAtLatLong( lat, lon );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the planet.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::_makePlanet()
{
  // Only make it once.
  if ( _planet.valid() )
    return;
  
  _planet = new Planet;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the show legend flag.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::showLegend( bool b )
{
  Guard guard ( this );
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

bool MinervaDocument::isShowLegend() const
{
  Guard guard ( this );
  return _showLegend;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the legend width percentage.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::legendWidth ( float p )
{
  Guard guard ( this );
  _legendWidth = p;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the legend width percentage.
//
///////////////////////////////////////////////////////////////////////////////

float MinervaDocument::legendWidth() const
{
  Guard guard ( this );
  return _legendWidth;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the legend padding.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::legendPadding( const osg::Vec2& padding )
{
  Guard guard ( this );
  _legendPadding = padding;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the legend padding.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec2& MinervaDocument::legendPadding () const
{
  Guard guard ( this );
  return _legendPadding;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Height of each item showing in the legend.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::legendHeightPerItem( unsigned int height )
{
  Guard guard ( this );
  _legendHeightPerItem = height;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Height of each item showing in the legend.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int MinervaDocument::legendHeightPerItem() const
{
  Guard guard ( this );
  return _legendHeightPerItem;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the legend position.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::legendPosition ( LegendPosition position )
{
  Guard guard ( this );
  _legendPosition = position;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the legend position.
//
///////////////////////////////////////////////////////////////////////////////

MinervaDocument::LegendPosition MinervaDocument::legendPosition () const
{
  Guard guard ( this );
  return _legendPosition;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the legend.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::_buildLegend( Usul::Interfaces::IUnknown *caller )
{
  // Always clear.
  _legend->clear();
  
  if( this->isShowLegend() )
  {
    // Set the legend size.
    unsigned int legendWidth  ( static_cast < unsigned int > ( _width * _legendWidth ) );
    unsigned int legendHeight ( static_cast < unsigned int > ( _height - ( _legendPadding.y() * 2 ) ) );
    
    _legend->maximiumSize( legendWidth, legendHeight );
    _legend->heightPerItem( _legendHeightPerItem );
    
    for ( Layers::iterator iter = _layers.begin(); iter != _layers.end(); ++iter )
    {
      Usul::Interfaces::ILayer::QueryPtr layer ( *iter );
      if ( layer.valid() )
      {
        Usul::Interfaces::IAddRowLegend::QueryPtr addRow ( layer );
        if( layer->showLayer() && addRow.valid() && addRow->showInLegend() )
        {
          OsgTools::Legend::LegendObject::RefPtr row ( new OsgTools::Legend::LegendObject );
          addRow->addLegendRow( row.get() );
          _legend->addLegendObject( row.get() );
        }
      }
    }
    
    // Must be called after rows are added to the legend.
    this->_setLegendPosition( legendWidth );
    
    // Build the legend.
    _camera->addChild( _legend->buildScene() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the legend position.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::_setLegendPosition ( unsigned int legendWidth )
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

bool MinervaDocument::dirty() const
{
  Guard guard ( this );
  return _dirty;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the dirty flag
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::dirty( bool b )
{
  Guard guard ( this );
  _dirty = b;
}
