
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Experimental/WRF/WrfModel/WRFDocument.h"
#include "Experimental/WRF/WrfModel/LoadDataJob.h"

#include "Usul/Adaptors/Bind.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Commands/GenericCommand.h"
#include "Usul/Commands/GenericCheckCommand.h"
#include "Usul/Components/Manager.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Factory/RegisterCreator.h"
#include "Usul/File/Path.h"
#include "Usul/Functions/Color.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/GUI/IStatusBar.h"
#include "Usul/Interfaces/GUI/IProgressBarFactory.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/IPlanetCoordinates.h"
#include "Usul/Interfaces/IFrameStamp.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Math/Angle.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Network/Names.h"
#include "Usul/Predicates/CloseFloat.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Strings/Case.h"
#include "Usul/Strings/Convert.h"
#include "Usul/Strings/Format.h"
#include "Usul/Trace/Trace.h"
#include "Usul/Threads/Safe.h"

#include "OsgTools/Font.h"
#include "OsgTools/GlassBoundingBox.h"
#include "OsgTools/DisplayLists.h"
#include "OsgTools/Builders/Arrow.h"
#include "OsgTools/Volume/TransferFunction1D.h"

#include "Serialize/XML/Serialize.h"
#include "Serialize/XML/Deserialize.h"

#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"
#include "MenuKit/RadioButton.h"
#include "MenuKit/ToggleButton.h"

#include "osgText/Text"
#include "osg/Geode"
#include "osg/Geometry"
#include "osg/Material"
#include "osg/LightModel"
#include "osg/Texture2D"
#include "osg/CoordinateSystemNode"

#include "osgDB/ReadFile"

#include "osgUtil/CullVisitor"
#include "osgUtil/UpdateVisitor"

#include <limits>
#include <iterator>

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( WRFDocument, WRFDocument::BaseClass );
USUL_FACTORY_REGISTER_CREATOR ( WRFDocument );

USUL_IO_TEXT_DEFINE_READER_TYPE_VECTOR_3 ( osg::Vec3 );
USUL_IO_TEXT_DEFINE_WRITER_TYPE_VECTOR_3 ( osg::Vec3 );
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
  _volumeTransform ( new osg::MatrixTransform ),
  _volumeNode ( new Volume ),
  _bb (),
  _dirty ( true ),
  _requests (),
  _jobForScene ( 0x0 ),
  _animating ( false ),
  _cellSize ( 1000.0, 1000.0, 300.0 ),
  _cellScale ( 0.001, 0.001, 0.001 ),
  _maxCacheSize ( 50 ),
  _cacheRawData ( false ),
  _volumeCache ( ),
  _dataCache ( ),
  _headers ( true ),
  _lowerLeft ( 0.0, 0.0 ),
  _upperRight ( 0.0, 0.0 ),
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
  this->_addMember ( "channels", _channelInfo );
  this->_addMember ( "headers", _headers );
  this->_addMember ( "cache_size", _maxCacheSize );
  this->_addMember ( "starting_timestep",  _currentTimestep );
  this->_addMember ( "starting_channel",  _currentChannel );
  this->_addMember ( "lower_left", _lowerLeft );
  this->_addMember ( "upper_right", _upperRight );
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
  case Usul::Interfaces::IMenuAdd::IID:
    return static_cast < Usul::Interfaces::IMenuAdd * > ( this );
  case Usul::Interfaces::ILayer::IID:
    return static_cast < Usul::Interfaces::ILayer* > ( this );
  case Usul::Interfaces::ITreeNode::IID:
    return static_cast < Usul::Interfaces::ITreeNode* > ( this );
  case Usul::Interfaces::ISerialize::IID:
    return static_cast<Usul::Interfaces::ISerialize*> ( this );
  case Usul::Interfaces::IBusyState::IID:
    return static_cast<Usul::Interfaces::IBusyState*> ( this );
  case Usul::Interfaces::IBooleanState::IID:
    return static_cast<Usul::Interfaces::IBooleanState*> ( this );
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

void WRFDocument::write ( const std::string &name, Unknown *caller, Unknown *progress ) const
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the document.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::read ( const std::string &name, Unknown *caller, Unknown *progress )
{
  // Deserialize the xml tree.
  XmlTree::XercesLife life;
  XmlTree::Document::ValidRefPtr document ( new XmlTree::Document );
  document->load ( name );
  this->deserialize ( *document );
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
  USUL_TRACE_SCOPE;
   
  Usul::Interfaces::IPlanetCoordinates::QueryPtr pc ( caller );

  if ( pc.valid() )
  {
     Guard guard ( this->mutex() );
    
    // Lat long center of the bounding box.
    const Usul::Math::Vec2d center ( ( _lowerLeft + _upperRight ) * 0.5  );
    double height ( ( _z * _cellSize [ 2 ] ) / 2.0 );

    osg::Matrix m ( pc->planetRotationMatrix ( center [ 0 ], center [ 1 ], height, 0.0 ) );
    _volumeTransform->setMatrix ( m );
  }
  
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &WRFDocument::_buildScene ), "2642125610" );
  return _root.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::_buildScene()
{
  USUL_TRACE_SCOPE;

  Guard guard ( this->mutex() );

  // Return now if we don't have any data.
  if ( 0 == _timesteps || 0 == _channels )
    return;

  // Remove what we have.
  _root->removeChild ( 0, _root->getNumChildren() );
  _volumeTransform->removeChild ( 0, _volumeTransform->getNumChildren () );

  // Add the volume back.
  _root->addChild ( _volumeTransform.get () );

  // If we don't have the data already...
  if ( false == this->_dataCached ( _currentTimestep, _currentChannel ) )
  {
    if ( true == this->_dataRequested ( _currentTimestep, _currentChannel ) )
    {
      // Keep a handle the job to wait for.
      _jobForScene = _requests [ Request ( _currentTimestep, _currentChannel ) ];
    }

    // Launch a job to read the data.
    else
    {
      this->_requestData ( _currentTimestep, _currentChannel, true );
    }

    // Build proxy geometry.
    _volumeTransform->addChild ( this->_buildProxyGeometry() );
  }
  #if 1
  // We have the data ...
  else
  {
    // We no longer need to wait for any job.
    _jobForScene = 0x0;

    ImageData& data ( _volumeCache [ Request ( _currentTimestep, _currentChannel ) ] );

    // Get the 3D image for the volume.
    ImagePtr image ( new osg::Image );
    image->setImage ( _x, _y, _z, GL_INTENSITY, GL_LUMINANCE, GL_UNSIGNED_BYTE, &data.front(), osg::Image::NO_DELETE );

#ifdef _MSC_VER
    osg::ref_ptr < osg::Image > scaled ( new osg::Image );
    scaled->allocateImage ( 512, 512, 128, GL_LUMINANCE, GL_UNSIGNED_BYTE );
    ::memset ( scaled->data(), 0, 512 * 512 * 128 );

    for ( unsigned int r = 0; r < _z; ++r )
    {
      for ( unsigned int t = 0; t < _y; ++ t )
      {
        for ( unsigned int s = 0; s < _x; ++ s )
        {
          *scaled->data ( s, t, r ) = *image->data ( s, t, r );
        }
      }
    }
    image = scaled;
#endif

#if USE_RAY_CASTING
    _volumeNode->samplingRate ( 1.0 / _numPlanes ); 
#else
    _volumeNode->numPlanes ( _numPlanes );
#endif
    _volumeNode->image ( image.get() );

    // Add the volume to the scene.
    _volumeTransform->addChild ( _volumeNode.get() );
    //_volumeTransform->addChild ( this->_buildVectorField ( _currentTimestep, 0, 1 ) );
  }
#else
  _volumeTransform->addChild ( this->_buildProxyGeometry() );
#endif
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
//  Normalize.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class ImageData, class FloatData >
  inline void normalize ( ImageData& out, const FloatData& in, typename FloatData::value_type min, typename FloatData::value_type max )
  {
    typedef typename ImageData::value_type PixelType;
    typedef typename FloatData::value_type FloatType;

    out.reserve ( in.size() );

    Usul::Predicates::CloseFloat< FloatType > close ( 10 );

    for ( typename FloatData::const_iterator iter = in.begin(); iter != in.end(); ++iter )
    {
      // Get the value.
      FloatType value ( *iter );
      bool inValid ( !Usul::Math::finite ( value ) || Usul::Math::nan ( value ) || close ( value, 1.0e+035f ) );
      
      if( false == inValid )
      {
        value = ( value - min ) / ( max - min );
        out.push_back ( static_cast < PixelType > ( value * 255 ) );
      }
      else
        out.push_back ( 0 );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add volume to the cache.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::addData ( unsigned int timestep, unsigned int channel, const FloatData& data )
{
  USUL_TRACE_SCOPE;

  // Get the min/max information for this channel.
  Channel::RefPtr info ( Usul::Threads::Safe::get ( this->mutex(), _channelInfo.at ( channel ) ) );

  // Normalize the data to unsigned char.
  ImageData chars;
  Detail::normalize ( chars, data, info->min (), info->max () );

  // For convienence.
  Request request ( timestep, channel );

  // Guard the rest of the function...
  Guard guard ( this->mutex() );

  // Cache the volume.
  _volumeCache [ request ] = chars;

  // Cache the raw data if we a suppose to.
  if ( _cacheRawData )
    _dataCache [ request ] = data;

  // Erase the request from the list of jobs that are running.
  _requests.erase ( request );
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

  osg::ref_ptr<osgText::Font> font ( OsgTools::Font::defaultFont() );
  text->setFont( font.get()  );
  text->setColor( osg::Vec4 ( 1.0, 1.0, 1.0, 1.0 ) );
  text->setCharacterSize( 50 );
  text->setCharacterSizeMode ( osgText::Text::SCREEN_COORDS );
  text->setPosition ( _bb.center() );
  {
    Guard guard ( this );
    text->setText( Usul::Strings::format ( "Loading ", _currentTimestep, " of ", _timesteps ) );
  }
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
  Guard guard ( this->mutex() );
  _animating = true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Stop timestep animation.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::stopTimestepAnimation ()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _animating = false;
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
//  Go to the next timestep.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::nextTimeStep()
{
  unsigned int current ( this->getCurrentTimeStep () );
  this->setCurrentTimeStep ( ++current );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Go to the previous timestep.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::previousTimeStep()
{
  unsigned int current ( this->getCurrentTimeStep () );
  if ( 0 == current )
    current = this->getNumberOfTimeSteps();
  this->setCurrentTimeStep ( --current );
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
    if ( this->isAnimating() && num % 2 == 0 )
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
  Guard guard ( this->mutex () );

  // Return now if we don't have any data.
  if ( 0 == _timesteps || 0 == _channels )
    return;

  unsigned int timestepToLoad ( 0 );

  if ( false == _volumeCache.empty () )
  {
    Request last ( ( --_volumeCache.end () )->first );
    timestepToLoad = last.first + 1;
  }

  // Wrap to the beginning.
  if ( timestepToLoad > _timesteps )
    timestepToLoad = 0;

  // Get the current channel
  unsigned int currentChannel ( this->getCurrentChannel () );

  // Only make the request if we don't already have it.
  if ( false == this->_dataCached ( timestepToLoad, currentChannel ) )
    this->_requestData ( timestepToLoad, currentChannel, false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the cached volume data.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::_updateCache ()
{
  {
    Guard guard ( this->mutex () );

    // Return now if there are still requests pending...
    if ( false == _requests.empty() )
      return;
  }

  // Don't make any more requests if the cache is full and we are not animating.
  if ( this->_cacheFull () && false == this->isAnimating () )
    return;
  
  // Make the next request.
  this->_launchNextCacheRequest ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add to the menu.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::menuAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown * caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  typedef MenuKit::Button Button;
  typedef MenuKit::RadioButton RadioButton;
  typedef MenuKit::ToggleButton ToggleButton;

  Usul::Interfaces::IUnknown::QueryPtr me ( this );

  // Make the menu.
  MenuKit::Menu::RefPtr wrf ( new MenuKit::Menu ( "WRF" ) );
  
  // Add animating toggle.
  wrf->append ( new ToggleButton ( Usul::Commands::genericToggleCommand ( "Animate", Usul::Adaptors::memberFunction<void>( this, &WRFDocument::animating ), Usul::Adaptors::memberFunction<bool> ( this, &WRFDocument::isAnimating ) ) ) );

  wrf->append ( new Button ( Usul::Commands::genericCommand ( "First Timestep",    Usul::Adaptors::bind1<void> ( 0, Usul::Adaptors::memberFunction<void> ( this, &WRFDocument::setCurrentTimeStep )  ), Usul::Commands::TrueFunctor() ) ) );
  wrf->append ( new Button ( Usul::Commands::genericCommand ( "Next Timestep",     Usul::Adaptors::memberFunction<void> ( this, &WRFDocument::nextTimeStep ),     Usul::Commands::TrueFunctor() ) ) );
  wrf->append ( new Button ( Usul::Commands::genericCommand ( "Previous Timestep", Usul::Adaptors::memberFunction<void> ( this, &WRFDocument::previousTimeStep ), Usul::Commands::TrueFunctor() ) ) );

  MenuKit::Menu::RefPtr channels ( new MenuKit::Menu ( "Channels" ) );
  for ( ChannelInfos::iterator iter = _channelInfo.begin(); iter != _channelInfo.end(); ++ iter )
  {
    channels->append ( new RadioButton ( Usul::Commands::genericCheckCommand ( 
      (*iter)->name (), 
      Usul::Adaptors::bind1<void> ( (*iter)->index (), Usul::Adaptors::memberFunction<void> ( this, &WRFDocument::currentChannel ) ), 
      Usul::Adaptors::bind1<bool> ( (*iter)->index (), Usul::Adaptors::memberFunction<bool> ( this, &WRFDocument::isCurrentChannel ) ) ) ) );
  }
  wrf->append ( channels.get() );

  wrf->append ( new Button ( Usul::Commands::genericCommand ( Usul::Strings::format ( "Multiply planes x ", 4.0  ), Usul::Adaptors::bind1<void> ( 4.0,  Usul::Adaptors::memberFunction<void> ( this, &WRFDocument::numPlanesMultiply )  ), Usul::Commands::TrueFunctor() ) ) );
  wrf->append ( new Button ( Usul::Commands::genericCommand ( Usul::Strings::format ( "Multiply planes x ", 2.0  ), Usul::Adaptors::bind1<void> ( 2.0,  Usul::Adaptors::memberFunction<void> ( this, &WRFDocument::numPlanesMultiply )  ), Usul::Commands::TrueFunctor() ) ) );
  wrf->append ( new Button ( Usul::Commands::genericCommand ( Usul::Strings::format ( "Multiply planes x ", 0.5  ), Usul::Adaptors::bind1<void> ( 0.5,  Usul::Adaptors::memberFunction<void> ( this, &WRFDocument::numPlanesMultiply )  ), Usul::Commands::TrueFunctor() ) ) );
  wrf->append ( new Button ( Usul::Commands::genericCommand ( Usul::Strings::format ( "Multiply planes x ", 0.25 ), Usul::Adaptors::bind1<void> ( 0.25, Usul::Adaptors::memberFunction<void> ( this, &WRFDocument::numPlanesMultiply )  ), Usul::Commands::TrueFunctor() ) ) );

  MenuKit::Menu::RefPtr tf ( new MenuKit::Menu ( "Transfer Functions" ) );
  typedef TransferFunctions::const_iterator ConstIterator;
  for ( ConstIterator iter = _transferFunctions.begin(); iter != _transferFunctions.end(); ++iter )
  {
    const long num ( std::distance<ConstIterator> ( _transferFunctions.begin(), iter ) );
    tf->append ( new RadioButton ( 
                 Usul::Commands::genericCheckCommand ( Usul::Strings::format ( num ), 
                                 Usul::Adaptors::bind1<void> ( num, Usul::Adaptors::memberFunction<void> ( this, &WRFDocument::transferFunction ) ), 
                                 Usul::Adaptors::bind1<bool> ( num, Usul::Adaptors::memberFunction<bool> ( this, &WRFDocument::isTransferFunction ) ) ) ) );
  }

  wrf->append ( tf.get() );

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
    _volumeCache.clear();

    for ( Requests::iterator iter = _requests.begin(); iter != _requests.end(); ++iter )
      iter->second->cancel();

    _requests.clear();
  }

  this->dirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the current channel.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int WRFDocument::getCurrentChannel ( ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _currentChannel;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the current channel.
//
///////////////////////////////////////////////////////////////////////////////

bool WRFDocument::isCurrentChannel ( unsigned int value ) const
{
  USUL_TRACE_SCOPE;
  return value == this->getCurrentChannel();
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
//  Multiply the number of planes by a factor.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::numPlanesMultiply ( double factor )
{
  this->numPlanes ( static_cast < unsigned int > ( this->numPlanes() * factor ) );
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

bool WRFDocument::isAnimating () const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _animating;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the animation state.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::animating ( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _animating = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a vector field.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node * WRFDocument::_buildVectorField ( unsigned int timestep, unsigned int channel0, unsigned int channel1 )
{
  USUL_TRACE_SCOPE;
#if 0
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

  return group.get();
#else
  return 0x0;
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Request the data.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::_requestData ( unsigned int timestep, unsigned int channel, bool wait )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  typedef LoadDataJob::ReadRequest ReadRequest;

  ReadRequest request ( timestep, channel );
  LoadDataJob::RefPtr job ( new LoadDataJob ( request, this, Parser ( _parser ) ) );

  // Add the job to the list of things we are waiting for.
  _requests.insert ( Requests::value_type ( Request ( timestep, channel ), job.get() ) );

  // If we need to wait for this job...
  if ( wait )
  {
    _jobForScene = job.get();
  }

  // Add the job to the job manager.
  Usul::Jobs::Manager::instance().addJob ( job.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Deserialize.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::deserialize ( const XmlTree::Node &node )
{
  Guard guard ( this->mutex () );

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
  Colors hsv2      ( size );
  Colors greenToRed ( size );

  const Colors::size_type RED_CHANNEL   ( 0 );
  const Colors::size_type GREEN_CHANNEL ( 1 );
  const Colors::size_type BLUE_CHANNEL  ( 2 );
  const Colors::size_type ALPHA_CHANNEL ( 3 );

  // Don't draw voxels with a value of zero.
  grayscale.at ( 0 ) [ 3 ] = 0;
  hsv.at ( 0 ) [ 3 ] = 0;
  hsv2.at ( 0 ) [ 3 ] = 0;
  greenToRed.at ( 0 )[3] = 0;

  for ( unsigned int i = 1; i < size; ++i )
  {
    float value ( static_cast < float > ( i ) / 255 );
    unsigned int alpha ( 35 );

    unsigned char c ( 120 - ( static_cast < unsigned int > ( value * 120 ) ) );
    grayscale.at ( i )[RED_CHANNEL] = c;
    grayscale.at ( i )[GREEN_CHANNEL] = c;
    grayscale.at ( i )[BLUE_CHANNEL] = c;
    grayscale.at ( i )[ALPHA_CHANNEL] = alpha;

    float r ( 0.0 ), g ( 0.0 ), b ( 0.0 );
    Usul::Functions::Color::hsvToRgb ( r, g, b, 300 - ( value * 300 ), 1.0f, 1.0f );
    hsv.at ( i )[RED_CHANNEL] = static_cast < unsigned char > ( r * 255 );
    hsv.at ( i )[GREEN_CHANNEL] = static_cast < unsigned char > ( g * 255 );
    hsv.at ( i )[BLUE_CHANNEL] = static_cast < unsigned char > ( b * 255 );
    hsv.at ( i )[ALPHA_CHANNEL] = alpha;

    hsv2.at ( i )[RED_CHANNEL] = static_cast < unsigned char > ( r * 255 );
    hsv2.at ( i )[GREEN_CHANNEL] = static_cast < unsigned char > ( g * 255 );
    hsv2.at ( i )[BLUE_CHANNEL] = static_cast < unsigned char > ( b * 255 );
    if ( value < .25 || value > .75 )
      hsv2.at ( i )[ALPHA_CHANNEL] = static_cast < unsigned char > ( Usul::Math::absolute ( ( value - 0.5 ) * ( alpha * 2 ) ) );
    else
      hsv2.at ( i )[ALPHA_CHANNEL] = 0;

    const double orginalValue ( value );
    const double startValue ( 0.7 );
    const double range ( 1.0 - startValue );
    value = startValue + ( value * range );

    const float hue ( 320 - ( value * 320 ) );
    const float saturation ( 1.0f );
    const float v ( 1.0 );

    Usul::Functions::Color::hsvToRgb ( r, g, b, hue, saturation, v );
    greenToRed.at ( i )[RED_CHANNEL] = static_cast < unsigned char > ( r * 255 );
    greenToRed.at ( i )[GREEN_CHANNEL] = static_cast < unsigned char > ( g * 255 );
    greenToRed.at ( i )[BLUE_CHANNEL] = static_cast < unsigned char > ( b * 255 );
    if ( value < 0.9 )
    {
      greenToRed.at ( i )[ALPHA_CHANNEL] = 15;
    }
    else
    {
      greenToRed.at ( i )[ALPHA_CHANNEL] = alpha;
    }

    if ( r > 0.8 )
      greenToRed.at ( i )[ALPHA_CHANNEL] = 127;
  }

  _transferFunctions.push_back ( new TransferFunction1D ( grayscale ) );
  _transferFunctions.push_back ( new TransferFunction1D ( hsv ) );
  _transferFunctions.push_back ( new TransferFunction1D ( hsv2 ) );
  _transferFunctions.push_back ( new TransferFunction1D ( greenToRed ) );
}



///////////////////////////////////////////////////////////////////////////////
//
//  Set the transfer function.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::transferFunction ( unsigned int i )
{
  Guard guard ( this );
  _currentTransferFunction = i;
  _volumeNode->transferFunction ( _transferFunctions.at ( i ).get() );
  this->dirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is this the current  transfer function?
//
///////////////////////////////////////////////////////////////////////////////

bool WRFDocument::isTransferFunction ( unsigned int i ) const
{
  Guard guard ( this );
  return i == _currentTransferFunction;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pre-render notification.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::preRenderNotify ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  BaseClass::preRenderNotify ( caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pre-render notification.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::postRenderNotify ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  BaseClass::postRenderNotify ( caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the cull and update visitors of the caller.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::addView ( Usul::Interfaces::IView *view )
{
  USUL_TRACE_SCOPE;
  BaseClass::addView ( view );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the guid.
//
///////////////////////////////////////////////////////////////////////////////

std::string WRFDocument::guid() const
{
  USUL_TRACE_SCOPE;
  return "0B3F5A87-BA2E-4B06-A229-DC9F21F7EC27";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name.
//
///////////////////////////////////////////////////////////////////////////////

std::string WRFDocument::name() const
{
  USUL_TRACE_SCOPE;
  std::string name ( BaseClass::name() );
  return ( false == name.empty() ? name : this->fileName() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the name.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::name( const std::string& name )
{
  USUL_TRACE_SCOPE;
  BaseClass::name ( name );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set show layer state.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::showLayer ( bool b )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get show layer state.
//
///////////////////////////////////////////////////////////////////////////////

bool WRFDocument::showLayer() const
{
  USUL_TRACE_SCOPE;
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of children (ITreeNode).
//
///////////////////////////////////////////////////////////////////////////////

unsigned int WRFDocument::getNumChildNodes() const
{
  return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the child node (ITreeNode).
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::ITreeNode::RefPtr WRFDocument::getChildNode ( unsigned int which )
{
  return Usul::Interfaces::ITreeNode::RefPtr ( 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the name (ITreeNode).
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::setTreeNodeName ( const std::string & s )
{
  this->name ( s );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name (ITreeNode).
//
///////////////////////////////////////////////////////////////////////////////

std::string WRFDocument::getTreeNodeName() const
{
  return this->name();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get this as an IUnknown.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* WRFDocument::asUnknown()
{
  return this->queryInterface( Usul::Interfaces::IUnknown::IID );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the busy state (IBusyState).
//
///////////////////////////////////////////////////////////////////////////////

bool WRFDocument::busyStateGet() const
{
  // The job gets set to null when it's finished and the scene is updated.
  // As long as the job is valid, we are busy.
  Guard guard ( this->mutex() );
  return _jobForScene.valid();
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the given level falls within this layer's range of levels.
//
///////////////////////////////////////////////////////////////////////////////

bool WRFDocument::isInLevelRange ( unsigned int level ) const
{
  USUL_TRACE_SCOPE;
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the state (IBooleanState).
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::setBooleanState ( bool b )
{
  USUL_TRACE_SCOPE;
  const unsigned int nodeMask ( b ? 0xffffffff : 0x0 );
  if ( true == _root.valid() )
    _root->setNodeMask ( b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the state (IBooleanState).
//
///////////////////////////////////////////////////////////////////////////////

bool WRFDocument::getBooleanState() const
{
  USUL_TRACE_SCOPE;
  return ( _root.valid() ? 0x0 != _root->getNodeMask() : false );
}
