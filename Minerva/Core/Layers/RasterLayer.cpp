
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Layers/RasterLayer.h"

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
#include "Usul/Math/Absolute.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Registry/Database.h"
#include "Usul/Scope/Caller.h"
#include "Usul/Strings/Split.h"
#include "Usul/Threads/ThreadId.h"

#include "osgDB/ReadFile"
#include "osgDB/WriteFile"

#include "osg/Image"

#include "boost/algorithm/string/trim.hpp"
#include "boost/algorithm/string/replace.hpp"
#include "boost/functional/hash.hpp"

#include <algorithm>
#include <limits>
#include <ctime>

using namespace Minerva::Core::Layers;


/////////////////////////////////////////////////////////////////////////////
//
//  Declare serialization wrappers.
//
/////////////////////////////////////////////////////////////////////////////

USUL_IO_TEXT_DEFINE_READER_TYPE_VECTOR_4 ( RasterLayer::Extents );
USUL_IO_TEXT_DEFINE_WRITER_TYPE_VECTOR_4 ( RasterLayer::Extents );
SERIALIZE_XML_DECLARE_VECTOR_4_WRAPPER ( RasterLayer::Extents );

USUL_IMPLEMENT_IUNKNOWN_MEMBERS( RasterLayer, RasterLayer::BaseClass );


/////////////////////////////////////////////////////////////////////////////
//
//  Registry sections.
//
/////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  const std::string RASTER_LAYER_CACHE_DIR ( "raster_layer_cache_dir" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayer::RasterLayer() : 
  BaseClass(),
  _extents( -180.0, -90.0, 180.0, 90.0 ),
  _alphas(),
  _guid ( Usul::Functions::GUID::generate() ),
  _shown ( true ),
  _alpha ( 1.0f ),
  _cacheDir ( RasterLayer::defaultCacheDirectory() ),
  _reader ( 0x0 ),
  _log ( 0x0 ),
  _levelRange ( 0, std::numeric_limits<unsigned int>::max() ),
  SERIALIZE_XML_INITIALIZER_LIST
{
  this->_registerMembers();

  // Initialize the reader.
  this->_imageReaderFind ( "png" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayer::RasterLayer ( const RasterLayer& rhs ) : BaseClass ( rhs ),
  _extents ( rhs._extents ),
  _alphas ( rhs._alphas ),
  _guid ( Usul::Functions::GUID::generate() ),
  _shown ( rhs._shown ),
  _alpha ( rhs._alpha ),
  _cacheDir ( rhs._cacheDir ),
  _reader ( rhs._reader ),
  _log ( rhs._log ),
  _levelRange ( rhs._levelRange ),
  SERIALIZE_XML_INITIALIZER_LIST
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
  _guid.clear();
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
  this->_addMember ( "extents", _extents );
  this->_addMember ( "guid", _guid );
  this->_addMember ( "name", this->_getName() );
  this->_addMember ( "shown", _shown );
  this->_addMember ( new Serialize::XML::ValueMapMember<Alphas> ( "alphas", _alphas ) );
  this->_addMember ( "alpha", _alpha );
  this->_addMember ( "level_range", _levelRange );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the extents.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayer::extents ( const Extents& extents )
{
  Guard guard ( this );
  _extents = extents;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the extents.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayer::Extents RasterLayer::extents() const
{
  Guard guard ( this );
  return _extents;
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
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IRasterLayer::IID:
    return static_cast<Usul::Interfaces::IRasterLayer*> ( this );
  case Usul::Interfaces::ILayer::IID:
    return static_cast<Usul::Interfaces::ILayer*> ( this );
  case Usul::Interfaces::ILayerExtents::IID:
    return static_cast<Usul::Interfaces::ILayerExtents*> ( this );
  case Usul::Interfaces::ISerialize::IID:
    return static_cast<Usul::Interfaces::ISerialize*> ( this );
  case Usul::Interfaces::IRasterAlphas::IID:
    return static_cast<Usul::Interfaces::IRasterAlphas*> ( this );
  case Usul::Interfaces::IClonable::IID:
    return static_cast<Usul::Interfaces::IClonable*> ( this );
  case Usul::Interfaces::ITreeNode::IID:
    return static_cast < Usul::Interfaces::ITreeNode* > ( this );
  case Usul::Interfaces::IBooleanState::IID:
    return static_cast < Usul::Interfaces::IBooleanState* > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the guid for the layer.
//
///////////////////////////////////////////////////////////////////////////////

std::string RasterLayer::guid() const
{
  Guard guard ( this );
  return _guid;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name.
//
///////////////////////////////////////////////////////////////////////////////

std::string RasterLayer::name() const
{
  return BaseClass::name();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the name.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayer::name( const std::string& s )
{
  BaseClass::name ( s );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set show layer.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayer::showLayer ( bool b )
{
  Guard guard ( this );
  _shown = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//   Get show layer.
//
///////////////////////////////////////////////////////////////////////////////

bool RasterLayer::showLayer() const
{
  Guard guard ( this );
  return _shown;
}


///////////////////////////////////////////////////////////////////////////////
//
//   Get the min longitude (ILayerExtents).
//
///////////////////////////////////////////////////////////////////////////////

double RasterLayer::minLon() const
{
  Guard guard ( this );
  return _extents.minLon();
}


///////////////////////////////////////////////////////////////////////////////
//
//   Get the min latitude (ILayerExtents).
//
///////////////////////////////////////////////////////////////////////////////

double RasterLayer::minLat() const
{
  Guard guard ( this );
  return _extents.minLat();
}


///////////////////////////////////////////////////////////////////////////////
//
//   Get the max longitude (ILayerExtents).
//
///////////////////////////////////////////////////////////////////////////////

double RasterLayer::maxLon() const
{
  Guard guard ( this );
  return _extents.maxLon();
}


///////////////////////////////////////////////////////////////////////////////
//
//   Get the max latitude (ILayerExtents).
//
///////////////////////////////////////////////////////////////////////////////

double RasterLayer::maxLat() const
{
  Guard guard ( this );
  return _extents.maxLat();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of children (ITreeNode).
//
///////////////////////////////////////////////////////////////////////////////

unsigned int RasterLayer::getNumChildNodes() const
{
  USUL_TRACE_SCOPE;
  return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the child node (ITreeNode).
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::ITreeNode * RasterLayer::getChildNode ( unsigned int which )
{
  USUL_TRACE_SCOPE;
  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the name (ITreeNode).
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayer::setTreeNodeName ( const std::string & s )
{
  USUL_TRACE_SCOPE;
  this->name( s );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name (ITreeNode).
//
///////////////////////////////////////////////////////////////////////////////

std::string RasterLayer::getTreeNodeName() const
{
  USUL_TRACE_SCOPE;
  return this->name();
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
//  Set the default cache directory.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayer::defaultCacheDirectory ( const std::string& dir )
{
  Usul::Registry::Database::instance()[Detail::RASTER_LAYER_CACHE_DIR] = dir;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the default cache directory.
//
///////////////////////////////////////////////////////////////////////////////

std::string RasterLayer::defaultCacheDirectory()
{
  return Usul::Registry::Database::instance()[Detail::RASTER_LAYER_CACHE_DIR].get ( Usul::File::Temp::directory ( false ) );
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

  // Get the cache directory.
  const std::string cachDir ( this->_cacheDirectory() );
  if ( true == cachDir.empty() )
    return ImagePtr ( 0x0 );

  // Make the directory. Guard it so that it's atomic.
  const std::string baseDir ( this->_baseDirectory ( cachDir, width, height, level ) );
  {
    Guard guard ( this );
    Usul::Functions::safeCallR1 ( Usul::File::make, baseDir, "3201050697" );
  }

  // Make the file name.
  std::string file ( Usul::Strings::format ( baseDir, this->_baseFileName ( extents ), '.', this->_cacheFileExtension() ) );

  // See if the job has been cancelled.
  _checkForCanceledJob ( job );

  // If the file does not exist then return.
  if ( false == Usul::Predicates::FileExists::test ( file ) )
    return ImagePtr ( 0x0 );

  // See if the job has been cancelled.
  _checkForCanceledJob ( job );

  // If the file is empty then remove it and return.
  if ( 0 == Usul::File::size ( file ) )
  {
    this->_logEvent ( Usul::Strings::format ( "Warning 3552986954: Removing empty cached file: ", file ) );
    Usul::File::remove ( file, false, &std::cout );
    return ImagePtr ( 0x0 );
  }

  // Load the file.
  ImagePtr image ( this->_readImageFile ( file ) );

  // See if the job has been cancelled.
  _checkForCanceledJob ( job );

  // If it failed to load...
  if ( false == image.valid() )
  {
    this->_logEvent ( Usul::Strings::format ( "Error 4739433170: Failed to load file: ", file, ", Removing file." ) );
    Usul::File::remove ( file, false, 0x0 );
    return ImagePtr ( 0x0 );
  }

  // Set the file name and return.
  image->setFileName ( file );
  return image;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the string for the value.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  std::string makeString ( unsigned int value )
  {
    std::ostringstream out;
    out << std::setw ( 3 ) << std::setfill ( '0' ) << value;
    return out.str();
  }
  std::string makeString ( double value )
  {
    std::ostringstream out;

    const double positive ( Usul::Math::absolute ( value ) );

    const unsigned long integer ( static_cast < unsigned long > ( positive ) );
    const double decimal ( positive - static_cast < double > ( integer ) );

    const unsigned int bufSize ( 2047 );
    char buffer[bufSize + 1];
    ::sprintf ( buffer, "%0.15f", decimal );

    std::string temp ( buffer );
    boost::replace_first ( temp, "0.", " " );
    boost::trim_left ( temp );

    out << ( ( value >= 0 ) ? 'P' : 'N' ) << std::setfill ( '0' ) << std::setw ( 3 ) << integer << '_' << temp;
    return out.str();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the directory.
//
///////////////////////////////////////////////////////////////////////////////

std::string RasterLayer::_baseDirectory ( const std::string &cacheDir, unsigned int width, unsigned int height, unsigned int level ) const
{
  USUL_TRACE_SCOPE;

  const std::string resolution  ( Usul::Strings::format ( 'W', width, '_', 'H', height ) );
  const std::string levelString ( Usul::Strings::format ( 'L', Helper::makeString ( level ) ) );

  std::string dir ( Usul::Strings::format ( cacheDir, resolution, '/', levelString, '/' ) );
  std::replace ( dir.begin(), dir.end(), '\\', '/' );
  return dir;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the base file name.
//
///////////////////////////////////////////////////////////////////////////////

std::string RasterLayer::_baseFileName ( Extents extents ) const
{
  USUL_TRACE_SCOPE;

  std::string file ( Usul::Strings::format ( 
                     Helper::makeString ( extents.minimum()[0] ), '_', 
                     Helper::makeString ( extents.minimum()[1] ), '_', 
                     Helper::makeString ( extents.maximum()[0] ), '_', 
                     Helper::makeString ( extents.maximum()[1] ) ) );
  std::replace ( file.begin(), file.end(), '.', '-' );

  return file;
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
//  Return the mangled url.
//
///////////////////////////////////////////////////////////////////////////////

std::string RasterLayer::_mangledURL ( const std::string &url )
{
  USUL_TRACE_SCOPE_STATIC;

  // Trim all characters left of the '@' character, if it's found.
  // This will remove embedded username-password pairs.
  std::string mangled ( boost::trim_left_copy_if ( url, std::bind2nd ( std::equal_to<char>(), '@' ) ) );

  boost::replace_first ( mangled, "http://", " " );
  boost::replace_first ( mangled, "https://", " " );
  boost::trim_left ( mangled );

  std::replace ( mangled.begin(), mangled.end(), '@',  '_' );
  std::replace ( mangled.begin(), mangled.end(), ':',  '_' );
  std::replace ( mangled.begin(), mangled.end(), '/',  '_' );
  std::replace ( mangled.begin(), mangled.end(), '\\', '_' );
  std::replace ( mangled.begin(), mangled.end(), '&',  '_' );
  std::replace ( mangled.begin(), mangled.end(), '?',  '_' );
  std::replace ( mangled.begin(), mangled.end(), '.',  '_' );

  return mangled;
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
