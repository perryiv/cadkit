
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Layers/RasterLayer.h"
#include "Minerva/Core/Functions/CacheString.h"
#include "Minerva/Core/ElevationData.h"

#include "Usul/Adaptors/Bind.h"
#include "Usul/App/Application.h"
#include "Usul/Components/Manager.h"
#include "Usul/File/Make.h"
#include "Usul/File/Stats.h"
#include "Usul/File/Remove.h"
#include "Usul/File/Temp.h"
#include "Usul/Functions/Color.h"
#include "Usul/Functions/GUID.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Scope/Caller.h"
#include "Usul/Strings/Split.h"
#include "Usul/Threads/ThreadId.h"

#include "osgDB/ReadFile"
#include "osgDB/WriteFile"

#include "osg/Image"

#include "boost/functional/hash.hpp"

#include <algorithm>
#include <limits>
#include <ctime>

using namespace Minerva::Core::Layers;


USUL_IMPLEMENT_IUNKNOWN_MEMBERS( RasterLayer, RasterLayer::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayer::RasterLayer() : 
  BaseClass(),
  _alphas(),
  _alpha ( 1.0f ),
  _cacheDir ( RasterLayer::defaultCacheDirectory() ),
  _reader ( 0x0 ),
  _log ( 0x0 ),
  _levelRange ( 0, std::numeric_limits<unsigned int>::max() )
{
  this->_registerMembers();

  // Initialize the reader.
  this->_imageReaderFind ( "png" );
  
  // Set an inital guid.
  this->objectId ( Usul::Functions::GUID::generate() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayer::RasterLayer ( const RasterLayer& rhs ) : BaseClass ( rhs ),
  _alphas ( rhs._alphas ),
  _alpha ( rhs._alpha ),
  _cacheDir ( rhs._cacheDir ),
  _reader ( rhs._reader ),
  _log ( rhs._log ),
  _levelRange ( rhs._levelRange )
{
  this->_registerMembers();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayer::~RasterLayer()
{
  _alphas.clear();
  _cacheDir.clear();
  _reader = 0x0;
  _log = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Register members for serialization.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayer::_registerMembers()
{
  // Serialization glue.
  this->_addMember ( new Serialize::XML::ValueMapMember<Alphas> ( "alphas", _alphas ) );
  this->_addMember ( "alpha", _alpha );
  this->_addMember ( "level_range", _levelRange );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get overall alpha value.
//
///////////////////////////////////////////////////////////////////////////////

float RasterLayer::alpha() const
{
  Guard guard ( this );
  return _alpha;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set overall alpha value.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayer::alpha ( float a )
{
  Guard guard ( this );
  _alpha = a;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add an alpha value.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayer::alpha ( unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha )
{
  Guard guard ( this );
  Color color ( Usul::Functions::Color::pack ( red, green, blue, 0 ) );
  _alphas[color] = alpha;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the alpha values.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayer::Alphas RasterLayer::alphas() const
{
  Guard guard ( this );
  return _alphas;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the alpha values.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayer::alphas( const Alphas& alphas )
{
  Guard guard ( this );
  _alphas = alphas;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a blank image.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayer::ImagePtr RasterLayer::_createBlankImage( unsigned int width, unsigned int height ) const
{
  ImagePtr result ( new osg::Image );
  result->allocateImage ( width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE );
  ::memset ( result->data(), 0, result->getImageSizeInBytes() );

  return result;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for an interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* RasterLayer::queryInterface( unsigned long iid )
{
  switch (iid)
  {
  case Usul::Interfaces::IRasterLayer::IID:
    return static_cast<Usul::Interfaces::IRasterLayer*> ( this );
  case Usul::Interfaces::ILayer::IID:
    return static_cast<Usul::Interfaces::ILayer*> ( this );
  case Usul::Interfaces::IRasterAlphas::IID:
    return static_cast<Usul::Interfaces::IRasterAlphas*> ( this );
  case Usul::Interfaces::IClonable::IID:
    return static_cast<Usul::Interfaces::IClonable*> ( this );
  case Usul::Interfaces::IBooleanState::IID:
    return static_cast < Usul::Interfaces::IBooleanState* > ( this );
  case Minerva::Interfaces::ITileElevationData::IID:
    return static_cast<Minerva::Interfaces::ITileElevationData*> ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the guid for the layer.
//
///////////////////////////////////////////////////////////////////////////////

std::string RasterLayer::guid() const
{
  return this->objectId();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set show layer.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayer::showLayer ( bool b )
{
  this->visibility ( b );
}


///////////////////////////////////////////////////////////////////////////////
//
//   Get show layer.
//
///////////////////////////////////////////////////////////////////////////////

bool RasterLayer::showLayer() const
{
  return this->visibility();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the state (IBooleanState).
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayer::setBooleanState ( bool b )
{
  USUL_TRACE_SCOPE;
  this->showLayer ( b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the state (IBooleanState).
//
///////////////////////////////////////////////////////////////////////////////

bool RasterLayer::getBooleanState() const
{
  USUL_TRACE_SCOPE;
  return this->showLayer();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the base cache directory.
//
///////////////////////////////////////////////////////////////////////////////

std::string RasterLayer::baseCacheDirectory() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _cacheDir;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the base cache directory.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayer::baseCacheDirectory ( const std::string& dir, bool makeDefault )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  
  // Only set the directory if it's not empty.
  if ( false == dir.empty() )
  {
    // Set the directory.
    _cacheDir = dir;

    // Make it the default if we should.
    if ( makeDefault )
      RasterLayer::defaultCacheDirectory ( _cacheDir );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the texture.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayer::ImagePtr RasterLayer::texture ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job *job, IUnknown * )
{
  USUL_TRACE_SCOPE;

  // See if the job has been cancelled.
  RasterLayer::_checkForCanceledJob ( job );

  // Make the file name.
  std::string file;
  if ( CACHE_STATUS_FILE_OK == this->_getAndCheckCacheFilename ( extents, width, height, level, file ) )
  {
    RasterLayer::_checkForCanceledJob ( job );

    // Load the file, set the file name and return.
    ImagePtr image ( this->_readImageFile ( file ) );
    //image->setFileName ( file );
    return image;
  }

  return ImagePtr ( 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the cache filename.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayer::CacheStatus RasterLayer::_getAndCheckCacheFilename ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level, std::string& filename )
{
  // Get the cache directory.
  const std::string cachDir ( this->_cacheDirectory() );
  if ( true == cachDir.empty() )
    return CACHE_STATUS_FILE_NAME_ERROR;

  // Make the directory. Guard it so that it's atomic.
  const std::string baseDir ( this->_baseDirectory ( cachDir, width, height, level ) );
  {
    Guard guard ( this );
    Usul::Functions::safeCallR1 ( Usul::File::make, baseDir, "3201050697" );
  }

  // Make the file name.
  filename = Usul::Strings::format ( baseDir, this->_baseFileName ( extents ), '.', this->_cacheFileExtension() );

  // If the file does not exist then return.
  if ( false == Usul::Predicates::FileExists::test ( filename ) )
    return CACHE_STATUS_FILE_DOES_NOT_EXIST;

  // If the file is empty then remove it and return.
  if ( 0 == Usul::File::size ( filename ) )
  {
    this->_logEvent ( Usul::Strings::format ( "Warning 3552986954: Removing empty cached file: ", filename ) );
    Usul::File::remove ( filename, false, &std::cout );
    return CACHE_STATUS_FILE_EMPTY;
  }

  return CACHE_STATUS_FILE_OK;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the directory.
//
///////////////////////////////////////////////////////////////////////////////

std::string RasterLayer::_baseDirectory ( const std::string &cacheDir, unsigned int width, unsigned int height, unsigned int level ) const
{
  USUL_TRACE_SCOPE;
  return Minerva::Core::Functions::makeDirectoryString ( cacheDir, width, height, level );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the base file name.
//
///////////////////////////////////////////////////////////////////////////////

std::string RasterLayer::_baseFileName ( Extents extents ) const
{
  USUL_TRACE_SCOPE;
  return Minerva::Core::Functions::makeExtentsString ( extents );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the directory.
//
///////////////////////////////////////////////////////////////////////////////

std::string RasterLayer::_cacheDirectory() const
{
  USUL_TRACE_SCOPE;
  return std::string ( "" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Default is to cache in PNG format.
//
///////////////////////////////////////////////////////////////////////////////

std::string RasterLayer::_cacheFileExtension() const
{
  USUL_TRACE_SCOPE;
  return std::string ( "png" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Check if the job has been marked as canceled. If so, cancel it, which 
//  should throw an exception.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayer::_checkForCanceledJob ( Usul::Jobs::Job *job )
{
  USUL_TRACE_SCOPE_STATIC;
  if ( ( 0x0 != job ) && ( true == job->canceled() ) )
    job->cancel();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read an image file.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayer::ImagePtr RasterLayer::_readImageFile ( const std::string &file ) const
{
  return RasterLayer::_readImageFile ( file, this->_imageReaderGet() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read an image file.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayer::ImagePtr RasterLayer::_readImageFile ( const std::string &file, ReaderPtr reader )
{
  USUL_TRACE_SCOPE_STATIC;

  // Try to use the given reader.
  if ( reader.valid() )
    return reader->readImageFile ( file );

  // Temporarily turn off verbose output.
  osg::NotifySeverity level ( osg::getNotifyLevel() );
  osg::setNotifyLevel ( osg::ALWAYS ); // Yes, this turns it off.
  Usul::Scope::Caller::RefPtr reset ( Usul::Scope::makeCaller ( Usul::Adaptors::bind1 ( level, osg::setNotifyLevel ) ) );

  // Fall back on OSG if we don't have a reader.
  return osgDB::readImageFile ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the image reader.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayer::ReaderPtr RasterLayer::_imageReaderGet() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return ReaderPtr ( _reader );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the image reader.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayer::_imageReaderSet ( ReaderPtr reader )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _reader = reader;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find a reader for our format.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayer::_imageReaderFind ( const std::string &ext )
{
  USUL_TRACE_SCOPE;

  // Typedefs to shorten the lines.
  typedef Usul::Components::Manager PluginManager;
  typedef PluginManager::UnknownSet Unknowns;

  // Get the list of unknowns that can read this format.
  Unknowns unknowns ( PluginManager::instance().getInterfaces ( IReadImageFile::IID ) );

  // Find the one that also handles the extension.
  for ( Unknowns::const_iterator iter = unknowns.begin(); iter != unknowns.end(); ++iter )
  {
    IReadImageFile::QueryPtr reader ( iter->get() );
    if ( reader.valid() && reader->canRead ( Usul::Strings::format ( ".", ext ) ) )
    {
      // Set the new image reader.
      this->_imageReaderSet ( reader.get() );
      break;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the hashed string.
//
///////////////////////////////////////////////////////////////////////////////

std::size_t RasterLayer::_hashString ( const std::string &s )
{
  USUL_TRACE_SCOPE_STATIC;

  typedef boost::hash<std::string> StringHash;
  StringHash hashFunction;
  const std::size_t value ( hashFunction ( s ) );

  return value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the hashed string.
//
///////////////////////////////////////////////////////////////////////////////

std::string RasterLayer::_buildCacheDir ( const std::string &rootDir, const std::string &sourceDir, const std::size_t hashValue )
{
  USUL_TRACE_SCOPE_STATIC;

  const std::string program ( Usul::App::Application::instance().program() );
  std::string dir ( Usul::Strings::format ( rootDir, '/', program, '/', sourceDir, '/', hashValue, '/' ) );
  std::replace ( dir.begin(), dir.end(), '\\', '/' );

  return dir;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the cache filename.
//
///////////////////////////////////////////////////////////////////////////////

std::string RasterLayer::_cacheFileName( const Extents& extents, unsigned int width, unsigned int height, unsigned int level ) const
{
  // Get the cache directory.
  const std::string cacheDir ( this->_cacheDirectory() );
  const std::string baseDir ( this->_baseDirectory ( cacheDir, width, height, level ) );
  std::string cacheFile ( Usul::Strings::format ( baseDir, this->_baseFileName ( extents ), '.', this->_cacheFileExtension() ) );
  return cacheFile;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the image to cache.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayer::_writeImageToCache ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level, ImagePtr image )
{
  USUL_TRACE_SCOPE;

  // Check the image.
  if ( false == image.valid() )
    return;

  // Build the file name.
  std::string cacheFile ( this->_cacheFileName ( extents, width, height, level ) );

  // Write the file.
  osgDB::writeImageFile ( *image, cacheFile );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the log.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayer::logSet ( LogPtr lp )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _log = lp;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the log.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayer::LogPtr RasterLayer::logGet()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return LogPtr ( _log );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Log the event.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayer::_logEvent ( const std::string &s )
{
  USUL_TRACE_SCOPE;

  // Make the message.
  const std::string message ( Usul::Strings::format 
    ( "clock: ", ::clock(), ", system thread: ", Usul::Threads::currentThreadId(), ", event: ", s ) );

  // Get the log.
  LogPtr file ( this->logGet() );

  // Use the log if it's valid.
  if ( ( false == s.empty() ) && ( true == file.valid() ) )
  {
    file->write ( message );
  }

  // Otherwise, print to stdout.
  else
  {
    std::cout << Usul::Strings::format ( "Warning 3604300631: layer '", this->name(), "' has a null log file", '\n', message, '\n' ) << std::flush;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the given level falls within this layer's range of levels.
//
///////////////////////////////////////////////////////////////////////////////

bool RasterLayer::isInLevelRange ( unsigned int level ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return ( ( level >= _levelRange[0] ) && ( level <= _levelRange[1] ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name.
//
///////////////////////////////////////////////////////////////////////////////

std::string RasterLayer::name() const
{
  USUL_TRACE_SCOPE;
  return BaseClass::name();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the name.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayer::name ( const std::string& name )
{
  USUL_TRACE_SCOPE;
  BaseClass::name ( name );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create ElevationData from an osg::Image.  This is to support the legacy api and will be removed.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail {

  template < class SrcType >
  RasterLayer::IElevationData::RefPtr convertFromOsgImage ( const osg::Image& image )
  {
    const SrcType *src ( reinterpret_cast < const SrcType* > ( image.data()  ) );

    // Make sure the pointers are valid.
    if ( 0x0 == src )
      return 0x0;

    const unsigned int width ( image.s() );
    const unsigned int height ( image.t() );
    Minerva::Interfaces::IElevationData::RefPtr data ( new Minerva::Core::ElevationData ( width, height ) );
    for ( unsigned int i = 0; i < width; ++i )
    {
      for ( unsigned int j = 0; j < height; ++j )
      {
        const SrcType value ( *reinterpret_cast < const SrcType * > ( image.data ( i, j ) ) );
        data->value ( i, j, static_cast<float> ( value ) );
      }
    }
    return data;
  }

  
  RasterLayer::IElevationData::RefPtr convertFromOsgImage ( osg::Image *image )
  {
    if ( 0x0 != image )
    {
      switch ( image->getDataType() )
      {
      // Treat unsigned shorts as shorts.  Any number greater than max short will be treated as a negative number.
      // This is a work around for one earth's wms server.
      case GL_UNSIGNED_SHORT:
      case GL_SHORT:
        return Detail::convertFromOsgImage<Usul::Types::Int16> ( *image );
      case GL_UNSIGNED_BYTE:
        return Detail::convertFromOsgImage<Usul::Types::Uint8> ( *image );
      case GL_FLOAT:
        return Detail::convertFromOsgImage<Usul::Types::Float32> ( *image );
      }
    }

    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the raster data as elevation data.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayer::IElevationData::RefPtr RasterLayer::elevationData ( 
  double minLon,
  double minLat,
  double maxLon,
  double maxLat,
  unsigned int width,
  unsigned int height,
  unsigned int level,
  Usul::Jobs::Job* job,
  Usul::Interfaces::IUnknown* caller )
{
  osg::ref_ptr<osg::Image> image ( this->texture ( Extents ( minLon, minLat, maxLon, maxLat ), width, height, level, job, caller ) );
  Minerva::Interfaces::IElevationData::RefPtr data ( Detail::convertFromOsgImage ( image.get() ) );
  return data;
}
