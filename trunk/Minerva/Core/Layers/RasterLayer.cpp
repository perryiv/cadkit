
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Layers/RasterLayer.h"

#include "Usul/App/Application.h"
#include "Usul/Documents/Manager.h"
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

#include "osgDB/ReadFile"
#include "osgDB/WriteFile"

#include "osg/Image"

#include "boost/algorithm/string/trim.hpp"
#include "boost/algorithm/string/replace.hpp"
#include "boost/functional/hash.hpp"

#include <algorithm>

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

std::string RasterLayer::cacheDirectory() const
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

void RasterLayer::cacheDirectory ( const std::string& dir, bool makeDefault )
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

  // Make the base file name.
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
    std::cout << Usul::Strings::format ( "Warning 3552986954: Removing empty cached file: ", file ) << std::endl;
    Usul::File::remove ( file, false, &std::cout );
    return ImagePtr ( 0x0 );
  }

  // Load the file.
  ImagePtr image ( this->_readImageFile ( file, this->_imageReaderGet() ) );

  // See if the job has been cancelled.
  _checkForCanceledJob ( job );

  // If it failed to load...
  if ( false == image.valid() )
  {
    std::cout << Usul::Strings::format ( "Error 4739433170: Failed to load file: ", file, ", Removing file." ) << std::endl;
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
//  Get the extension for the cached files.
//
///////////////////////////////////////////////////////////////////////////////

std::string RasterLayer::_cacheFileExtension() const
{
  USUL_TRACE_SCOPE;
  return std::string ( "" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Check if the job has been marked as canceled. If so, cancel it, which 
//  should throw an exception.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayer::_checkForCanceledJob ( Usul::Jobs::Job *job )
{
  USUL_TRACE_SCOPE;
  if ( ( 0x0 != job ) && ( true == job->canceled() ) )
    job->cancel();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read an image file.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayer::ImagePtr RasterLayer::_readImageFile ( const std::string &file, ReaderPtr reader )
{
  USUL_TRACE_SCOPE;

  // Try to use the given reader.
  if ( reader.valid() )
    return reader->readImageFile ( file );

  // Fall back on OSG if we don't have a reader.
  return osgDB::readImageFile ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the image reader.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayer::ReaderPtr RasterLayer::_imageReaderGet()
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
  typedef Usul::Documents::Manager DocManager;
  typedef DocManager::Documents Documents;

  // Get the list of documents that can read this format.
  Documents docs ( DocManager::instance().create ( "." + ext, 0x0, true, false ) );

  // Find the one that also implements the needed interface.
  for ( Documents::const_iterator iter = docs.begin(); iter != docs.end(); ++iter )
  {
    IReadImageFile::QueryPtr reader ( iter->get() );
    if ( reader.valid() )
    {
      // Set the new image reader.
      this->_imageReaderSet ( reader.get() );
      break;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the mangled utl.
//
///////////////////////////////////////////////////////////////////////////////

std::string RasterLayer::_mangledURL ( const std::string &url )
{
  USUL_TRACE_SCOPE_STATIC;

  std::string mangled ( url );

  boost::replace_first ( mangled, "http://", " " );
  boost::replace_first ( mangled, "https://", " " );
  boost::trim_left ( mangled );

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
//  Write the image to cache.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayer::_writeImageToCache ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level, ImagePtr image )
{
  USUL_TRACE_SCOPE;

  // Check the image.
  if ( false == image.valid() )
    return;

  // Get the cache directory.
  const std::string cachDir ( this->_cacheDirectory() );
  if ( true == cachDir.empty() )
    return;

  // Build the file name.
  const std::string baseDir ( this->_baseDirectory ( cachDir, width, height, level ) );
  std::string cacheFile ( Usul::Strings::format ( baseDir, this->_baseFileName ( extents ), '.', this->_cacheFileExtension() ) );

  // Write the file.
  osgDB::writeImageFile ( *image, cacheFile );
}
