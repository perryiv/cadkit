
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "WRFDocument.h"
#include "ChannelCommand.h"
#include "NextTimestep.h"
#include "PreviousTimestep.h"
#include "ChangeNumPlanes.h"
#include "AnimateCommand.h"

#include "Usul/File/Path.h"
#include "Usul/Strings/Case.h"
#include "Usul/Strings/Convert.h"
#include "Usul/Predicates/CloseFloat.h"
#include "Usul/Functions/Color.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Trace/Trace.h"
#include "Usul/Adaptors/Boost.h"
#include "Usul/Interfaces/GUI/IStatusBar.h"
#include "Usul/Interfaces/GUI/IProgressBarFactory.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Functions/SafeCall.h"

#include "XmlTree/Document.h"

#include "OsgTools/Font.h"
#include "OsgTools/GlassBoundingBox.h"
#include "OsgTools/Volume/Texture3DVolume.h"
#include "OsgTools/DisplayLists.h"

#include "osgText/Text"
#include "osg/Geode"
#include "osg/LightModel"

#include "osgDB/ReadFile"

#include "boost/filesystem/operations.hpp"
#include "Usul/File/Boost.h"

#include <limits>

#define INVALID_TIMESTEP std::numeric_limits < unsigned int >::max()

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( WRFDocument, WRFDocument::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
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
  _numPlanes ( 256 ),
  _channelInfo (),
  _root ( new osg::MatrixTransform ),
  _volumeTransform ( new osg::MatrixTransform ),
  _geometry ( new osg::Group ),
  _bb (),
  _dirty ( true ),
  _data (),
  _requests (),
  _jobForScene ( 0x0 ),
  _lastTimestepLoaded ( INVALID_TIMESTEP ),
  _animating ( false ),
  _offset (),
  _triangleSet ( 0x0 ),
  _topography ( 0x0 )
{
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

void WRFDocument::read ( const std::string &name, Unknown *caller )
{
  XmlTree::Document::RefPtr document ( new XmlTree::Document );
  document->load ( name );

  if ( "wrf" == document->name() )
  {
    this->_read ( *document, caller );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the document.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::_read ( XmlTree::Node &node, Unknown *caller )
{
  typedef XmlTree::Document::Attributes Attributes;
  typedef XmlTree::Document::Children Children;

  Attributes& attributes ( node.attributes() );
  for ( Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  {
    if ( "timesteps" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, _timesteps );
    }
    if ( "channels" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, _channels );
    }
    if ( "x" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, _x );
    }
    if ( "y" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, _y );
    }
    if ( "z" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, _z );
    }
    if ( "file" == iter->first )
    {
      _filename = iter->second;
    }
  }

  // Set the parser's datamembers.
  _parser.filename ( _filename );
  _parser.timesteps ( _timesteps );
  _parser.channels ( _channels );
  _parser.setSizes ( _x, _y, _z );

  // Make enough room for the channel information.
  _channelInfo.resize ( _channels );

  Children& children ( node.children() );
	
  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( *iter );
    if ( "channel" == node->name() )
    {
      this->_parseChannel ( *node, caller );
    }
    if ( "geometry" == node->name() )
    {
      this->_parseGeometry ( *node, caller );
    }
    if ( "offset" == node->name() )
    {
      this->_parseOffset ( *node, caller );
    }
    if ( "vector_field" == node->name () )
    {
      this->_parseVectorField ( *node, caller );
    }
  }

  // Initialize the bounding box.
  this->_initBoundingBox ();

  // Make room for the volumes.
  _data.resize ( _timesteps );
  //std::for_each ( _data.begin(), _data.end(), std::bind2nd ( std::mem_fun ( &ChannelVolumes::resize ), _channels ) );
  for ( TimestepsData::iterator iter = _data.begin(); iter != _data.end(); ++iter )
    iter->resize ( _channels );

  // Build the topography.
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &WRFDocument::_buildTopography ), "3345743110" );

  _volumeTransform->setMatrix ( osg::Matrix::rotate ( -osg::PI_2, osg::Z_AXIS ) * osg::Matrix::translate ( _offset ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse a channel.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::_parseChannel ( XmlTree::Node &node, Unknown *caller )
{
  typedef XmlTree::Document::Attributes Attributes;

  ChannelInfo info;

  Attributes& attributes ( node.attributes() );
  for ( Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  {
    if ( "name" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, info.name );
    }
    if ( "index" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, info.index );
    }
    if ( "min" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, info.min );
    }
    if ( "max" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, info.max );
    }
  }

  _channelInfo.at ( info.index ) = info;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse any external geometry..
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::_parseGeometry ( XmlTree::Node &node, Unknown *caller )
{
  std::string directory ( node.value () );

  typedef std::vector < std::string > Filenames;
  Filenames filenames;

  if ( boost::filesystem::is_directory ( directory ) )
  {
    Usul::File::findFiles ( directory, "osga", filenames );
    Usul::File::findFiles ( directory, "osg", filenames );
    Usul::File::findFiles ( directory, "ive", filenames );

    for ( Filenames::const_iterator iter = filenames.begin(); iter != filenames.end(); ++iter )
    {
      const std::string filename ( *iter );

      // Find a document that can load the first filename.
      Usul::Documents::Document::RefPtr document ( Usul::Documents::Manager::instance().find ( filename ).document );
      Usul::Interfaces::IBuildScene::QueryPtr buildScene ( document );

      // Make sure the document can build a scene.
      if ( document.valid() && buildScene.valid() )
      {
        // Set the label.
        Usul::Interfaces::IStatusBar::UpdateStatusBar label ( caller );
        label ( "Loading filename: " + filename, true );

        if ( document->canOpen ( filename ) )
          document->read ( filename, caller );

        // Get the node.
        Usul::Interfaces::IBuildScene::Options options;
        _geometry->addChild ( buildScene->buildScene ( options ) );
      }
    }
  }
  else
  {
    _geometry->addChild ( osgDB::readNodeFile ( directory ) );
  }

  // Turn off display lits.
  //OsgTools::DisplayLists dl ( false );
  //dl ( _geometry.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse any offset.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::_parseOffset ( XmlTree::Node &node, Unknown *caller )
{
  std::string offset ( node.value() );

  if ( false == offset.empty() )
  {
    std::istringstream in ( offset );
    double x ( 0.0 ), y ( 0.0 ), z ( 0.0 );
    in >> x >> y >> z;
    _offset.set ( x, y, z );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse any vector field information.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::_parseVectorField ( XmlTree::Node& node, Unknown *caller )
{
  typedef XmlTree::Document::Attributes Attributes;

  Attributes& attributes ( node.attributes() );
  for ( Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  {
    if ( "name" == iter->first )
    {
    }
    if ( "u" == iter->first )
    {
    }
    if ( "v" == iter->first )
    {
    }
  }
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


namespace Detail
{
  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Normalize.
  //
  ///////////////////////////////////////////////////////////////////////////////

  void normalize ( std::vector < unsigned char >& out, const std::vector < float >& in, double min, double max )
  {
    out.resize ( in.size() );

    Usul::Predicates::CloseFloat< double > close ( 10 );

    for ( unsigned int i = 0; i < in.size(); ++i )
    {
      double value ( in.at( i ) );
      bool inValid ( !Usul::Math::finite ( value ) || Usul::Math::nan ( value ) || close ( value, 1.0e+035f ) );
      
      if( false == inValid )
      {
        value = ( value - min ) / ( max - min );
        out.at( i ) = static_cast < unsigned char > ( value * 255 );
      }
      else
        out.at ( i ) = 0;
    }
  }

  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Build the transferFunction.
  //
  ///////////////////////////////////////////////////////////////////////////////

  osg::Image* buildTransferFunction ( )
  {
    osg::ref_ptr < osg::Image > image ( new osg::Image );
    image->allocateImage ( 256, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE );

    unsigned char *data ( image->data() );

    for ( unsigned int i = 0; i < 256; ++i )
    {
  #if 0
      *data++ = i;
      *data++ = i;
      *data++ = i;
      *data++ = i;//i == 0 ? 0 : 5;
  #else
      float value ( static_cast < float > ( i ) / 255 );
      float r ( 0.0 ), g ( 0.0 ), b ( 0.0 );
      Usul::Functions::hsvToRgb ( r, g, b, ( 300 - ( value * 300 ) ), 1.0f, 1.0f );
      *data++ = static_cast < unsigned char > ( r * 255 );
      *data++ = static_cast < unsigned char > ( g * 255 );
      *data++ = static_cast < unsigned char > ( b * 255 );
      *data++ = ( i == 0 ? 0 : 10 );
  #endif
    }

    return image.release();
  }
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

  // Remove what we have.
  {
    Guard guard ( this->mutex() );
    _root->removeChild ( 0, _root->getNumChildren() );
    _root->addChild ( _geometry.get() );
    _volumeTransform->removeChild ( 0, _volumeTransform->getNumChildren () );
    _volumeTransform->addChild ( _topography.get() );
    _root->addChild ( _volumeTransform.get() );
  }

  // If we don't have the data already...
  if ( false == this->_dataCached ( _currentTimestep, _currentChannel ) )
  {
    if ( true == this->_dataRequested ( _currentTimestep, _currentChannel ) )
    {
      // Keep a handle the job to wait for.
      {
        Guard guard ( this->mutex() );
        _jobForScene = _requests [ Request ( _currentTimestep, _currentChannel ) ];
      }
    }

    // Launch a job to read the data.
    else
    {
      typedef LoadDataJob::ReadRequest ReadRequest;
      typedef LoadDataJob::ReadRequests ReadRequests;

      ChannelInfo info ( _channelInfo.at ( _currentChannel ) );

      ReadRequest request ( _currentTimestep, info );
      ReadRequests requests;
      requests.push_back ( request );

      LoadDataJob::RefPtr job ( new LoadDataJob ( requests, this, _parser ) );
      job->setSize ( _x, _y, _z );

      _jobForScene = job.get();

      {
        Guard guard ( this->mutex() );
        _requests.insert ( Requests::value_type ( Request ( _currentTimestep, _currentChannel ), job.get() ) );
      }

      Usul::Jobs::Manager::instance().add ( job.get() );
    }

    // Build proxy geometry.
    {
      Guard guard ( this->mutex() );
      _root->addChild ( this->_buildProxyGeometry() );
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

    // Matrix transform to place the volume.
    

    // Make a bounding box around the volume.
    //OsgTools::GlassBoundingBox gbb ( _bb );
    //gbb ( _volumeTransform.get(), true, false, false );

    // Add the volume to the scene.
    {
      Guard guard ( this->mutex() );
      _volumeTransform->addChild ( this->_buildVolume ( image.get() ) );
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
  return _data [ timestep ] [ channel ].valid();
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

void WRFDocument::addVolume ( osg::Image* image, unsigned int timestep, unsigned int channel )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _data [ timestep ] [ channel ] = image;
  _requests.erase ( Request ( timestep, channel ) );
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
  return _data [ timestep ] [ channel ].get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the volume.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* WRFDocument::_buildVolume( osg::Image* image )
{
  USUL_TRACE_SCOPE;

  osg::ref_ptr < OsgTools::Volume::Texture3DVolume > volume ( new OsgTools::Volume::Texture3DVolume );
  volume->numPlanes ( _numPlanes );
  volume->image ( image );
  volume->boundingBox ( _bb );
  volume->transferFunction ( Detail::buildTransferFunction() );

  return volume.release();
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
  double xLength ( _x * 10 );
  double yLength ( _y * 10 );
  double zLength ( _z * 3 );

  float xHalf ( static_cast < float > ( xLength ) / 2.0f );
  float yHalf ( static_cast < float > ( yLength ) / 2.0f );
  float zHalf ( static_cast < float > ( zLength ) / 2.0f );

  _bb.set ( -xHalf, -yHalf, -zHalf, xHalf, yHalf, zHalf );
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

  if ( _requests.empty() )
  {
    unsigned int timestepToLoad ( 0 );

    if ( INVALID_TIMESTEP != _lastTimestepLoaded )
    {
      timestepToLoad = _lastTimestepLoaded + 1;
    }

    _lastTimestepLoaded = timestepToLoad;

    if ( timestepToLoad < _timesteps )
    {
      typedef LoadDataJob::ReadRequest ReadRequest;
      typedef LoadDataJob::ReadRequests ReadRequests;

      ReadRequests requests;

      for ( unsigned int i = 0; i < _channels; ++ i )
      {
        // Only make a request if we don't alreay have the data.
        if ( false == this->_dataCached ( timestepToLoad, i ) )
        {
          ChannelInfo info ( _channelInfo.at ( i ) );
          ReadRequest request ( timestepToLoad, info );
          requests.push_back ( request );
        }
      }

      LoadDataJob::RefPtr job ( new LoadDataJob ( requests, this, _parser ) );
      job->setSize ( _x, _y, _z );

      for ( ReadRequests::const_iterator iter = requests.begin(); iter != requests.end(); ++iter )
      {
        Guard guard ( this->mutex() );
        _requests.insert ( Requests::value_type ( Request ( iter->first, iter->second.index ), job.get() ) );
      }

      Usul::Jobs::Manager::instance().add ( job.get() );
    }
  }

  if ( this->dirty () )
    this->_buildScene();

  static unsigned int num ( 0 );
  ++num;
  if ( this->animating() && num % 60 == 0 )
  {
    unsigned int currentTimestep ( this->getCurrentTimeStep () );
    this->setCurrentTimeStep ( ++currentTimestep );

    this->_buildScene();
  }
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
    commands.push_back ( new ChannelCommand ( iter->name, iter->index, this ) );
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
//  Constructor..
//
///////////////////////////////////////////////////////////////////////////////

WRFDocument::LoadDataJob::LoadDataJob ( const ReadRequests& requests, WRFDocument* document, const Parser& parser ) :
  BaseClass (),
  _requests ( requests ),
  _document ( document ),
  _parser ( parser ),
  _x ( 0 ),
  _y ( 0 ),
  _z ( 0 )
{
  USUL_TRACE_SCOPE;
}



///////////////////////////////////////////////////////////////////////////////
//
//  Get the data.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::LoadDataJob::_started ()
{
  USUL_TRACE_SCOPE;

  // Return if we don't have any.
  if ( _requests.empty() || false == _document.valid() )
    return;


  // Process all the requests.
  while ( false == _requests.empty() )
  {
    ReadRequest request ( _requests.front() );
    _requests.pop_front ();

    osg::ref_ptr < osg::Image > image ( this->_createImage ( request ) );

    unsigned int timestep ( request.first );
    unsigned int channel ( request.second.index );

    _document->addVolume ( image.get(), timestep, channel );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  All done.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::LoadDataJob::_finished ()
{
  USUL_TRACE_SCOPE;

  if ( _document.valid() )
    _document->loadJobFinished ( this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the sizes..
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::LoadDataJob::setSize ( unsigned int x, unsigned int y, unsigned int z )
{
  USUL_TRACE_SCOPE;

  _x = x;
  _y = y;
  _z = z;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the image.
//
///////////////////////////////////////////////////////////////////////////////

osg::Image*  WRFDocument::LoadDataJob::_createImage ( const ReadRequest& request )
{
  USUL_TRACE_SCOPE;

  ChannelInfo info ( request.second );
  unsigned int timestep ( request.first );
  unsigned int channel ( info.index );

  Parser::Data data;

  std::cout << "Reading data.  Timestep: " << timestep << " Channel: " << channel << std::endl;

  _parser.data ( data, timestep, channel );

  // Normalize the data to unsigned char.
  std::vector < unsigned char > chars;
  Detail::normalize ( chars, data, info.min, info.max );

  unsigned int width ( _y ), height ( _x ), depth ( _z );

  osg::ref_ptr < osg::Image > image ( new osg::Image );
  image->allocateImage ( width, height, depth, GL_LUMINANCE, GL_UNSIGNED_BYTE );

  unsigned char *pixels ( image->data() );
  std::copy ( chars.begin(), chars.end(), pixels );
#if 0
  osg::ref_ptr < osg::Image > scaled ( new osg::Image );
  scaled->allocateImage ( 256, 256, 128, GL_LUMINANCE, GL_UNSIGNED_BYTE );
  ::memset ( scaled->data(), 0, 256 * 256 * 128 );

  for ( unsigned int r = 0; r < depth; ++r )
  {
    for ( unsigned int t = 0; t < height; ++ t )
    {
      for ( unsigned int s = 0; s < width; ++ s )
      {
        *scaled->data ( s, t, r ) = *image->data ( s, t, r );
      }
    }
  }

  return scaled.release ();
#else
  return image.release();
#endif
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
  parser.topography ( data );

  // Make a triangle set.
  TriangleSet::RefPtr triangles ( new TriangleSet );

  const unsigned int width ( _y ), height ( _x );

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

      // Add first triangle.
      {
        osg::Vec3 n ( ( d - c ) ^ ( a - c ) );
        n.normalize();
        OsgTools::Triangles::Triangle::RefPtr t ( triangles->addTriangle ( a, c, d, n, false ) );
        t->original ( true );
        //vertices->push_back ( a );
        //vertices->push_back ( c );
        //vertices->push_back ( d );
        //normals->push_back ( n );
      }

      // Add second triangle.
      {
        osg::Vec3 n ( ( b - d ) ^ ( a - d ) );
        n.normalize();
        OsgTools::Triangles::Triangle::RefPtr t ( triangles->addTriangle ( a, d, b, n, false ) );
        t->original ( true );

        //vertices->push_back ( a );
        //vertices->push_back ( d );
        //vertices->push_back ( b );
        //normals->push_back ( n );
      }
    }
  }

#if 1
  // Build the scene.
  TriangleSet::Options options;
  options [ "normals" ] = "per-vertex";

  osg::ref_ptr < osg::Node > node ( triangles->buildScene ( options, 0x0 ) );

  // Turn off display lists.
  triangles->displayList ( true );
#else
  osg::ref_ptr < osg::Geode > node ( new osg::Geode );
  osg::ref_ptr < osg::Geometry > geometry ( new osg::Geometry );
  geometry->setUseDisplayList ( false );

  geometry->setVertexArray ( vertices.get() );
  geometry->setNormalArray ( normals.get() );
  geometry->setNormalBinding ( osg::Geometry::BIND_PER_PRIMITIVE );

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
  material->setAmbient ( osg::Material::FRONT, frontDiffuse );

  material->setDiffuse ( osg::Material::BACK,  backDiffuse );
  material->setDiffuse ( osg::Material::FRONT, frontDiffuse );

  ss->setAttribute ( material.get(), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
#endif
  // Set the data members.
  {
    Guard guard ( this->mutex() );
    _triangleSet = triangles;
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
  return 0x0;
}
