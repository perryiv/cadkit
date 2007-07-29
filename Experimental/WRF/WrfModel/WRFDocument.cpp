
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

#include "Usul/File/Path.h"
#include "Usul/Strings/Case.h"
#include "Usul/Strings/Convert.h"
#include "Usul/Predicates/CloseFloat.h"
#include "Usul/Functions/Color.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Trace/Trace.h"

#include "XmlTree/Document.h"

#include "OsgTools/Font.h"
#include "OsgTools/GlassBoundingBox.h"
#include "OsgTools/Volume/Texture3DVolume.h"

#include "osgText/Text"
#include "osg/Geode"

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
  _bb (),
  _dirty ( true ),
  _data (),
  _requests (),
  _jobForScene ( 0x0 )
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
    this->_read ( *document );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the document.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::_read ( XmlTree::Node &node )
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
      this->_parseChannel ( *node );
    }
  }

  // Initialize the bounding box.
  this->_initBoundingBox ();

  // Make room for the volumes.
  _data.resize ( _timesteps );
  //std::for_each ( _data.begin(), _data.end(), std::bind2nd ( std::mem_fun ( &ChannelVolumes::resize ), 4 ) );
  for ( TimestepsData::iterator iter = _data.begin(); iter != _data.end(); ++iter )
    iter->resize ( 4 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse a channel.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::_parseChannel ( XmlTree::Node &node )
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
      Usul::Functions::hsvToRgb ( r, g, b, value * 360, 1.0f, 1.0f );
      *data++ = static_cast < unsigned char > ( r * 255 );
      *data++ = static_cast < unsigned char > ( g * 255 );
      *data++ = static_cast < unsigned char > ( b * 255 );
      *data++ = ( i < 10 ? 0 : 10 );
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

    ImagePtr image ( this->_volume ( _currentTimestep, _currentChannel ) );

    // Add the volume to the scene.
    {
      Guard guard ( this->mutex() );
      _root->addChild ( this->_buildVolume ( image.get() ) );
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
  text->setFont( OsgTools::Font::defaultFont());
  text->setColor( osg::Vec4 ( 1.0, 1.0, 1.0, 1.0 ) );
  text->setCharacterSize( 25 );
  text->setPosition ( _bb.center() );
  text->setText( "Loading..." );
  text->setAutoRotateToScreen( true );
  text->setFontResolution( 40, 40 );

  osg::ref_ptr < osg::Geode > geode ( new osg::Geode );
  geode->addDrawable ( text.get () );

  group->addChild ( geode.get() );

  return group.release();
}


/// Usul::Interfaces::ITimestepAnimation
void WRFDocument::startTimestepAnimation ()
{
}

void WRFDocument::stopTimestepAnimation ()
{
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

  if ( this->dirty () )
    this->_buildScene();

#if 0
  static unsigned int num ( 0 );
  ++num;
  if ( num % 60 == 0 )
  {
    ++_currentTimestep;
    if ( _currentTimestep >= _timesteps )
      _currentTimestep = 0;

    this->_buildScene();
  }
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get a list of avialable commands.
//
///////////////////////////////////////////////////////////////////////////////

WRFDocument::CommandList WRFDocument::getCommandList ()
{
  USUL_TRACE_SCOPE;

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

  std::cout << " Number of scalar values: " << data.size () << std::endl;
  std::cout << " Number of normalized charaters: " << chars.size () << std::endl;

  unsigned int width ( _y ), height ( _x ), depth ( _z );

  std::cout << "Allocating image: " << width << " " << height << " " << depth << std::endl;
  osg::ref_ptr < osg::Image > image ( new osg::Image );
  image->allocateImage ( width, height, depth, GL_LUMINANCE, GL_UNSIGNED_BYTE );

#if 1
   std::cout << "Copying pixels..." << std::endl;
  unsigned char *pixels ( image->data() );
  std::copy ( chars.begin(), chars.end(), pixels );

   std::cout << "Scaling image..." << std::endl;
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
