
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Document/MinervaDocument.h"
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
#include "Minerva/Core/DataObjects/DataObject.h"
#include "Minerva/Core/DataObjects/UserData.h"
#include "Minerva/Core/Factory/Readers.h"
#include "Minerva/Core/Visitors/TemporalAnimation.h"
#include "Minerva/Core/Visitors/FindMinMaxDates.h"
#include "Minerva/Core/Visitors/StackPoints.h"
#include "Minerva/Core/Visitors/ResizePoints.h"
#include "Minerva/Core/Visitors/BuildLegend.h"
#include "Minerva/Core/Utilities/ClampNearFar.h"
#include "Minerva/Core/Visitors/SetJobManager.h"
#include "Minerva/Core/Extents.h"
#include "Minerva/Core/TileEngine/LandModelEllipsoid.h"
#include "Minerva/Core/TileEngine/SplitCallbacks.h"

#include "MenuKit/Button.h"
#include "MenuKit/ToggleButton.h"
#include "MenuKit/RadioButton.h"

#include "OsgTools/Font.h"
#include "OsgTools/Group.h"

#include "Serialize/XML/Serialize.h"
#include "Serialize/XML/Deserialize.h"

#include "Usul/Adaptors/Bind.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Bits/Bits.h"
#include "Usul/Commands/GenericCheckCommand.h"
#include "Usul/Components/Manager.h"
#include "Usul/Documents/Manager.h"
#include "Usul/File/Path.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/IAddRowLegend.h"
#include "Usul/Interfaces/IAnimatePath.h"
#include "Usul/Interfaces/IAxes.h"
#include "Usul/Interfaces/ICommand.h"
#include "Usul/Interfaces/IClippingDistance.h"
#include "Usul/Interfaces/ICullSceneVisitor.h"
#include "Usul/Interfaces/ILayerExtents.h"
#include "Usul/Interfaces/ISceneIntersect.h"
#include "Usul/Interfaces/IViewport.h"
#include "Usul/Interfaces/IViewMatrix.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Math/Constants.h"
#include "Usul/Registry/Constants.h"
#include "Usul/Registry/Database.h"
#include "Usul/Strings/Case.h"
#include "Usul/System/Host.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"

#include "osg/CoordinateSystemNode"
#include "osg/Geode"
#include "osg/Light"

#include "osgGA/GUIEventAdapter"

#include "osgUtil/IntersectVisitor"
#include "osgUtil/CullVisitor"

#include <sstream>

using namespace Minerva::Document;


USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( MinervaDocument, MinervaDocument::BaseClass );
USUL_IMPLEMENT_TYPE_ID ( MinervaDocument );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MinervaDocument::MinervaDocument ( LogPtr log ) : 
  BaseClass( "Minerva Document" ),
#if USE_DISTRIBUTED == 1
  _commandsSend ( false ),
  _commandsReceive ( false ),
  _sessionName(),
  _sender ( new CommandSender ),
  _receiver ( new CommandReceiver ),
  _connection ( 0x0 ),
  _commandUpdate ( 5000 ),
  _commandJob ( 0x0 ),
#endif
  _dirty ( false ),
  _layersMenu ( new MenuKit::Menu ( "&Layers" ) ),
  _root ( new osg::Group ),
  _camera ( new osg::Camera ),
  _bodies (),
  _activeBody ( 0x0 ),
  _manager ( 0x0 ),
  _hud(),
  _callback ( 0x0 ),
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
  _showCompass ( true ),
  _allowSplit ( true ),
  _keepDetail ( false ),
  _log ( log ),
  SERIALIZE_XML_INITIALIZER_LIST
{
  // Serialization glue.
#if USE_DISTRIBUTED == 1
  this->_addMember ( "commands_send", _commandsSend );
  this->_addMember ( "commands_receive", _commandsReceive );
  this->_addMember ( "session_name", _sessionName );
  this->_addMember ( "connection", _connection );
#endif
  this->_addMember ( "time_spans", _timeSpans );
  this->_addMember ( "bodies", _bodies );

  _camera->setName ( "Minerva_Camera" );
  _camera->setRenderOrder ( osg::Camera::POST_RENDER );
  _camera->setReferenceFrame ( osg::Camera::ABSOLUTE_RF );
  _camera->setClearMask( GL_DEPTH_BUFFER_BIT );
  _camera->setViewMatrix( osg::Matrix::identity() );
  _camera->setComputeNearFarMode ( osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR );
  _camera->setCullingMode ( osg::CullSettings::NO_CULLING );
  
  osg::ref_ptr<osg::StateSet> ss ( _camera->getOrCreateStateSet() );
  
  {
    osg::ref_ptr< osg::Light > light ( new osg::Light );
    light->setLightNum ( 1 );
    light->setDiffuse( osg::Vec4 ( 0.8, 0.8, 0.8, 1.0 ) );
    light->setDirection( osg::Vec3 ( 0.0, 0.0, -1.0f ) );
    light->setPosition ( osg::Vec4 ( 0.0, 0.0, 1.0f, 0.0 ) );
    
    ss->setAttributeAndModes ( light.get(), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
  }
  
  {
    osg::ref_ptr< osg::Light > light ( new osg::Light );
    light->setLightNum ( 0 );
    
    ss->setAttributeAndModes ( light.get(), osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );
  }
  
#ifndef _MSC_VER
#ifndef __APPLE__

  this->showLegend ( false );
  this->showCompass ( false );
    
  if( Usul::System::Host::name() == "viz4" )
  {
    this->showLegend ( true );
    this->legendWidth ( 0.85 );
    this->legendPadding ( osg::Vec2 ( 20.0, 40.0 ) );
    this->legendHeightPerItem ( 60 );
    this->legendPosition( LEGEND_TOP_LEFT );
  }

  if ( Usul::System::Host::name() == "viz0" )
    this->showCompass ( true );
    
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
#if USE_DISTRIBUTED == 1
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( _sender, &CommandSender::deleteSession ), "5415547030" );

  _sender = 0x0;
  _receiver = 0x0;
#endif
  
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &MinervaDocument::_clear ), "1582439358" );

  _root = 0x0;
  _log = 0x0;
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
  case Minerva::Interfaces::IDirtyScene::IID:
    return static_cast < Minerva::Interfaces::IDirtyScene * > ( this );
  case Usul::Interfaces::IMenuAdd::IID:
    return static_cast < Usul::Interfaces::IMenuAdd * > ( this );
  case Usul::Interfaces::ICommandExecuteListener::IID:
    return static_cast < Usul::Interfaces::ICommandExecuteListener * > ( this );
  case Usul::Interfaces::IIntersectListener::IID:
    return static_cast < Usul::Interfaces::IIntersectListener * > ( this );
  case Usul::Interfaces::ITreeNode::IID:
    return static_cast < Usul::Interfaces::ITreeNode* > ( this );
  case Usul::Interfaces::IJobFinishedListener::IID:
    return static_cast < Usul::Interfaces::IJobFinishedListener* > ( this );
  case Usul::Interfaces::IMouseEventListener::IID:
    return static_cast < Usul::Interfaces::IMouseEventListener* > ( this );
  case Minerva::Interfaces::ILookAtLayer::IID:
    return static_cast < Minerva::Interfaces::ILookAtLayer * > ( this );
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
  Filters filters ( Minerva::Core::Factory::Readers::instance().filters() );
  Filters open ( Usul::Documents::Manager::instance().filtersOpen() );
  filters.insert ( filters.end(), open.begin(), open.end() );
  
  // Remove our filter until inserting is tested.
  filters.erase ( std::remove ( filters.begin(), 
                                filters.end(), 
                                Filter ( "Minerva (*.minerva)", "*.minerva" ) ),
                  filters.end() );
  
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

  if ( "minerva" == ext )
  {
    // Deserialize the xml tree.
    XmlTree::XercesLife life;
    XmlTree::Document::ValidRefPtr document ( new XmlTree::Document );
    document->load ( filename );
    this->deserialize ( *document );

    // Make a visitor to set the job manager.
    Minerva::Core::Visitors::SetJobManager::RefPtr setter ( new Minerva::Core::Visitors::SetJobManager ( this->_getJobManager() ) );

    // Loop through the bodies.
    for ( Bodies::iterator iter = _bodies.begin(); iter != _bodies.end(); ++iter )
    {
      Body::RefPtr body ( *iter );
      if ( body.valid() )
      {
        // Set the job manager for each body.
        body->accept ( *setter );
      }
    }
    
    if ( false == _bodies.empty() )
      this->activeBody ( _bodies.front() );
  }
  else
  {
    // Check the registered readers first.
    Usul::Interfaces::IUnknown::QueryPtr unknown ( Minerva::Core::Factory::Readers::instance().create ( ext ) );
    
    // If we didn't find one, ask the document manager.
    if ( false == unknown.valid() )
    {
      unknown = Usul::Interfaces::IUnknown::QueryPtr ( Usul::Documents::Manager::instance().find ( filename ).document.get() );
    }
    
    Usul::Interfaces::IRead::QueryPtr read ( unknown );
    
    if ( read.valid() )
    {
      // Create a job to read the file.
      // Pass the QueryPtr to memberFunction so that if the user removes the layer, there is still a reference until the job finishes.
      Usul::Jobs::Job::RefPtr job ( Usul::Jobs::create ( Usul::Adaptors::bind3 ( filename, caller, progress,
                                    Usul::Adaptors::memberFunction ( read, &Usul::Interfaces::IRead::read ) ), caller ) );

      // Add the job to the manager.
      Usul::Jobs::Manager::instance().addJob ( job );
    }
    
    Usul::Interfaces::ILayer::QueryPtr layer ( read );
    this->addLayer ( layer.get() );
  }

  // Reset all the log pointers.
  this->_setLog();
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
    Serialize::XML::serialize ( *this, filename );
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
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &MinervaDocument::_clear ), "2171542707" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::_clear()
{
  this->_connectToDistributedSession();
  
#if USE_DISTRIBUTED == 1
  if ( _sender.valid() ) _sender->deleteSession();
#endif
  
  if ( _legend.valid() ) _legend->clear();

  // Delete the tiles.
  OsgTools::Group::removeAllChildren ( _root.get() );
  if ( 1 != _root->referenceCount() )
    std::cout << "Warning 3380491951: Dangling reference to Minerva's scene root, memory leak?" << std::endl;
  
  // Clean up job manager.
  if ( 0x0 != _manager )
  {
    // Remove all queued jobs and cancel running jobs.
    _manager->cancel();
    
    // Wait for remaining jobs to finish.
    _manager->wait();
    
    // Delete the manager.
    delete _manager; _manager = 0x0;
  }

  // Clear the bodies.
  _bodies.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node * MinervaDocument::buildScene ( const BaseClass::Options &options, Unknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  
  // Make sure we have at least one body.
  this->_makePlanet();
  
  osg::ref_ptr<osg::Group> group ( new osg::Group );
  group->setName ( "Minerva document" );
  for ( Bodies::const_iterator iter = _bodies.begin(); iter != _bodies.end(); ++iter )
  {
    Body::RefPtr body ( *iter );
    
    if ( body.valid () )
      group->addChild ( body->scene() );
  }
  
  group->addChild ( _root.get() );
  group->addChild ( _hud.buildScene() );
  
  return group.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  View the layer extents.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::lookAtLayer ( Usul::Interfaces::IUnknown * layer )
{
  Usul::Interfaces::ILayerExtents::QueryPtr le ( layer );
  const double minLon ( le.valid() ? le->minLon() : -180.0 );
  const double minLat ( le.valid() ? le->minLat() :  -90.0 );
  const double maxLon ( le.valid() ? le->maxLon() :  180.0 );
  const double maxLat ( le.valid() ? le->maxLat() :   90.0 );
  
  Minerva::Core::Extents<osg::Vec2d> extents ( minLon, minLat, maxLon, maxLat );

  Body::RefPtr body ( this->activeBody() );

  if ( body.valid() )
  {
    // Get the center.
    osg::Vec2 center ( extents.center() );

    // For now use this formula to get meters per degree.
    // http://books.google.com/books?id=wu7zHtd2LO4C&pg=PA167&lpg=PA167&dq=meters+per+degree+average+earth&source=web&ots=yF1Q6sp1nP&sig=S8gdbKLvNzrXGKoUn3ha-oqYMaU&hl=en
    const double lat ( Usul::Math::DEG_TO_RAD * center[1] );
    const double metersPerDegree ( 111132.09 - 566.05 * Usul::Math::sin ( 2 * lat ) + 120 * Usul::Math::cos ( 4 * lat ) - 0.0002 * Usul::Math::cos ( 6 * lat ) );

    // Calculate an altitude.
    const double altitude ( ( extents.maximum() - extents.minimum() ).length() * metersPerDegree );

    // Heading, Pitch, and Roll (For future use).
    osg::Vec3d hpr ( 0.0, 0.0, 0.0 );

    // Convert to x,y,z on the planet.
    Usul::Math::Vec3d point;
    body->convertToPlanet ( Usul::Math::Vec3d ( center[0], center[1], altitude ), point );

    // Get the rotation.
    osg::Matrixd matrix ( body->planetRotationMatrix ( center[1], center[0], altitude, hpr[0] ) );

    // Rotation about x.
    osg::Matrixd RX ( osg::Matrixd::rotate ( Usul::Math::DEG_TO_RAD * hpr[1], 1, 0, 0 ) );

    // Rotation about y.
    osg::Matrixd RY ( osg::Matrixd::rotate ( Usul::Math::DEG_TO_RAD * hpr[2], 0, 1, 0 ) );

    osg::Matrix M ( matrix * RX * RY );
    //M.setTrans ( point[0], point[1], point[2] );

    // Look for plugin to play path.
    typedef Usul::Interfaces::IAnimatePath IAnimatePath;
    typedef Usul::Components::Manager PluginManager;

    IAnimatePath::QueryPtr animate ( PluginManager::instance().getInterface ( IAnimatePath::IID ) );
    Usul::Interfaces::IViewMatrix::QueryPtr vm ( Usul::Documents::Manager::instance().activeView() );

    if ( animate.valid() )
    {
      // Get the first and last matrix.
      const osg::Matrixd m1 ( vm.valid() ? vm->getViewMatrix() : osg::Matrixd() );
      const osg::Matrixd m2 ( osg::Matrixd::inverse ( M ) );

      // Prepare path.
      IAnimatePath::PackedMatrices matrices;
      matrices.push_back ( IAnimatePath::PackedMatrix ( m1.ptr(), m1.ptr() + 16 ) );
      matrices.push_back ( IAnimatePath::PackedMatrix ( m2.ptr(), m2.ptr() + 16 ) );

      // Animate through the path.
      animate->animatePath ( matrices, 50 );
    }
    else if ( vm.valid() )
    {
      vm->setViewMatrix ( M );
    }
  }
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
#if USE_DISTRIBUTED == 1
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
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a layer.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::removeLayer ( Usul::Interfaces::ILayer * layer )
{
  USUL_TRACE_SCOPE;

  if ( 0x0 == layer )
    return;

  Guard guard ( this->mutex() );

  // Get the active body.
  Body::RefPtr body ( this->activeBody() );
  
  if ( body.valid () )
  {
    Usul::Interfaces::IRasterLayer::QueryPtr rl ( layer );
    body->rasterRemove ( rl.get() );
    body->vectorRemove ( layer );
  }
  
  // Find the min and max dates again.
  Usul::Threads::Safe::set ( this->mutex(), true, _datesDirty );

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

  // Return now if the layer isn't valid.
  if ( 0x0 == layer )
    return;

  try
  {
    // Get the active body.
    Body::RefPtr body ( this->activeBody() );
    
    if ( body.valid() )
    {
      Usul::Interfaces::IElevationDatabase::QueryPtr elevation ( layer );
      Usul::Interfaces::IRasterLayer::QueryPtr rl ( layer );
      if ( rl.valid() )
      {
        if ( elevation.valid() )
          body->elevationAppend ( rl.get() );
        else
          body->rasterAppend ( rl.get() );
      }
      
      body->vectorAppend ( layer );
    }
    
    // Find the min and max dates again.
    Usul::Threads::Safe::set ( this->mutex(), true, _datesDirty );

    // We are modified.
    this->modified ( true );
    this->dirty( true );  
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
  
  // The dates are dirty.
  Usul::Threads::Safe::set ( this->mutex(), true, _datesDirty );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute a command.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::_executeCommand ( Usul::Interfaces::ICommand* command )
{
  USUL_TRACE_SCOPE;
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
  USUL_TRACE_SCOPE;
  
#if USE_DISTRIBUTED == 1
  
  if ( 0x0 != command )
  { 
    // Send the command to the distributed client.
    if ( _sender.valid() && _commandsSend )
    {
      // Send the command.
      _sender->sendCommand ( command );
    }
  }
  
#endif
}

///////////////////////////////////////////////////////////////////////////////
//
//  Start the animation.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::startAnimation()
{
  USUL_TRACE_SCOPE;
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
  USUL_TRACE_SCOPE;
  
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
  _hud.dateFeedback ( "" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Stop the animation.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::pauseAnimation()
{
  USUL_TRACE_SCOPE;
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
  USUL_TRACE_SCOPE;
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
  USUL_TRACE_SCOPE;
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
  USUL_TRACE_SCOPE;
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
  USUL_TRACE_SCOPE;
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
  Guard guard ( this );

  for ( Bodies::iterator iter = _bodies.begin(); iter != _bodies.end(); ++iter )
  {
    Body::RefPtr body ( *iter );
    if ( body.valid() )
      body->preRender ( caller );
  }

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
  Guard guard ( this );

  for ( Bodies::iterator iter = _bodies.begin(); iter != _bodies.end(); ++iter )
  {
    Body::RefPtr body ( *iter );
    if ( body.valid() )
    {
      body->postRender ( caller );
      
      // Draw again if a new texture has been added.
      if ( body->newTexturesLastFrame() > 0 || body->needsRedraw() )
        this->requestRedraw();
      
      // Request has been made.  Reset state.
      body->needsRedraw ( false );
    }
  }

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

  // Call the base class' on first.
  BaseClass::addView ( view );
  
  // Hide the axes.
  Usul::Interfaces::IAxes::QueryPtr axes ( view );
  if ( axes.valid() )
    axes->axesShown ( false );

  Usul::Interfaces::ICullSceneVisitor::QueryPtr csv ( view );
  if ( csv.valid() )
  {
    osg::ref_ptr<osgUtil::CullVisitor> cv ( csv->getCullSceneVisitor( 0x0 ) );
    cv->setClampProjectionMatrixCallback ( new Minerva::Core::Utilities::ClampNearFar ( *cv ) );
    cv->setInheritanceMask ( Usul::Bits::remove ( cv->getInheritanceMask(), osg::CullSettings::CLAMP_PROJECTION_MATRIX_CALLBACK ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a view to this document.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::removeView ( Usul::Interfaces::IView *view )
{
  // Call the base class' first.
  BaseClass::removeView ( view );
  
  // If there are no more views, remove the job finished listener.
  if ( 0 == this->numViews() )
  {
    Usul::Jobs::Manager *manager ( this->_getJobManager() );
    manager->removeJobFinishedListener ( Usul::Interfaces::IUnknown::QueryPtr ( this ) );
  }
}


#if USE_DISTRIBUTED == 1

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
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Update.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::updateNotify ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  
  if ( this->dirty() )
  {
    Minerva::Core::Visitors::StackPoints::RefPtr visitor ( new Minerva::Core::Visitors::StackPoints );
    this->accept ( *visitor );

    this->_buildLayerMenu();
  }
  
  // Update.
  Usul::Interfaces::IUnknown::QueryPtr unknown ( this->queryInterface ( Usul::Interfaces::IUnknown::IID ) );
  std::for_each ( _bodies.begin(), _bodies.end(), std::bind2nd ( std::mem_fun ( &Body::updateNotify ), unknown.get() ) );

  // Build the scene.
  this->_buildScene ( caller );

#if USE_DISTRIBUTED == 1
  const bool jobFinished ( _commandJob.valid() ? _commandJob->isDone () : true );

  // Check to see if we should receive commands...
  if ( _commandsReceive && _commandUpdate () && jobFinished )
  {
    Usul::Jobs::Job::RefPtr job ( new Detail::CheckForCommands ( caller, _receiver.get() ) );
    Usul::Jobs::Manager::instance().addJob ( job.get () );
    _commandJob = job;
  }
#endif
  
  // Animate.
  this->_animate ( caller );
  
  this->dirty ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the dirty scene flag.
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::dirtyScene() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _dirty;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Dirty the scene.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::dirtyScene ( bool b, Usul::Interfaces::IUnknown* caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  
  _dirty = b;
  
  // Get the active body.
  Body::RefPtr body ( this->activeBody() );
  Usul::Interfaces::IRasterLayer::QueryPtr rl ( caller );
  
  if ( body.valid() && rl.valid() )
    body->rasterChanged ( rl.get() );

  // Show we always modify?
  this->modified ( true );
}


#if USE_DISTRIBUTED == 1
///////////////////////////////////////////////////////////////////////////////
//
//  Set the connection.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::connection ( Minerva::DataSources::PG::Connection* connection )
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

Minerva::DataSources::PG::Connection* MinervaDocument::connection ()
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
  USUL_TRACE_SCOPE;
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
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Animate.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::_animate ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
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
      const double time ( fs.valid () ? fs->frameStamp()->getReferenceTime () : 0.0 );

      // Set the last time if it hasn't been set.
      if ( -1.0 == _lastTime )
        _lastTime = time;

      // Duration between last time the date was incremented.
      const double duration ( time - _lastTime );

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
        _hud.dateFeedback ( lastDate.toString() );
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
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  for ( Bodies::iterator iter = _bodies.begin(); iter != _bodies.end(); ++iter )
  {
    Body::RefPtr body ( *iter );
    if ( body.valid() )
      body->accept ( visitor );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add to the menu.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::menuAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown * caller )
{
  USUL_TRACE_SCOPE;
  
  // Typedefs to help shorten lines.
  typedef MenuKit::ToggleButton ToggleButton;
  typedef MenuKit::Button       Button;
  typedef MenuKit::RadioButton  RadioButton;
  
  // Namespace aliases to help shorten lines.
  namespace UA = Usul::Adaptors;
  namespace UC = Usul::Commands;
  
  MenuKit::Menu::RefPtr m ( menu.find ( "&Options", true ) );
	MenuKit::Menu::RefPtr animate ( menu.find ( "&Animate", true ) );

  if ( false == m->items().empty() )
    m->addSeparator();

  Usul::Interfaces::IUnknown::QueryPtr me ( this );

  animate->append ( new Button       ( new Minerva::Core::Commands::StartAnimation ( me ) ) );
  animate->append ( new Button       ( new Minerva::Core::Commands::StopAnimation  ( me ) ) );
  animate->append ( new Button       ( new Minerva::Core::Commands::PauseAnimation ( me ) ) );
  animate->append ( new ToggleButton ( new Minerva::Core::Commands::ShowPastEvents ( me ) ) );

  MenuKit::Menu::RefPtr speed ( new MenuKit::Menu ( "Speed" ) );
  speed->append ( new RadioButton ( new Minerva::Core::Commands::AnimationSpeed ( 0.1, me ) ) );
  speed->append ( new RadioButton ( new Minerva::Core::Commands::AnimationSpeed ( 0.5, me ) ) );
  speed->append ( new RadioButton ( new Minerva::Core::Commands::AnimationSpeed ( 1.0, me ) ) );
  speed->append ( new RadioButton ( new Minerva::Core::Commands::AnimationSpeed ( 2.0, me ) ) );
  animate->append ( speed );

  MenuKit::Menu::RefPtr type ( new MenuKit::Menu ( "Timestep" ) );
  type->append ( new RadioButton ( new Minerva::Core::Commands::ChangeTimestepType ( Minerva::Interfaces::IAnimationControl::DAY,   me ) ) );
  type->append ( new RadioButton ( new Minerva::Core::Commands::ChangeTimestepType ( Minerva::Interfaces::IAnimationControl::MONTH, me ) ) );
  type->append ( new RadioButton ( new Minerva::Core::Commands::ChangeTimestepType ( Minerva::Interfaces::IAnimationControl::YEAR,  me ) ) );
  animate->append ( type );

	// Time spans.
  this->_buildTimeSpanMenu();
  animate->append ( _timeSpanMenu.get() );

  // Points sub menu.
  MenuKit::Menu::RefPtr points ( new MenuKit::Menu ( "Points" ) );
  points->append ( new Button ( UC::genericCommand ( "Size * 2", UA::bind1<void> ( 2.0, UA::memberFunction<void> ( this, &MinervaDocument::_resizePoints ) ), UC::TrueFunctor() ) ) );
  points->append ( new Button ( UC::genericCommand ( "Size / 2", UA::bind1<void> ( 0.5, UA::memberFunction<void> ( this, &MinervaDocument::_resizePoints ) ), UC::TrueFunctor() ) ) ); 
  m->append ( points.get() );

  MenuKit::Menu::RefPtr split ( new MenuKit::Menu ( "Split" ) );
  split->append ( new Button ( UC::genericCommand ( "Increase Split", UA::memberFunction<void> ( this, &MinervaDocument::_increaseSplitDistance ), UC::TrueFunctor() ) ) );
  split->append ( new Button ( UC::genericCommand ( "Decrease Split", UA::memberFunction<void> ( this, &MinervaDocument::_decreaseSplitDistance ), UC::TrueFunctor() ) ) );

  m->append ( split.get() );
  
  m->append ( new ToggleButton ( UC::genericToggleCommand ( "Show Legend", 
                                                            UA::memberFunction<void> ( this, &MinervaDocument::showLegend ), 
                                                            UA::memberFunction<bool> ( this, &MinervaDocument::isShowLegend ) ) ) );
  
  m->append ( new ToggleButton ( UC::genericToggleCommand ( "Show Compass", 
                                                            UA::memberFunction<void> ( this, &MinervaDocument::showCompass ), 
                                                            UA::memberFunction<bool> ( this, &MinervaDocument::isShowCompass ) ) ) );

  m->append ( new ToggleButton ( UC::genericToggleCommand ( "Show Position", 
                                                            UA::memberFunction<void> ( this, &MinervaDocument::showLatLonText ), 
                                                            UA::memberFunction<bool> ( this, &MinervaDocument::isShowLatLonText ) ) ) );

  m->append ( new ToggleButton ( UC::genericToggleCommand ( "Show Job Feedback", 
                                                            UA::memberFunction<void> ( this, &MinervaDocument::showJobFeedback ), 
                                                            UA::memberFunction<bool> ( this, &MinervaDocument::isShowJobFeedback ) ) ) );

  m->append ( new ToggleButton ( UC::genericToggleCommand ( "Show Date Feedback", 
                                                            UA::memberFunction<void> ( this, &MinervaDocument::showDateFeedback ), 
                                                            UA::memberFunction<bool> ( this, &MinervaDocument::isShowDateFeedback ) ) ) );
  
  m->append ( new ToggleButton ( UC::genericToggleCommand ( "Show Borders", 
                                                            UA::memberFunction<void> ( this, &MinervaDocument::showBorders ), 
                                                            UA::memberFunction<bool> ( this, &MinervaDocument::isShowBorders ) ) ) );
  
  m->append ( new ToggleButton ( UC::genericToggleCommand ( "Show Skirts", 
                                                            UA::memberFunction<void> ( this, &MinervaDocument::showSkirts ), 
                                                            UA::memberFunction<bool> ( this, &MinervaDocument::isShowSkirts ) ) ) );
  
  m->append ( new ToggleButton ( UC::genericToggleCommand ( "Allow Splitting", 
                                                           UA::memberFunction<void> ( this, &MinervaDocument::allowSplit ), 
                                                           UA::memberFunction<bool> ( this, &MinervaDocument::isAllowSplit ) ) ) );

  m->append ( new ToggleButton ( UC::genericToggleCommand ( "Keep Detail", 
                                                           UA::memberFunction<void> ( this, &MinervaDocument::keepDetail ), 
                                                           UA::memberFunction<bool> ( this, &MinervaDocument::isKeepDetail ) ) ) );

	this->_buildLayerMenu();
  menu.append ( _layersMenu.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build time span menu.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::_buildTimeSpanMenu()
{
  USUL_TRACE_SCOPE;
  
  typedef MenuKit::RadioButton  RadioButton;

  // Namespace aliases to help shorten lines.
  namespace UA = Usul::Adaptors;
  namespace UC = Usul::Commands;
  
  _timeSpanMenu->clear();

  if ( _global.valid() )
    _timeSpanMenu->append ( new RadioButton ( UC::genericCheckCommand ( _global->name(), 
                                                                        UA::bind1<void> ( _global, UA::memberFunction<void> ( this, &MinervaDocument::currentTimeSpan ) ), 
                                                                        UA::bind1<bool> ( _global, UA::memberFunction<bool> ( this, &MinervaDocument::isCurrentTimeSpan ) ) ) ) );

  for ( TimeSpans::iterator iter = _timeSpans.begin(); iter != _timeSpans.end(); ++iter )
  {
    _timeSpanMenu->append ( new RadioButton ( UC::genericCheckCommand ( (*iter)->name(), 
                                                                        UA::bind1<void> ( *iter, UA::memberFunction<void> ( this, &MinervaDocument::currentTimeSpan ) ), 
                                                                        UA::bind1<bool> ( *iter, UA::memberFunction<bool> ( this, &MinervaDocument::isCurrentTimeSpan ) ) ) ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the current time span.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::currentTimeSpan ( TimeSpan::RefPtr span )
{
  USUL_TRACE_SCOPE;
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
  USUL_TRACE_SCOPE;
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
  USUL_TRACE_SCOPE;
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


///////////////////////////////////////////////////////////////////////////////
//
//  Increase split distance.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::_increaseSplitDistance()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  
  for ( Bodies::iterator iter = _bodies.begin(); iter != _bodies.end(); ++iter )
  {
    Body::RefPtr body ( *iter );
    if ( body.valid() )
      body->splitDistance ( body->splitDistance() * 1.1 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Decrease split distance.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::_decreaseSplitDistance()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  
  for ( Bodies::iterator iter = _bodies.begin(); iter != _bodies.end(); ++iter )
  {
    Body::RefPtr body ( *iter );
    if ( body.valid() )
      body->splitDistance ( body->splitDistance() / 1.1 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert matrix to heading, pitch, roll.
//  See:
//  http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToEuler/
//  http://www.euclideanspace.com/maths/geometry/rotations/conversions/eulerToMatrix/index.htm
//
//  Implementation adapted from ossimPlanet (www.ossim.org).
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  void matrixToHpr( osg::Vec3d& hpr, const osg::Matrixd& rotation )
  {
    // Initialize answer.
    hpr.set ( 0.0, 0.0, 0.0 );
    
    osg::Matrixd mat;
    
    // I'm not really sure what the following code inside {} is about.
    {
      osg::Vec3d col1 ( rotation( 0, 0 ), rotation( 0, 1 ), rotation( 0, 2 ) );
      const double s ( col1.length() );
      
      if ( s <= 0.00001 )
      {
        hpr.set(0.0f, 0.0f, 0.0f);
        return;
      }
      
      const double oneOverS ( 1.0f / s );
      for( int i = 0; i < 3; i++ )
        for( int j = 0; j < 3; j++ )
          mat(i, j) = rotation(i, j) * oneOverS;
    }
    
    // Set the pitch
    hpr[1] = ::asin ( Usul::Math::clamp ( mat ( 1, 2 ), -1.0, 1.0 ) );
    
    double cp ( ::cos( hpr[1] ) );
    
    // See if the cosine of the pitch is close to zero.
    // This is for singularities at the north and south poles.
    if ( cp > -0.00001 && cp < 0.00001 )
    {
      const double cr ( Usul::Math::clamp (  mat( 0, 1 ), -1.0, 1.0 ) );
      const double sr ( Usul::Math::clamp ( -mat( 2, 1 ), -1.0, 1.0 ) );
      
      hpr[0] = 0.0f;
      hpr[2] = ::atan2 ( sr, cr );
    }
    else
    {
      // Remove the cosine of pitch from these elements.
      cp = 1.0 / cp ;
      double sr ( Usul::Math::clamp ( ( -mat( 0, 2 ) * cp ), -1.0, 1.0 ) );
      double cr ( Usul::Math::clamp ( (  mat( 2, 2 ) * cp ), -1.0, 1.0 ) );
      double sh ( Usul::Math::clamp ( ( -mat( 1, 0 ) * cp ), -1.0, 1.0 ) );
      double ch ( Usul::Math::clamp ( (  mat( 1, 1 ) * cp ), -1.0, 1.0 ) );
      
      if ( ( sh == 0.0f && ch == 0.0f ) || ( sr == 0.0f && cr == 0.0f ) )
      {
        cr = Usul::Math::clamp (  mat( 0, 1 ), -1.0, 1.0 );
        sr = Usul::Math::clamp ( -mat( 2, 1 ), -1.0, 1.0 );
        
        hpr[0] = 0.0f;
      }
      else
      {
        hpr[0] = ::atan2 ( sh, ch );
      }
      
      hpr[2] = ::atan2 ( sr, cr );
    }
    
    // Convert to degress.
    hpr[0] *= Usul::Math::RAD_TO_DEG;
    hpr[1] *= Usul::Math::RAD_TO_DEG;
    hpr[2] *= Usul::Math::RAD_TO_DEG;
    
    // Sign flip.
    hpr[0] *= -1.0;
  }  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::_buildScene ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  
  // Clear what we have.
  _root->removeChild( 0, _root->getNumChildren() );
  
  bool viewportChanged ( false );
  bool buildLegend ( false );
  
  Usul::Interfaces::IViewport::QueryPtr vp ( caller );
  if ( vp.valid () )
  {
    const unsigned int width  ( static_cast < unsigned int > ( vp->width() ) );
    const unsigned int height ( static_cast < unsigned int > ( vp->height() ) );
    
    if ( width > 0 && height > 0 && ( width != _width || height != _height ) )
    {
      _width = width;
      _height = height;
      viewportChanged = true;
      
      // Set the build legend flag.
      buildLegend = true;
    }
  }
  
  if ( viewportChanged )
  {
    _camera->setViewport ( 0, 0, _width, _height );
    _camera->setProjectionMatrixAsOrtho ( 0, _width, 0, _height, -100.0, 100.0 );
    
    // Set the build legend flag.
    buildLegend = true;
  }
  
  if ( buildLegend || this->dirty() )
  {
    _camera->removeChild ( 0, _camera->getNumChildren() );
    
    // Build the legend.
    this->_buildLegend( caller );
  }
  
  _root->addChild ( _camera.get() );

  if ( _callback.valid() )
  {
    osg::Vec3d hpr ( _callback->_hpr );
    _hud.hpr (  hpr[0], hpr[1], hpr[2] );
  }
  
  Usul::Jobs::Manager *manager ( this->_getJobManager() );
  const unsigned int queued    ( ( 0x0 == manager ) ? 0 : manager->numJobsQueued() );
  const unsigned int executing ( ( 0x0 == manager ) ? 0 : manager->numJobsExecuting() );
  Usul::Jobs::Manager::Strings names;
  if ( 0x0 != manager )
    manager->executingNames ( names );
  
  _hud.requests ( queued );
  _hud.running ( names );
  
  if ( vp.valid() )
    _hud.updateScene( static_cast<unsigned int> ( vp->width() ), static_cast<unsigned int> ( vp->height() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the planet.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::_makePlanet()
{
  // Only make it once.
  {
    Guard guard ( this );
    if ( false == _bodies.empty() )
      return;
  }
  
  // Local typedefs to shorten the lines.
  typedef Body::Extents Extents;
  
  // Make the land model.
  typedef Minerva::Core::TileEngine::LandModelEllipsoid Land;
  Land::Vec2d radii ( osg::WGS_84_RADIUS_EQUATOR, osg::WGS_84_RADIUS_POLAR );
  Land::RefPtr land ( new Land ( radii ) );
  
  // Make a good split distance.
  const double splitDistance ( land->size() * 2.5 );
  
  // Size of the mesh.
  Body::MeshSize meshSize ( 17, 17 );
  
  // Add the body.
  Body::RefPtr body ( new Body ( land, this->_getJobManager(), meshSize, splitDistance ) );
  body->useSkirts ( true );
  
  // Add tiles to the body.
  body->addTile ( Extents ( -180, -90,    0,   90 ) );
  body->addTile ( Extents (    0, -90,  180,   90 ) );

  {
    Guard guard ( this );
    _bodies.push_back ( body.get() );
  }

  this->activeBody ( body.get() );

  // Set the log.
  this->_setLog();
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

    // Add items to the legend.
    Minerva::Core::Visitors::BuildLegend::RefPtr visitor ( new Minerva::Core::Visitors::BuildLegend ( _legend.get() ) );
    this->accept( *visitor );
    
    // Must be called after rows are added to the legend.
    this->_setLegendPosition( legendWidth );
  }

  // Build the legend.
  _camera->addChild( _legend->buildScene() );
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


///////////////////////////////////////////////////////////////////////////////
//
//  Build a layer menu.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  void buildLayerMenu ( MenuKit::Menu& menu, Usul::Interfaces::ITreeNode* node )
  {
    // Return now if layer is null.
    if ( 0x0 == node )
      return;
    
    Usul::Interfaces::ILayer::QueryPtr layer ( node );
    
    // Add the toggle.
    if ( layer.valid() )
      menu.append ( new MenuKit::ToggleButton ( new Minerva::Core::Commands::ToggleShown ( layer ) ) );
    
    {
      MenuKit::Menu::RefPtr layerMenu ( new MenuKit::Menu ( node->getTreeNodeName() ) );
      
      const unsigned int number ( node->getNumChildNodes() );
      for ( unsigned int i = 0; i < number; ++i  )
      {
        Usul::Interfaces::ITreeNode::QueryPtr child ( node->getChildNode ( i ) );
        if ( child.valid() )
          Detail::buildLayerMenu ( *layerMenu, child.get() );
        
        Usul::Interfaces::IMenuAdd::QueryPtr ma ( node->getChildNode ( i ) );
        if ( ma.valid() )
          ma->menuAdd ( *layerMenu );
      }
      menu.append ( layerMenu.get() );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the layer menu.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::_buildLayerMenu()
{
  Guard guard ( this );
  _layersMenu->clear();
  
  for ( Bodies::iterator iter = _bodies.begin(); iter != _bodies.end(); ++iter )
  {
    Body::RefPtr body ( *iter );
    if ( body.valid() )
      Detail::buildLayerMenu ( *_layersMenu, Usul::Interfaces::ITreeNode::QueryPtr ( body ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Resize all points.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::_resizePoints ( double factor )
{
  Minerva::Core::Visitors::ResizePoints::RefPtr visitor ( new Minerva::Core::Visitors::ResizePoints ( factor ) );
  this->accept ( *visitor );
  //this->dirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify the observer of the intersection (IIntersectListener).
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::intersectNotify ( float x, float y, const osgUtil::Hit &hit, Usul::Interfaces::IUnknown *caller )
{
  Body::RefPtr body ( this->activeBody() );
  
  // Set the intersection point.
  if ( body.valid() )
  {
    osg::Vec3 world ( hit.getWorldIntersectPoint() );

    Usul::Math::Vec3d point ( world[0], world[1], world[2] );
    Usul::Math::Vec3d latLonPoint;
    body->convertFromPlanet( point, latLonPoint );
    _hud.position( latLonPoint[1], latLonPoint[0], latLonPoint[2] );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set show compass state.
//  
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::showCompass( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _hud.showCompass ( b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get show compass state.
//  
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::isShowCompass() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _hud.showCompass();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set use skirts state.
//  
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::isShowSkirts() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  Body::RefPtr body ( this->activeBody() );
  return ( body.valid() ? body->useSkirts() : false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Toggle the skirts on and off.
//  
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::showSkirts ( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  Body::RefPtr body ( this->activeBody() );
  if ( body.valid() )
    body->useSkirts ( b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set use borders state.
//  
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::isShowBorders() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  Body::RefPtr body ( this->activeBody() );
  return ( body.valid() ? body->useBorders() : false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Toggle the borders on and off.
//  
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::showBorders ( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  Body::RefPtr body ( this->activeBody() );
  if ( body.valid() )
    body->useBorders ( b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the active body.
//  
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::activeBody ( Body* body )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _activeBody = body;
  
  if ( _activeBody.valid () )
  {
    _callback = new Callback;
    _callback->_body = _activeBody;
    _activeBody->scene()->setCullCallback ( _callback.get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the active body.
//  
///////////////////////////////////////////////////////////////////////////////

MinervaDocument::Body* MinervaDocument::activeBody() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _activeBody.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the job manager.
//  
///////////////////////////////////////////////////////////////////////////////

Usul::Jobs::Manager * MinervaDocument::_getJobManager()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  
  // Only make it once.
  if ( 0x0 == _manager )
  {
    typedef Usul::Registry::Database Reg;
    namespace Sections = Usul::Registry::Sections;

    const std::string type ( Reg::instance().convertToTag ( this->typeName() ) );
    Usul::Registry::Node &node ( Reg::instance()[Sections::DOCUMENT_SETTINGS][type]["job_manager_thread_pool_size"] );
    const unsigned int poolSize ( node.get<unsigned int> ( 5, true ) );

    _manager = new Usul::Jobs::Manager ( poolSize, true );
    _manager->log ( Usul::Jobs::Manager::instance().log() );
    _manager->addJobFinishedListener ( Usul::Interfaces::IUnknown::QueryPtr ( this ) );
  }
  
  return _manager;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get data from cull visitor.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::Callback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{ 
  if ( osg::NodeVisitor::CULL_VISITOR ==  nv->getVisitorType() )
  {
    osgUtil::CullVisitor* cullVisitor = dynamic_cast<osgUtil::CullVisitor*>( nv );
    if( cullVisitor )
    {
      // Set the eye position.
      _eye = cullVisitor->getEyePoint();
      
      if ( 0x0 != _body )
      {
        // Convert the eye to lat,lon, height.
        Usul::Math::Vec3d point ( _eye[0], _eye[1], _eye[2] );
        Usul::Math::Vec3d latLonPoint;
        _body->convertFromPlanet( point, latLonPoint );
        
        // Get the model view matrix from the cull visitor.
        osg::ref_ptr<osg::RefMatrix> m ( cullVisitor->getModelViewMatrix() );
        
        // Get the inverse of the view matrix.
        osg::Matrixd viewMatrix ( 0x0 != m.get() ? osg::Matrixd::inverse ( *m ) : osg::Matrixd() );
        
        // Get the matrix to point north at the eye position.
        osg::Matrixd localLsr ( _body->planetRotationMatrix( latLonPoint[1], latLonPoint[0], latLonPoint[2], 0.0 ) ); 
        
        osg::Matrixd invert;
        if ( invert.invert ( localLsr ) )
        {
          osg::Matrixf matrix ( viewMatrix * invert );
          Detail::matrixToHpr( _hpr, matrix );
        }
      }
    }
  }
  
  this->traverse( node, nv );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of children (ITreeNode).
//
///////////////////////////////////////////////////////////////////////////////

unsigned int MinervaDocument::getNumChildNodes() const
{
  USUL_TRACE_SCOPE;
  return _bodies.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the child node (ITreeNode).
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::ITreeNode * MinervaDocument::getChildNode ( unsigned int which )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return Usul::Interfaces::ITreeNode::QueryPtr ( _bodies.at ( which ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the name (ITreeNode).
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::setTreeNodeName ( const std::string & )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name (ITreeNode).
//
///////////////////////////////////////////////////////////////////////////////

std::string MinervaDocument::getTreeNodeName() const
{
  USUL_TRACE_SCOPE;
  return this->fileName();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the freeze split flag.
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::isAllowSplit() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _allowSplit;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the freeze split flag.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::allowSplit( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _allowSplit = b;
  
  for ( Bodies::iterator iter = _bodies.begin(); iter != _bodies.end(); ++iter )
  {
    Body::RefPtr body ( *iter );
    if ( body.valid() )
      body->allowSplit ( _allowSplit );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the keep detail flag.
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::isKeepDetail() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _keepDetail;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the keep detail flag.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::keepDetail( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _keepDetail = b;
  
  for ( Bodies::iterator iter = _bodies.begin(); iter != _bodies.end(); ++iter )
  {
    Body::RefPtr body ( *iter );
    if ( body.valid() )
      body->keepDetail ( _keepDetail );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  The job has finished.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::jobFinished ( Usul::Jobs::Job *job )
{
  this->requestRedraw();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when mouse event occurs.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::mouseEventNotify ( osgGA::GUIEventAdapter& ea, Usul::Interfaces::IUnknown * caller )
{
  // Query for the interface.
  Usul::Interfaces::ISceneIntersect::QueryPtr si ( caller );

  // See if it's the left button.
  const bool left ( Usul::Bits::has ( ea.getButton(), osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON ) );

  if ( left && si.valid() && osgGA::GUIEventAdapter::PUSH == ea.getEventType() )
  {
    osgUtil::Hit hit;
    if ( si->intersect ( ea.getX(), ea.getY(), hit ) )
    {
      // See if there is user data.
      osg::ref_ptr < Minerva::Core::DataObjects::UserData > userdata ( 0x0 );
      for( osg::NodePath::reverse_iterator iter = hit._nodePath.rbegin(); iter != hit._nodePath.rend(); ++iter )
      {
        if( Minerva::Core::DataObjects::UserData *ud = dynamic_cast < Minerva::Core::DataObjects::UserData *> ( (*iter)->getUserData() ) )
          userdata = ud;
      }

      if( userdata.valid() && 0x0 != userdata->_do )
      {
        Minerva::Core::DataObjects::DataObject::RefPtr dataObject ( userdata->_do );
        dataObject->clicked();
      }
    }
  }
  else if ( osgGA::GUIEventAdapter::MOVE == ea.getEventType() )
    this->requestRedraw();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the lat lon text shown?
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::isShowLatLonText() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _hud.showPointerPosition();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Toggle showing of lat lon text.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::showLatLonText ( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _hud.showPointerPosition( b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the job feedback text shown?
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::isShowJobFeedback() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _hud.showJobFeedback();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Toggle showing of job feedback text.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::showJobFeedback ( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _hud.showJobFeedback( b );
}



///////////////////////////////////////////////////////////////////////////////
//
//  Is the date feedback text shown?
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::isShowDateFeedback() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _hud.showDateFeedback();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Toggle showing of date feedback text.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::showDateFeedback ( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _hud.showDateFeedback( b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reset all the logs.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::_setLog()
{
  // Get the log.
  LogPtr logFile ( Usul::Threads::Safe::get ( this->mutex(), _log ) );

  // Loop through the bodies.
  for ( Bodies::iterator iter = _bodies.begin(); iter != _bodies.end(); ++iter )
  {
    Body::RefPtr body ( *iter );
    if ( body.valid() )
    {
      // Set the log.
      body->logSet ( logFile );
    }
  }
}
