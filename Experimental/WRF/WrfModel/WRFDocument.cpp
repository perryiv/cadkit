
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Experimental/WRF/WrfModel/WRFDocument.h"
#include "Experimental/WRF/WrfModel/ChannelCommand.h"
#include "Experimental/WRF/WrfModel/NextTimestep.h"
#include "Experimental/WRF/WrfModel/PreviousTimestep.h"
#include "Experimental/WRF/WrfModel/ChangeNumPlanes.h"
#include "Experimental/WRF/WrfModel/AnimateCommand.h"
#include "Experimental/WRF/WrfModel/LoadDataJob.h"

#include "Usul/File/Path.h"
#include "Usul/Strings/Case.h"
#include "Usul/Strings/Convert.h"
#include "Usul/Predicates/CloseFloat.h"
#include "Usul/Functions/Color.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Trace/Trace.h"
#include "Usul/Interfaces/GUI/IStatusBar.h"
#include "Usul/Interfaces/GUI/IProgressBarFactory.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/IPlanetNode.h"
#include "Usul/Interfaces/IPlanetCoordinates.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Math/Angle.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Components/Manager.h"
#include "Usul/Factory/RegisterCreator.h"

#include "OsgTools/Font.h"
#include "OsgTools/GlassBoundingBox.h"
#include "OsgTools/DisplayLists.h"
#include "OsgTools/Builders/Arrow.h"
#include "OsgTools/Volume/TransferFunction1D.h"

#include "Serialize/XML/Deserialize.h"

#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"

#include "osgText/Text"
#include "osg/Geode"
#include "osg/Geometry"
#include "osg/Material"
#include "osg/LightModel"
#include "osg/Texture2D"

#include "osgDB/ReadFile"

#include <limits>

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( WRFDocument, WRFDocument::BaseClass );
USUL_FACTORY_REGISTER_CREATOR ( WRFDocument );

SERIALIZE_XML_DECLARE_VECTOR_4_WRAPPER ( osg::Vec3 );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

WRFDocument::WRFDocument() : 
  BaseClass ( "WRF Document" ),
  _parser (),
  _filename ( "" ),
  _currentTimestep ( 0 ),
  _currentChannel ( 0 ),
  _timesteps ( 0 ),
  _channels ( 0 ),
  _x ( 0 ),
  _y ( 0 ),
  _z ( 0 ),
  _num2DFields ( 0 ),
  _numPlanes ( 256 ),
  _channelInfo (),
  _root ( new osg::MatrixTransform ),
  _planet ( new osg::Group ),
  _volumeTransform ( new osg::MatrixTransform ),
  _volumeNode ( new OsgTools::Volume::Texture3DVolume ),
  _topography ( 0x0 ),
  _bb (),
  _dirty ( true ),
  _requests (),
  _jobForScene ( 0x0 ),
  _animating ( false ),
  _offset (),
  _textureFile ( "" ),
  _vectorCache (),
  _cellSize ( 1000.0, 1000.0, 300.0 ),
  _cellScale ( 0.001, 0.001, 0.001 ),
  _maxCacheSize ( 50 ),
  _cacheRawData ( false ),
  _volumeCache ( ),
  _dataCache ( ),
  _headers ( true ),
  _lowerLeft ( 0.0, 0.0 ),
  _upperRight ( 0.0, 0.0 ),
  _usePlanet ( true ),
  _transferFunctions (),
  SERIALIZE_XML_INITIALIZER_LIST
{
  this->_addMember ( "filename", _filename );
  this->_addMember ( "num_timesteps", _timesteps );
  this->_addMember ( "num_channels", _channels );
  this->_addMember ( "x", _x );
  this->_addMember ( "y", _y );
  this->_addMember ( "z", _z );
  this->_addMember ( "num_2D_fields", _num2DFields );
  this->_addMember ( "texture_file", _textureFile );
  this->_addMember ( "channels", _channelInfo );
  this->_addMember ( "headers", _headers );
  this->_addMember ( "cache_size", _maxCacheSize );
  this->_addMember ( "starting_timestep",  _currentTimestep );
  this->_addMember ( "starting_channel",  _currentChannel );
  this->_addMember ( "lower_left", _lowerLeft );
  this->_addMember ( "upper_right", _upperRight );
  this->_addMember ( "use_planet", _usePlanet );
  this->_addMember ( "cell_size", _cellSize );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

WRFDocument::~WRFDocument()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *WRFDocument::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IBuildScene::IID:
    return static_cast < Usul::Interfaces::IBuildScene* > ( this );
  case Usul::Interfaces::ITimestepAnimation::IID:
    return static_cast < Usul::Interfaces::ITimestepAnimation * > ( this );
  case Usul::Interfaces::ITimeVaryingData::IID:
    return static_cast < Usul::Interfaces::ITimeVaryingData * > ( this );
  case Usul::Interfaces::IUpdateListener::IID:
    return static_cast < Usul::Interfaces::IUpdateListener * > ( this );
  case Usul::Interfaces::ICommandList::IID:
    return static_cast < Usul::Interfaces::ICommandList * > ( this );
  case Usul::Interfaces::IMenuAdd::IID:
    return static_cast < Usul::Interfaces::IMenuAdd * > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document exports given extension
//
///////////////////////////////////////////////////////////////////////////////

bool WRFDocument::canExport ( const std::string &file ) const
{
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document inserts given extension
//
///////////////////////////////////////////////////////////////////////////////

bool WRFDocument::canInsert ( const std::string &file ) const
{
  return this->canOpen ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document reads given extension
//
///////////////////////////////////////////////////////////////////////////////

bool WRFDocument::canOpen ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "wrf" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document writes given extension
//
///////////////////////////////////////////////////////////////////////////////

bool WRFDocument::canSave ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "wrf" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the document to given file name.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::write ( const std::string &name, Unknown *caller  ) const
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the document.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::read ( const std::string &name, Unknown *caller, Unknown *progress )
{
  Serialize::XML::deserialize ( name, *this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the document.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::clear ( Usul::Interfaces::IUnknown *caller )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can save.
//
///////////////////////////////////////////////////////////////////////////////

WRFDocument::Filters WRFDocument::filtersSave() const
{
  Filters filters;
  filters.push_back ( Filter ( "WRF xml (*.wrf)", "*.wrf" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can export.
//
///////////////////////////////////////////////////////////////////////////////

WRFDocument::Filters WRFDocument::filtersExport() const
{
  Filters filters;
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can open.
//
///////////////////////////////////////////////////////////////////////////////

WRFDocument::Filters WRFDocument::filtersOpen() const
{
  Filters filters;
  filters.push_back ( Filter ( "WRF xml (*.wrf)", "*.wrf" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can insert.
//
///////////////////////////////////////////////////////////////////////////////

WRFDocument::Filters WRFDocument::filtersInsert() const
{
  return this->filtersOpen();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *WRFDocument::buildScene ( const BaseClass::Options &options, Unknown *caller )
{
  try
  {
    this->_buildScene();

    return _root.get();
  }
  catch ( const std::exception& e )
  {
    std::cout << "Error 2642125610: Standard exceptiong caught: " << e.what() << std::endl;
  }
  catch ( ... )
  {
    std::cout << "Error 5186009000: Unknown exceptiong caught: " << std::endl;
  }

  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::_buildScene ( )
{
  USUL_TRACE_SCOPE;

  // Return now if we don't have any data.
  if ( 0 == _timesteps || 0 == _channels )
    return;

  // Remove what we have.
  {
    Guard guard ( this->mutex() );
    _root->removeChild ( 0, _root->getNumChildren() );
    _volumeTransform->removeChild ( 0, _volumeTransform->getNumChildren () );
    _root->addChild ( _volumeTransform.get () );

    // Add the planet.
    if ( _usePlanet )
      _root->addChild ( _planet.get() );

    // Add the topography back.
    else
    {      
      _root->addChild ( _topography.get() );
    }
  }

  // If we don't have the data already...
  if ( false == this->_dataCached ( _currentTimestep, _currentChannel ) )
  {
    if ( true == this->_dataRequested ( _currentTimestep, _currentChannel ) )
    {
      // Keep a handle the job to wait for.
      Guard guard ( this->mutex() );
      _jobForScene = _requests [ Request ( _currentTimestep, _currentChannel ) ];
    }

    // Launch a job to read the data.
    else
    {
      this->_requestData ( _currentTimestep, _currentChannel, true );
    }

    // Build proxy geometry.
    {
      Guard guard ( this->mutex() );
      _volumeTransform->addChild ( this->_buildProxyGeometry() );
    }
  }
  
  // We have the data ...
  else
  {
    // We no longer need to wait for any job.
    {
      Guard guard ( this->mutex() );
      _jobForScene = 0x0;
    }

    // Get the 3D image for the volume.
    ImagePtr image ( this->_volume ( _currentTimestep, _currentChannel ) );

    // Make a bounding box around the volume.
    //OsgTools::GlassBoundingBox gbb ( _bb );
    //gbb ( _volumeTransform.get(), true, false, false );

    // Add the volume to the scene.
    {
      Guard guard ( this->mutex() );
      _volumeTransform->addChild ( this->_buildVolume ( image.get() ) );
      //_volumeTransform->addChild ( this->_buildVectorField ( _currentTimestep, 0, 1 ) );
    }
  }

  // No longer dirty
  this->dirty ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the requested data cached?
//
///////////////////////////////////////////////////////////////////////////////

bool WRFDocument::_dataCached ( unsigned int timestep, unsigned int channel )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _volumeCache.end() != _volumeCache.find ( Request ( timestep, channel ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Has the data been requested?
//
///////////////////////////////////////////////////////////////////////////////

bool WRFDocument::_dataRequested ( unsigned int timestep, unsigned int channel )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _requests.end() != _requests.find ( Request ( timestep, channel ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add volume to the cache.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::addVolume ( const Parser::Data& data, osg::Image* image, unsigned int timestep, unsigned int channel )
{
  USUL_TRACE_SCOPE;

  // For convienence.
  Request request ( timestep, channel );

  // Guard the rest of the function...
  Guard guard ( this->mutex() );

  // Cache the volume.
  _volumeCache [ request ] = image;

  // Cache the raw data if we a suppose to.
  if ( _cacheRawData )
    _dataCache [ request ] = data;

  // Erase the request from the list of jobs that are running.
  _requests.erase ( request );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the volume at the timestep and channel.
//
///////////////////////////////////////////////////////////////////////////////

osg::Image * WRFDocument::_volume ( unsigned int timestep, unsigned int channel )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _volumeCache [ Request ( timestep, channel ) ].get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the volume.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* WRFDocument::_buildVolume( osg::Image* image )
{
  USUL_TRACE_SCOPE;

  _volumeNode->numPlanes ( _numPlanes );
  _volumeNode->image ( image );

  return _volumeNode.get ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::_initBoundingBox ()
{
  USUL_TRACE_SCOPE;

  // Create the bounding box.
  double xLength ( _x * _cellSize [ 0 ] );
  double yLength ( _y * _cellSize [ 1 ] );
  double zLength ( _z * _cellSize [ 2 ] );

  float xHalf ( static_cast < float > ( xLength ) / 2.0f );
  float yHalf ( static_cast < float > ( yLength ) / 2.0f );
  float zHalf ( static_cast < float > ( zLength ) / 2.0f );

  _bb.set ( -xHalf, -yHalf, -zHalf, xHalf, yHalf, zHalf );

  // Set the volume nodes bounding box.
  _volumeNode->boundingBox ( _bb );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the proxy geometry.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node * WRFDocument::_buildProxyGeometry ()
{
  USUL_TRACE_SCOPE;

  osg::ref_ptr < osg::Group > group ( new osg::Group );
  OsgTools::GlassBoundingBox gbb ( _bb );
  gbb ( group.get(), true, false, false );

  osg::ref_ptr< osgText::Text > text ( new osgText::Text );
  text->setFont( OsgTools::Font::defaultFont() );
  text->setColor( osg::Vec4 ( 1.0, 1.0, 1.0, 1.0 ) );
  text->setCharacterSize( 50 );
  text->setCharacterSizeMode ( osgText::Text::SCREEN_COORDS );
  text->setPosition ( _bb.center() );
  text->setText( "Loading..." );
  text->setAutoRotateToScreen( true );
  text->setUseDisplayList ( false );
  text->setFontResolution( 40, 40 );

  osg::ref_ptr < osg::Geode > geode ( new osg::Geode );
  geode->addDrawable ( text.get () );

  group->addChild ( geode.get() );

  return group.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start timestep animation.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::startTimestepAnimation ()
{
  USUL_TRACE_SCOPE;
  {
    Guard guard ( this->mutex() );
    _animating = true;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Stop timestep animation.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::stopTimestepAnimation ()
{
  USUL_TRACE_SCOPE;
  {
    Guard guard ( this->mutex() );
    _animating = false;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the current timestep.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::setCurrentTimeStep ( unsigned int current )
{
  USUL_TRACE_SCOPE;
  {
    Guard guard ( this->mutex() );

    _currentTimestep = current;
    if ( _currentTimestep >= _timesteps )
      _currentTimestep = 0;
  }

  // Purge the cache.
  this->_purgeCache ();

  this->dirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the current timestep.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int WRFDocument::getCurrentTimeStep () const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _currentTimestep;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of timesteps.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int WRFDocument::getNumberOfTimeSteps () const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _timesteps;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::updateNotify ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;

  // Update the cache.
  this->_updateCache ();

  // Only animate if we aren't waiting for data.
  //if ( false == _jobForScene.valid () )
  {
    static unsigned int num ( 0 );

    ++num;
    if ( this->animating() && num % 2 == 0 )
    {
      unsigned int currentTimestep ( this->getCurrentTimeStep () );
      this->setCurrentTimeStep ( ++currentTimestep );

      this->dirty ( true );
    }
  }

  // Buid the scene if we need to.
  if ( this->dirty () )
    this->_buildScene();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Purge the cache.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::_purgeCache ()
{
  if ( this->_cacheFull () )
  {
    // Erase the first element in the cache.
    Guard guard ( this->mutex () );

    Request r ( _volumeCache.begin()->first );
    unsigned int timestep ( r.first );

    VolumeCache::iterator iter = _volumeCache.upper_bound ( Request ( timestep, _channels ) );
    _volumeCache.erase ( _volumeCache.begin(), iter );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of items in the cache.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int WRFDocument::cacheSize () const
{
  Guard guard ( this->mutex () );
  return _volumeCache.size ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the maximium number of items in the cache.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int WRFDocument::maxCacheSize () const
{
  Guard guard ( this->mutex () );
  return _maxCacheSize;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the cache full?
//
///////////////////////////////////////////////////////////////////////////////

bool WRFDocument::_cacheFull () const
{
  return this->cacheSize () > this->maxCacheSize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find what timestep and channel to load next and launch job.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::_launchNextCacheRequest ()
{
  // Return now if we don't have any data.
  if ( 0 == _timesteps || 0 == _channels )
    return;

  unsigned int timestepToLoad ( 0 );

  if ( false == _volumeCache.empty () )
  {
    Request last ( ( --_volumeCache.end () )->first );
    timestepToLoad = last.first + 1;
  }

  if ( timestepToLoad > _timesteps )
    timestepToLoad = 0;

  typedef LoadDataJob::ReadRequest ReadRequest;
  typedef LoadDataJob::ReadRequests ReadRequests;

  ReadRequests requests;

  // If we are animation only request data for the current channel.
  //if ( this->animating () )
  {
    // Get the current channel
    unsigned int currentChannel ( this->currentChannel () );

    // Only make the request if we don't already have it.
    if ( false == this->_dataCached ( timestepToLoad, currentChannel ) )
      this->_requestData ( timestepToLoad, currentChannel, false );
  }
#if 0
  else
  {
    for ( unsigned int i = 0; i < _channels; ++ i )
    {
      // Only make a request if we don't alreay have the data.
      if ( false == this->_dataCached ( timestepToLoad, i ) )
      {
        Channel::RefPtr info ( _channelInfo.at ( i ) );
        ReadRequest request ( timestepToLoad, info );
        requests.push_back ( request );
      }
    }

    LoadDataJob::RefPtr job ( new LoadDataJob ( requests, this, _parser ) );
    job->setSize ( _x, _y, _z );

    for ( ReadRequests::const_iterator iter = requests.begin(); iter != requests.end(); ++iter )
    {
      Guard guard ( this->mutex() );
      _requests.insert ( Requests::value_type ( Request ( iter->first, iter->second->index() ), job.get() ) );
    }

    Usul::Jobs::Manager::instance().add ( job.get() );
  }
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the cached volume data.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::_updateCache ()
{
  // Return now if there are still requests pending...
  if ( false == _requests.empty() )
    return;

  // Don't make any more requests if the cache is full and we are not animating.
  if ( this->_cacheFull () && false == this->animating () )
    return;
  
  // Make the next request.
  this->_launchNextCacheRequest ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get a list of avialable commands.
//
///////////////////////////////////////////////////////////////////////////////

WRFDocument::CommandList WRFDocument::getCommandList ()
{
  USUL_TRACE_SCOPE;

  Usul::Interfaces::IUnknown::QueryPtr me ( this );

  CommandList commands;

  commands.push_back ( new NextTimestep ( this ) );
  commands.push_back ( new PreviousTimestep ( this ) );

  for ( ChannelInfos::iterator iter = _channelInfo.begin(); iter != _channelInfo.end(); ++ iter )
  {
    commands.push_back ( new ChannelCommand ( (*iter)->name (), (*iter)->index (), this ) );
  }

  commands.push_back ( new ChangeNumPlanes ( 4.0, this ) );
  commands.push_back ( new ChangeNumPlanes ( 2.0, this ) );
  commands.push_back ( new ChangeNumPlanes ( 0.5, this ) );
  commands.push_back ( new ChangeNumPlanes ( 0.25, this ) );

  commands.push_back ( new AnimateCommand ( true, me.get() ) );
  commands.push_back ( new AnimateCommand ( false, me.get() ) );

  return commands;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add to the menu.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::menuAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown * caller )
{
  Usul::Interfaces::IUnknown::QueryPtr me ( this );

  // Make the menu.
  MenuKit::Menu::RefPtr wrf ( new MenuKit::Menu ( "WRF" ) );
  
  wrf->append ( new MenuKit::Button ( new NextTimestep ( this ) ) );
  wrf->append ( new MenuKit::Button ( new PreviousTimestep ( this ) ) );

  for ( ChannelInfos::iterator iter = _channelInfo.begin(); iter != _channelInfo.end(); ++ iter )
  {
    wrf->append ( new MenuKit::Button ( new ChannelCommand ( (*iter)->name (), (*iter)->index (), this ) ) );
  }

  wrf->append ( new MenuKit::Button ( new ChangeNumPlanes ( 4.0, this ) ) );
  wrf->append ( new MenuKit::Button ( new ChangeNumPlanes ( 2.0, this ) ) );
  wrf->append ( new MenuKit::Button ( new ChangeNumPlanes ( 0.5, this ) ) );
  wrf->append ( new MenuKit::Button ( new ChangeNumPlanes ( 0.25, this ) ) );

  wrf->append ( new MenuKit::Button ( new AnimateCommand ( true, me.get() ) ) );
  wrf->append ( new MenuKit::Button ( new AnimateCommand ( false, me.get() ) ) );

  menu.append ( wrf );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the current channel.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::currentChannel ( unsigned int value )
{
  USUL_TRACE_SCOPE;
  {
    Guard guard ( this->mutex() );
    _currentChannel = value;
  }
  this->dirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the current channel.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int WRFDocument::currentChannel () const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _currentChannel;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::dirty ( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _dirty = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

bool WRFDocument::dirty () const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _dirty;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of planes.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::numPlanes ( unsigned int numPlanes )
{
  USUL_TRACE_SCOPE;

  {
    Guard guard ( this->mutex() );
    _numPlanes = numPlanes;
  }

  this->dirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of planes.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int WRFDocument::numPlanes () const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _numPlanes;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load job has finished.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::loadJobFinished ( Usul::Jobs::Job* job )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  if ( job == _jobForScene.get () )
  {
    _jobForScene = 0x0;
    this->dirty ( true );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are we animating?
//
///////////////////////////////////////////////////////////////////////////////

bool WRFDocument::animating () const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _animating;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the topography.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::_buildTopography ()
{
  USUL_TRACE_SCOPE;

  // Make a copy of the parser.
  Parser parser ( _parser );

  // Get the topograpy.
  Parser::Data data;
  parser.field2D ( data, 0 );

  const unsigned int width ( _x ), height ( _y );

  double xCellSize ( 10.0 );
  double yCellSize ( 10.0 );
  double zCellSize ( 3.0 );

  double xLength ( width  * xCellSize );
  double yLength ( height * yCellSize );
  double zLength ( _z * zCellSize );

  double xHalf ( xLength / 2.0 );
  double yHalf ( yLength / 2.0 );
  double zHalf ( zLength / 2.0 );

  osg::ref_ptr < osg::Vec3Array > vertices ( new osg::Vec3Array );
  osg::ref_ptr < osg::Vec3Array > normals ( new osg::Vec3Array );
  osg::ref_ptr < osg::Vec2Array > coords  ( new osg::Vec2Array );

  unsigned int numPoints ( height * width );
  vertices->reserve ( numPoints );
  normals->reserve  ( numPoints / 3 );
  coords->reserve   ( numPoints );

  // Loop over the rows
  for ( unsigned int i = 0; i < height - 1; ++i )
  {
    float xa ( -yHalf + ( ( i     ) * xCellSize ) );
    float xb ( -yHalf + ( ( i + 1 ) * xCellSize ) );

    // Loop over the columns.
    for ( unsigned int j = 0; j < width - 1; ++j )
    {
      float y0 ( -xHalf + ( ( j     ) * yCellSize ) );
      float y1 ( -xHalf + ( ( j + 1 ) * yCellSize ) );

      // Calculate vertices of current triangles.
      const osg::Vec3 a ( xa, y0, ( data.at ( ( ( i     ) * width ) + j     ) / 100.0 ) - zHalf );
      const osg::Vec3 b ( xa, y1, ( data.at ( ( ( i     ) * width ) + j + 1 ) / 100.0 ) - zHalf );
      const osg::Vec3 c ( xb, y0, ( data.at ( ( ( i + 1 ) * width ) + j     ) / 100.0 ) - zHalf );
      const osg::Vec3 d ( xb, y1, ( data.at ( ( ( i + 1 ) * width ) + j + 1 ) / 100.0 ) - zHalf );

      const osg::Vec2 ta ( static_cast < double > ( i     ) / height, static_cast < double > ( j     ) / width );
      const osg::Vec2 tb ( static_cast < double > ( i     ) / height, static_cast < double > ( j + 1 ) / width );
      const osg::Vec2 tc ( static_cast < double > ( i + 1 ) / height, static_cast < double > ( j     ) / width );
      const osg::Vec2 td ( static_cast < double > ( i + 1 ) / height, static_cast < double > ( j + 1 ) / width );

      // Add first triangle.
      {
        osg::Vec3 n ( ( d - c ) ^ ( a - c ) );
        n.normalize();

        vertices->push_back ( a );
        vertices->push_back ( c );
        vertices->push_back ( d );
        normals->push_back ( n );

        coords->push_back ( ta );
        coords->push_back ( tc );
        coords->push_back ( td );
      }

      // Add second triangle.
      {
        osg::Vec3 n ( ( b - d ) ^ ( a - d ) );
        n.normalize();

        vertices->push_back ( a );
        vertices->push_back ( d );
        vertices->push_back ( b );
        normals->push_back ( n );

        coords->push_back ( ta );
        coords->push_back ( td );
        coords->push_back ( tb );
      }
    }
  }

  osg::ref_ptr < osg::Geode > node ( new osg::Geode );
  osg::ref_ptr < osg::Geometry > geometry ( new osg::Geometry );
  geometry->setUseDisplayList ( false );

  geometry->setVertexArray ( vertices.get() );
  geometry->setNormalArray ( normals.get() );
  geometry->setNormalBinding ( osg::Geometry::BIND_PER_PRIMITIVE );

  if ( Usul::Predicates::FileExists::test ( _textureFile ) )
  {
    osg::ref_ptr < osg::Image > image ( osgDB::readImageFile ( _textureFile ) );

    if ( image.valid() )
    {
      osg::ref_ptr < osg::Texture2D > texture ( new osg::Texture2D );
      texture->setImage( image.get() );

      texture->setFilter( osg::Texture::MIN_FILTER, osg::Texture::NEAREST );
      texture->setFilter( osg::Texture::MAG_FILTER, osg::Texture::NEAREST );
      texture->setWrap  ( osg::Texture::WRAP_S, osg::Texture::CLAMP );
      texture->setWrap  ( osg::Texture::WRAP_T, osg::Texture::CLAMP );
      texture->setInternalFormatMode ( osg::Texture::USE_IMAGE_DATA_FORMAT );

      // Get the state set.
      osg::ref_ptr< osg::StateSet > ss ( node->getOrCreateStateSet() );
      ss->setTextureAttributeAndModes ( 0, texture.get(), osg::StateAttribute::ON );
      geometry->setTexCoordArray ( 0, coords.get() );
    }
  }

  geometry->addPrimitiveSet ( new osg::DrawArrays ( GL_TRIANGLES, 0, vertices->size() ) );

  node->addDrawable ( geometry.get() );

  osg::ref_ptr < osg::StateSet > ss ( node->getOrCreateStateSet () );
  
  // Make a light-model.
  osg::ref_ptr<osg::LightModel> lm ( new osg::LightModel );
  lm->setTwoSided( true );
 
  // Set the state. Make it override any other similar states.
  typedef osg::StateAttribute Attribute;
  ss->setAttributeAndModes ( lm.get(), Attribute::OVERRIDE | Attribute::ON );

  // Turn off back face culling
  ss->setMode ( GL_CULL_FACE, Attribute::OVERRIDE | Attribute::OFF );

  //osg::Vec4 frontDiffuse ( 20.0 / 255.0f, 100.0f / 255.0f, 140.0f / 255.0f, 1.0f );
  osg::Vec4 frontDiffuse ( 80.0 / 255.0f, 56.0f / 255.0f, 28.0f / 255.0f, 1.0f );
  osg::Vec4 backDiffuse ( 255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 1.0f );
  //osg::Vec4 ambient ( diffuse );

  osg::ref_ptr < osg::Material > material ( new osg::Material );
  material->setAmbient ( osg::Material::BACK,  backDiffuse );
  material->setAmbient ( osg::Material::FRONT, backDiffuse );

  material->setDiffuse ( osg::Material::BACK,  backDiffuse );
  material->setDiffuse ( osg::Material::FRONT, backDiffuse );

  ss->setAttribute ( material.get(), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );

  // Set the data members.
  {
    Guard guard ( this->mutex() );
    _topography = node.get();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a vector field.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node * WRFDocument::_buildVectorField ( unsigned int timestep, unsigned int channel0, unsigned int channel1 )
{
  USUL_TRACE_SCOPE;

  // Check the cache.
  {
    Guard guard ( this->mutex () );
    osg::ref_ptr < osg::Node > &node ( _vectorCache.at ( timestep ) );
    if ( node.valid () )
      return node.get();
  }

  // Check the first channel.
  if ( false == this->_dataCached ( timestep, channel0 ) )
  {
    if ( true == this->_dataRequested ( timestep, channel0 ) )
    {
      // Keep a handle the job to wait for.
      Guard guard ( this->mutex() );
      _jobForScene = _requests [ Request ( timestep, channel0 ) ];
    }

    // Launch a job to read the data.
    else
    {
      this->_requestData ( timestep, channel0, true );
    }

    return this->_buildProxyGeometry();
  }

  // Check the second channel.
  if ( false == this->_dataCached ( timestep, channel1 ) )
  {
    if ( true == this->_dataRequested ( timestep, channel1 ) )
    {
      // Keep a handle the job to wait for.
      Guard guard ( this->mutex() );
      _jobForScene = _requests [ Request ( timestep, channel1 ) ];
    }

    // Launch a job to read the data.
    else
    {
      this->_requestData ( timestep, channel1, true );
    }

    return this->_buildProxyGeometry();
  }

  // If we get here, we have the data we need.
  const unsigned int width ( _y ), height ( _x );

  double xLength ( width  * _cellSize.x() );
  double yLength ( height * _cellSize.y() );
  double zLength ( _z * _cellSize.z() );

  double xHalf ( xLength / 2.0 );
  double yHalf ( yLength / 2.0 );
  double zHalf ( zLength / 2.0 );

  // Group to return.
  osg::ref_ptr < osg::Group > group ( new osg::Group );
#if 0
  Usul::Predicates::CloseFloat < float > close ( 10 );

  double min ( Usul::Math::minimum ( _channelInfo.at ( channel0 )->min (), _channelInfo.at ( channel1 )->min () ) );
  double max ( Usul::Math::maximum ( _channelInfo.at ( channel0 )->max (), _channelInfo.at ( channel1 )->max () ) );

  double magnitude ( ::sqrt ( ( min * min ) + ( max * max ) ) );

  // Loop over the rows.
  for ( unsigned int i = 0; i < height; i += 1 )
  {
    float x ( -yHalf + ( i * _cellSize.x() ) );

    // Loop over the columns.
    for ( unsigned int j = 0; j < width; j += 1 )
    {
      float y ( -xHalf + ( j * _cellSize.y() ) );

      // Loop over the depth
      for ( unsigned int k = 0; k < _z; k += 1 )
      {
        float z ( -zHalf + ( k * _cellSize.z() ) );

        float u ( this->_value ( timestep, channel0, i, j, k ) );
        float v ( this->_value ( timestep, channel1, i, j, k ) );

        if ( false == close ( u, 1.0e+035f ) && false == close ( v, 1.0e+035f ) )
        {
          osg::Vec3 vector ( u, v, 0.0 );

          osg::Vec3 translate ( x, y, z );
          translate += ( _cellSize / 2.0 );

          double value ( vector.length() / magnitude );
          //vector.normalize();

          osg::ref_ptr < osg::MatrixTransform > mt ( new osg::MatrixTransform );
          mt->setMatrix ( osg::Matrix::translate ( translate ) );
#if 0
          OsgTools::Builders::Arrow arrow;
          arrow.start ( osg::Vec3 ( 0.0, 0.0, 0.0 ) );
          arrow.end ( vector * 10 );
          arrow.radius ( 1.75 );
          arrow.height ( 9.0 );

          //float r ( 0.0 ), g ( 0.0 ), b ( 0.0 );
          //Usul::Functions::hsvToRgb ( r, g, b, static_cast < float > ( value * 300 ), 1.0f, 1.0f );
          //arrow.color ( osg::Vec4 ( r, g, b, 1.0 ) );
          arrow.color ( osg::Vec4 ( 1.0, 0.0, 0.0, 1.0 ) );

          arrow.tessellation ( 0.25f );
          arrow.thickness ( 3.0f );

          mt->addChild ( arrow () );
#else
          OsgTools::Ray ray;
          ray.start ( osg::Vec3 ( 0.0, 0.0, 0.0 ) );
          ray.end ( vector );

          ray.color ( osg::Vec4 ( 1.0, 0.0, 0.0, 1.0 ) );

          ray.thickness ( 3.0f );

          mt->addChild ( ray () );
#endif
          group->addChild ( mt.get() );
        }
      }
    }
  }

  // Cache.
  {
    Guard guard ( this->mutex () );
    _vectorCache.at ( timestep ) = group.get();
  }
#endif
  return group.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Request the data.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::_requestData ( unsigned int timestep, unsigned int channel, bool wait )
{
  USUL_TRACE_SCOPE;

  typedef LoadDataJob::ReadRequest ReadRequest;
  typedef LoadDataJob::ReadRequests ReadRequests;

  Channel::RefPtr info ( _channelInfo.at ( channel ) );

  ReadRequest request ( timestep, info );
  ReadRequests requests;
  requests.push_back ( request );

  LoadDataJob::RefPtr job ( new LoadDataJob ( requests, this, _parser ) );
  job->setSize ( _x, _y, _z );

  {
    Guard guard ( this->mutex() );    
    _requests.insert ( Requests::value_type ( Request ( timestep, channel ), job.get() ) );
  }

  // If we need to wait for this job...
  if ( wait )
  {
    Guard guard ( this->mutex() );
    _jobForScene = job.get();
  }

  // Add the job to the job manager.
  Usul::Jobs::Manager::instance().add ( job.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Deserialize.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::deserialize ( const XmlTree::Node &node )
{
  _dataMemberMap.deserialize ( node );

  // Set the parser's datamembers.
  _parser.numFields2D ( _num2DFields );
  _parser.filename ( _filename );
  _parser.timesteps ( _timesteps );
  _parser.channels ( _channels );
  _parser.setSizes ( _x, _y, _z );
  _parser.headers ( _headers );
  
  // Initialize the bounding box.
  this->_initBoundingBox ();

  // Build default transfer functions.
  this->_buildDefaultTransferFunctions ();

  // Build the transfer function.
  _volumeNode->transferFunction ( _transferFunctions.at ( 0 ).get() );

  // Make enough room for the cache.
  _vectorCache.resize ( _timesteps );

  if ( _usePlanet )
  {
    Usul::Interfaces::IPlanetNode::QueryPtr pn ( Usul::Components::Manager::instance ().getInterface ( Usul::Interfaces::IPlanetNode::IID  ) );

    if ( pn.valid () )
    {
      Guard guard ( this->mutex () );
      _planet = pn->planetNode ( _filename );
    }

    // Add the transform for the volume.
    //_planet->addChild ( _volumeTransform.get() );

    Usul::Interfaces::IPlanetCoordinates::QueryPtr pc ( Usul::Components::Manager::instance ().getInterface ( Usul::Interfaces::IPlanetCoordinates::IID ) );

    if ( pc.valid () )
    {
      Usul::Math::Vec2d center ( ( _lowerLeft + _upperRight ) );
      center /= 2.0;

      Usul::Math::Vec3d translate;

      double height ( (_z * _cellSize [ 2 ] ) / 2.0 );

      pc->convertToPlanetEllipsoid ( Usul::Math::Vec3d ( center [ 1 ], center [ 0 ], height ), translate );

      osg::Matrix T ( osg::Matrix::translate ( translate [ 0 ], translate [ 1 ], translate [ 2 ] ) );

      osg::Vec3 v0 ( translate [ 0 ], translate [ 1 ], translate [ 2 ] );

      osg::Vec3 up ( v0 );
      up.normalize();

      osg::Matrix R;
      R.makeRotate ( osg::Vec3( 0.0, 0.0, -1.0 ), up );

      // Calculate the up vector of the bounding box.
      osg::Vec3 bbUp ( R * osg::Vec3 ( 0.0, 0.0, -1.0 ) );
      bbUp.normalize();

      // Calculate the north vector.
      Usul::Math::Vec3d v1;
      pc->convertToPlanetEllipsoid ( Usul::Math::Vec3d ( center [ 1 ], center [ 0 ] + 0.1, height ), v1 );
      Usul::Math::Vec3d diff ( v1 - translate );
      diff.normalize();

      osg::Vec3 north ( diff[0], diff[1], diff[2] );

#if 0
      {
        OsgTools::Ray ray;
        ray.start ( v0 );
        ray.end ( v0 + ( north * 100000 ) );
        ray.color ( osg::Vec4 ( 1.0, 0.0, 0.0, 1.0 ) );
        ray.thickness ( 15.0f );

        _planet->addChild ( ray() );
      }

      {
        OsgTools::Ray ray;
        ray.start ( v0 );
        ray.end ( v0 + ( bbUp * 100000 ) );
        ray.color ( osg::Vec4 ( 0.0, 1.0, 0.0, 1.0 ) );
        ray.thickness ( 15.0f );

        _planet->addChild ( ray() );
      }

      {
        OsgTools::Ray ray;
        ray.start ( v0 );
        ray.end ( v0 + ( up * 100000 ) );
        ray.color ( osg::Vec4 ( 0.0, 0.0, 1.0, 1.0 ) );
        ray.thickness ( 15.0f );

        _planet->addChild ( ray() );
      }
#endif

      typedef Usul::Math::Angle < double, 3 > Angle;

      double angle ( Angle::get < osg::Vec3 > ( bbUp, north ) );

      osg::Matrix R1;
      R1.makeRotate ( angle, up );

      osg::Matrix m;
      m.postMult ( R );
      m.postMult ( R1 );
      m.postMult ( T );

      _volumeTransform->setMatrix ( m );
    }

  }

  // Build the topography.
  else
    Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &WRFDocument::_buildTopography ), "3345743110" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the default transfer function.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::_buildDefaultTransferFunctions ()
{
  // Typedefs.
  typedef OsgTools::Volume::TransferFunction1D TransferFunction1D;
  typedef TransferFunction1D::Colors           Colors;

  const unsigned int size ( 256 );

  Colors grayscale ( size );
  Colors hsv       ( size );

  for ( unsigned int i = 0; i < size; ++i )
  {
    float value ( static_cast < float > ( i ) / 255 );
    unsigned int alpha ( 35 );

    unsigned char c ( 120 - ( static_cast < unsigned int > ( value * 120 ) ) );
    grayscale.at ( i ) [ 0 ] = c;
    grayscale.at ( i ) [ 1 ] = c;
    grayscale.at ( i ) [ 2 ] = c;
    grayscale.at ( i ) [ 3 ] = ( i == 0 ? 0 : alpha );

    float r ( 0.0 ), g ( 0.0 ), b ( 0.0 );
    Usul::Functions::hsvToRgb ( r, g, b, 300 - ( value * 300 ), 1.0f, 1.0f );
    hsv.at ( i ) [ 0 ] = static_cast < unsigned char > ( r * 255 );
    hsv.at ( i ) [ 1 ] = static_cast < unsigned char > ( g * 255 );
    hsv.at ( i ) [ 2 ] = static_cast < unsigned char > ( b * 255 );
    hsv.at ( i ) [ 3 ] = ( i == 0 ? 0 : alpha );
  }

  _transferFunctions.push_back ( new TransferFunction1D ( grayscale ) );
  _transferFunctions.push_back ( new TransferFunction1D ( hsv ) );
}
