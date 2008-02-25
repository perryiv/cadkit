
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

#include "StarSystem/RasterLayerWms.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/App/Application.h"
#include "Usul/Factory/RegisterCreator.h"
#include "Usul/File/Make.h"
#include "Usul/File/Path.h"
#include "Usul/File/Stats.h"
#include "Usul/File/Temp.h"
#include "Usul/Functions/SafeCall.h"
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

using namespace StarSystem;

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
  _readFailedFlags  ( false )
{
  USUL_TRACE_SCOPE;

  this->extents ( maxExtents );

  this->_registerMembers();
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
  _readFailedFlags ( rhs._readFailedFlags )
{
  this->_registerMembers();
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

RasterLayerWms::ImagePtr RasterLayerWms::texture ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job *, IUnknown *caller )
{
  USUL_TRACE_SCOPE;

  std::string url ( Usul::Threads::Safe::get ( this->mutex(), _url ) );
  if ( true == url.empty() )
    return ImagePtr ( 0x0 );

  Options options ( Usul::Threads::Safe::get ( this->mutex(), _options ) );
  if ( true == options.empty() )
    return ImagePtr ( 0x0 );

  // Add options.
  options[Usul::Network::Names::BBOX] = Usul::Strings::format ( extents.minimum()[0], ',', extents.minimum()[1], ',', extents.maximum()[0], ',', extents.maximum()[1] );
  options[Usul::Network::Names::WIDTH] = Usul::Strings::format ( width );
  options[Usul::Network::Names::HEIGHT] = Usul::Strings::format ( height );

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

  // Get the flags.
  bool useNetwork    ( false );
  bool writeFailed   ( false );
  bool lookForFailed ( false );
  {
    Guard guard ( this );
    useNetwork    = _useNetwork;
    writeFailed   = _writeFailedFlags;
    lookForFailed = _readFailedFlags;
  }

  // Are we supposed to look for a "failed" file?
  if ( true == lookForFailed )
  {
    const std::string failedFile ( Helper::getFailedFileName ( file ) );
    if ( true == Usul::Predicates::FileExists::test ( failedFile ) )
    {
      // The "failed" file exists, so return null.
      return 0x0;
    }
  }

  // Pull it down if we should...
  if ( ( false == Usul::Predicates::FileExists::test ( file ) ) && ( true == useNetwork ) )
  {
    std::ostream *stream ( 0x0 );
    Usul::Interfaces::IUnknown::RefPtr caller ( 0x0 );
    Usul::Functions::safeCallV1V2V3 ( Usul::Adaptors::memberFunction ( &wms, &Usul::Network::WMS::download ), 5, stream, caller.get(), "2052060829" );
  }

  // Initialize.
  ImagePtr image ( 0x0 );

  // Load the file iff it exists and it's non-zero size.
  if ( ( true == Usul::Predicates::FileExists::test ( file ) ) && ( Usul::File::size ( file ) > 0 ) )
  {
    image = osgDB::readImageFile ( file );
  }

  // If it failed to load then delete the file.
  if ( false == image.valid() )
  {
    std::cout << Usul::Strings::format ( "Error 2720181403: Failed to load file: ", file, ", so removing it. URL = ", fullUrl ) << std::endl;
    Usul::File::remove ( file, false );

    // Write a "failed" file if we're supposed to.
    if ( true == writeFailed )
    {
      const std::string failedFile ( Helper::getFailedFileName ( file ) );
      std::ofstream out ( failedFile.c_str() );
      out << "Failed to download file: \n" << file << std::endl;
    }
  }

  // Set the file name.
  else
  {
    image->setFileName ( file );
  }

  // Return image, which may be null.
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
