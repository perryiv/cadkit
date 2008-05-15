
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  WMS layer class.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif

#include "Minerva/Core/Layers/RasterLayerWms.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/App/Application.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Factory/RegisterCreator.h"
#include "Usul/File/Make.h"
#include "Usul/File/Path.h"
#include "Usul/File/Stats.h"
#include "Usul/File/Temp.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Math/Absolute.h"
#include "Usul/Network/WMS.h"
#include "Usul/Registry/Database.h"
#include "Usul/Strings/Format.h"
#include "Usul/Scope/RemoveFile.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"

#include "osgDB/ReadFile"

#include "osg/ref_ptr"
#include "osg/Image"

#include "boost/algorithm/string/trim.hpp"
#include "boost/algorithm/string/replace.hpp"
#include "boost/functional/hash.hpp"

#include <algorithm>
#include <iomanip>
#include <fstream>

using namespace Minerva::Core::Layers;

USUL_FACTORY_REGISTER_CREATOR ( RasterLayerWms );

namespace Detail
{
  const std::string WMS_CACHE_DIR ( "wms_cache_dir" );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayerWms::RasterLayerWms ( const Extents &maxExtents, const std::string &url, const Options &options ) : 
  BaseClass(),
  _url              ( url ),
  _options          ( options ),
  _dir              ( RasterLayerWms::defaultCacheDirectory() ),
  _useNetwork       ( true ),
  _writeFailedFlags ( false ),
  _readFailedFlags  ( false ),
  _reader           ( 0x0 )
{
  USUL_TRACE_SCOPE;

  this->extents ( maxExtents );

  this->_registerMembers();
  
  // Find a reader.
  this->_findImageReader();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayerWms::RasterLayerWms ( const RasterLayerWms& rhs ) :
  BaseClass ( rhs ),
  _url ( rhs._url ),
  _options ( rhs._options ),
  _dir ( rhs._dir ),
  _useNetwork ( rhs._useNetwork ),
  _writeFailedFlags ( rhs._writeFailedFlags ),
  _readFailedFlags ( rhs._readFailedFlags ),
  _reader          ( 0x0 )
{
  this->_registerMembers();
  
  // Find a reader.
  this->_findImageReader();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayerWms::~RasterLayerWms()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &RasterLayerWms::_destroy ), "8458296530" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Register members for serialization.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayerWms::_registerMembers()
{
  // Serialization glue.
  this->_addMember ( "url", _url );
  this->_addMember ( "options", _options );
  this->_addMember ( "use_network", _useNetwork );
  this->_addMember ( "write_failed_flags", _writeFailedFlags );
  this->_addMember ( "read_failed_flags", _readFailedFlags );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayerWms::_destroy()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clone.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* RasterLayerWms::clone() const
{
  Usul::Interfaces::IUnknown::QueryPtr clone ( new RasterLayerWms ( *this ) );
  return clone.release();}


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
//  The name of the "failed" file.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline std::string getFailedFileName ( const std::string &file )
  {
    return Usul::Strings::format ( Usul::File::directory ( file, true ), Usul::File::base ( file ), ".failed" );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the texture.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayerWms::ImagePtr RasterLayerWms::texture ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job *job, IUnknown * )
{
  USUL_TRACE_SCOPE;

  std::string url ( Usul::Threads::Safe::get ( this->mutex(), _url ) );
  if ( true == url.empty() )
    return ImagePtr ( 0x0 );

  Options options ( Usul::Threads::Safe::get ( this->mutex(), _options ) );
  if ( true == options.empty() )
    return ImagePtr ( 0x0 );

  // Add bounding box.  Use Usul convert for full precision.
  options[Usul::Network::Names::BBOX] = Usul::Strings::format ( 
    Usul::Convert::Type<Extents::ValueType,std::string>::convert ( extents.minimum()[0] ), ',', 
    Usul::Convert::Type<Extents::ValueType,std::string>::convert ( extents.minimum()[1] ), ',', 
    Usul::Convert::Type<Extents::ValueType,std::string>::convert ( extents.maximum()[0] ), ',', 
    Usul::Convert::Type<Extents::ValueType,std::string>::convert ( extents.maximum()[1] ) );

  // Add width and height.
  options[Usul::Network::Names::WIDTH]  = Usul::Convert::Type<unsigned int,std::string>::convert ( width  );
  options[Usul::Network::Names::HEIGHT] = Usul::Convert::Type<unsigned int,std::string>::convert ( height );

  // Get the cache directory.
  const std::string cachDir ( this->_cacheDirectory() );

  // Make the directory.
  const std::string baseDir ( this->_baseDirectory ( cachDir, width, height, level ) );
  Usul::Functions::safeCallR1 ( Usul::File::make, baseDir, "1061955244" );

  // Make the base file name.
  std::string file ( Usul::Strings::format ( baseDir, this->_baseFileName ( extents ) ) );

  // Make wms object and ask it for file extension.
  Usul::Network::WMS wms ( url, file, options.begin(), options.end() );
  file = Usul::Strings::format ( file, '.', wms.extension() );

  // Needed below a couple of times.
  const std::string fullUrl ( wms.fullUrl() );

  // Since the directory name was hashed from the url, make sure there is a "read me" file.
  const std::string readMe ( Usul::Strings::format ( cachDir, "ReadMe.txt" ) );
  if ( false == Usul::Predicates::FileExists::test ( readMe ) )
  {
    std::ofstream readMeFile ( readMe.c_str() );
    if ( true == readMeFile.is_open() )
    {
      readMeFile << Usul::Strings::format ( "Width: ", width, "\nHeight: ", height, "\nLevel: ", level, "\nURL: ", fullUrl ) << std::endl;
    }
  }

  // Are we supposed to look for a "failed" file?
  if ( true == Usul::Threads::Safe::get ( this->mutex(), _readFailedFlags ) )
  {
    const std::string failedFile ( Helper::getFailedFileName ( file ) );
    if ( true == Usul::Predicates::FileExists::test ( failedFile ) )
    {
      // The "failed" file exists, so return null.
      return ImagePtr ( 0x0 );
    }
  }

  // See if the job has been cancelled.
  if ( ( 0x0 != job ) && ( true == job->canceled() ) )
    job->cancel();

  // Pull it down if we should...
  if ( ( false == Usul::Predicates::FileExists::test ( file ) ) && ( true == Usul::Threads::Safe::get ( this->mutex(), _useNetwork ) ) )
  {
    std::ostream *stream ( 0x0 );
    Usul::Interfaces::IUnknown::QueryPtr caller ( job );
    Usul::Functions::safeCallV1V2V3 ( Usul::Adaptors::memberFunction ( &wms, &Usul::Network::WMS::download ), 5, stream, caller.get(), "2052060829" );
  }

  // See if the job has been cancelled.
  if ( ( 0x0 != job ) && ( true == job->canceled() ) )
    job->cancel();

  // If the file does not exist then return.
  if ( false == Usul::Predicates::FileExists::test ( file ) )
  {
    std::cout << Usul::Strings::format ( "Error 1276423772: Failed to download. File: ", file, ", URL: ", fullUrl ) << std::endl;
    this->_downloadFailed ( file, fullUrl );
    return ImagePtr ( 0x0 );
  }

  // If the file is empty then remove it and return.
  if ( 0 == Usul::File::size ( file ) )
  {
    std::cout << Usul::Strings::format ( "Error 3244363936: Download file is empty. File: ", file, ", URL: ", fullUrl, ". Removing file." ) << std::endl;
    Usul::File::remove ( file, false, 0x0 );
    this->_downloadFailed ( file, fullUrl );
    return ImagePtr ( 0x0 );
  }

  // Load the file.
  ImagePtr image ( this->_readImageFile ( file ) );

  // See if the job has been cancelled.
  if ( ( 0x0 != job ) && ( true == job->canceled() ) )
    job->cancel();

  // If it failed to load...
  if ( false == image.valid() )
  {
    std::cout << Usul::Strings::format ( "Error 2720181403: Failed to load file: ", file, ", downloaded from URL: ", fullUrl, ". Removing file." ) << std::endl;
    Usul::File::remove ( file, false, 0x0 );
    this->_downloadFailed ( file, fullUrl );
    return ImagePtr ( 0x0 );
  }

  // Set the file name and return.
  image->setFileName ( file );
  return image;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the base file name.
//
///////////////////////////////////////////////////////////////////////////////

std::string RasterLayerWms::_baseFileName ( Extents extents ) const
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

std::string RasterLayerWms::_cacheDirectory() const
{
  USUL_TRACE_SCOPE;

  std::string urlProxy ( Usul::Threads::Safe::get ( this->mutex(), _url ) );
  boost::replace_first ( urlProxy, "http://", " " );
  boost::replace_first ( urlProxy, "https://", " " );
  boost::trim_left ( urlProxy );
  std::replace ( urlProxy.begin(), urlProxy.end(), ':', '_' );
  std::replace ( urlProxy.begin(), urlProxy.end(), '/', '_' );
  std::replace ( urlProxy.begin(), urlProxy.end(), '&', '_' );
  std::replace ( urlProxy.begin(), urlProxy.end(), '?', '_' );
  std::replace ( urlProxy.begin(), urlProxy.end(), '.', '_' );

  boost::hash<std::string> stringHash;
  const std::size_t hashValue ( stringHash ( this->_getAllOptions() ) );

  std::string dir ( Usul::Strings::format ( this->cacheDirectory(), '/', 
                                            Usul::App::Application::instance().program(), '/',
                                            urlProxy, '/', hashValue, '/' ) );
  std::replace ( dir.begin(), dir.end(), '\\', '/' );
  return dir;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the directory.
//
///////////////////////////////////////////////////////////////////////////////

std::string RasterLayerWms::_baseDirectory ( const std::string &cacheDir, unsigned int width, unsigned int height, unsigned int level ) const
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
//  Set the default cache directory.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayerWms::defaultCacheDirectory ( const std::string& dir )
{
  Usul::Registry::Database::instance()[Detail::WMS_CACHE_DIR] = dir;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the default cache directory.
//
///////////////////////////////////////////////////////////////////////////////

std::string RasterLayerWms::defaultCacheDirectory()
{
  return Usul::Registry::Database::instance()[Detail::WMS_CACHE_DIR].get ( Usul::File::Temp::directory ( false ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the base cache directory.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayerWms::cacheDirectory ( const std::string& dir, bool makeDefault )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  
  // Only set the directory if it's not empty.
  if ( false == dir.empty() )
  {
    // Set the directory.
    _dir = dir;
    
    // Make it the default if we should.
    if ( makeDefault )
      RasterLayerWms::defaultCacheDirectory ( _dir );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the base cache directory.
//
///////////////////////////////////////////////////////////////////////////////

std::string RasterLayerWms::cacheDirectory() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _dir;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return a string containing all the options.
//
///////////////////////////////////////////////////////////////////////////////

std::string RasterLayerWms::_getAllOptions() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  std::ostringstream out;
  for ( Options::const_iterator i = _options.begin(); i != _options.end(); ++i )
  {
    const std::string key ( i->first );
    const std::string value ( i->second );
    out << key << '=' << value << '&';
  }

  const std::string result ( out.str() );
  return ( ( result.empty() ) ? "default" : result );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when a download fails.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayerWms::_downloadFailed ( const std::string &file, const std::string &url )
{
  USUL_TRACE_SCOPE;

  bool writeFailed ( Usul::Threads::Safe::get ( this->mutex(), _writeFailedFlags ) );
  if ( true == writeFailed )
  {
    const std::string failedFile ( Helper::getFailedFileName ( file ) );
    std::ofstream out ( failedFile.c_str() );
    out << "Failed to download\nFile: " << file << "\nURL: " << url << std::endl;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the options map.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayerWms::options ( const Options& options )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _options = options;
  
  // Find a reader.
  this->_findImageReader();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the options map.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayerWms::Options RasterLayerWms::options() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _options;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the url.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayerWms::url ( const std::string& url )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _url = url;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the url.
//
///////////////////////////////////////////////////////////////////////////////

std::string RasterLayerWms::url() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _url;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find a reader for our format.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayerWms::_findImageReader()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  
  // Clear the one we have.
  _reader = 0x0;
  
  const std::string format ( _options[Usul::Network::Names::FORMAT] );
  std::string ext ( ( format.size() > 6 && '/' == format.at(5) ) ? std::string ( format.begin() + 6, format.end() ) : format );
  ext = ( ( "jpeg" == ext ) ? "jpg" : ext );
  
  // Typedefs to shorten the lines.
  typedef Usul::Documents::Manager DocManager;
  typedef DocManager::Documents Documents;

  std::cout << "Looking for reader for " << ext << std::endl;
  
  Documents docs ( DocManager::instance().create ( "." + ext, 0x0, true, false ) );
  for ( Documents::const_iterator iter = docs.begin(); iter != docs.end(); ++iter )
  {
    _reader = Usul::Interfaces::IReadImageFile::QueryPtr ( (*iter).get() );

    if ( _reader.valid() )
    {
      std::cout << "Found document " << (*iter)->typeName() << std::endl;
      break;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read an image file.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayerWms::ImagePtr RasterLayerWms::_readImageFile ( const std::string& filename ) const
{
  USUL_TRACE_SCOPE;
  
  IReadImageFile::RefPtr reader ( Usul::Threads::Safe::get ( this->mutex(), _reader ) );
  
  if ( reader.valid() )
    return reader->readImageFile ( filename );
  
  // Fall back on OSG if we don't have a reader.
  return osgDB::readImageFile ( filename );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Deserialize.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayerWms::deserialize ( const XmlTree::Node& node )
{
  USUL_TRACE_SCOPE;
  
  BaseClass::deserialize ( node );
  
  // Find a reader.
  this->_findImageReader();
}
