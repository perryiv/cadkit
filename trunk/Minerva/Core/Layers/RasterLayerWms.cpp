
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
#include "Usul/Factory/RegisterCreator.h"
#include "Usul/File/Make.h"
#include "Usul/File/Path.h"
#include "Usul/File/Stats.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Math/Absolute.h"
#include "Usul/Network/WMS.h"
#include "Usul/Strings/Format.h"
#include "Usul/Scope/RemoveFile.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"

#include "osgDB/ReadFile"

#include "osg/ref_ptr"
#include "osg/Image"

#include <iomanip>
#include <fstream>

using namespace Minerva::Core::Layers;

USUL_FACTORY_REGISTER_CREATOR ( RasterLayerWms );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayerWms::RasterLayerWms ( const Extents &maxExtents, const std::string &url, const Options &options ) : 
  BaseClass(),
  _url              ( url ),
  _options          ( options ),
  _useNetwork       ( true ),
  _writeFailedFlags ( false ),
  _readFailedFlags  ( false )
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
  _useNetwork ( rhs._useNetwork ),
  _writeFailedFlags ( rhs._writeFailedFlags ),
  _readFailedFlags ( rhs._readFailedFlags )
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

RasterLayerWms::ImagePtr RasterLayerWms::texture ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job *job, IUnknown *caller )
{
  USUL_TRACE_SCOPE;

  // Let the base class go first.
  {
    ImagePtr answer ( BaseClass::texture ( extents, width, height, level, job, caller ) );
    if ( true == answer.valid() )
      return answer;
  }

  // Get url.
  std::string url ( Usul::Threads::Safe::get ( this->mutex(), _url ) );
  if ( true == url.empty() )
    return ImagePtr ( 0x0 );

  // Get options.
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

  // Make the directory. Guard it so that it's atomic.
  const std::string baseDir ( this->_baseDirectory ( cachDir, width, height, level ) );
  {
    Guard guard ( this );
    Usul::Functions::safeCallR1 ( Usul::File::make, baseDir, "1061955244" );
  }

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
  _checkForCanceledJob ( job );

  // Pull it down if we should...
  if ( ( false == Usul::Predicates::FileExists::test ( file ) ) && ( true == Usul::Threads::Safe::get ( this->mutex(), _useNetwork ) ) )
  {
    std::ostream *stream ( 0x0 );
    Usul::Interfaces::IUnknown::QueryPtr caller ( job );
    Usul::Functions::safeCallV1V2V3 ( Usul::Adaptors::memberFunction ( &wms, &Usul::Network::WMS::download ), 5, stream, caller.get(), "2052060829" );
  }

  // See if the job has been cancelled.
  _checkForCanceledJob ( job );

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
  ImagePtr image ( this->_readImageFile ( file, this->_imageReaderGet() ) );

  // See if the job has been cancelled.
  _checkForCanceledJob ( job );

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
//  Get the directory.
//
///////////////////////////////////////////////////////////////////////////////

std::string RasterLayerWms::_cacheDirectory() const
{
  USUL_TRACE_SCOPE;

  std::string urlProxy ( BaseClass::_mangledURL ( Usul::Threads::Safe::get ( this->mutex(), _url ) ) );
  const std::size_t hashValue ( BaseClass::_hashString ( this->_getAllOptions() ) );
  const std::string dir ( BaseClass::_buildCacheDir ( this->cacheDirectory(), urlProxy, hashValue ) );
  return dir;
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
  this->_imageReaderSet ( ReaderPtr ( 0x0 ) );

  // Set the reader.
  this->_imageReaderFind ( this->_cacheFileExtension() );
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


///////////////////////////////////////////////////////////////////////////////
//
//  Get the extension for the cached files.
//
///////////////////////////////////////////////////////////////////////////////

std::string RasterLayerWms::_cacheFileExtension() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // Get the format.
  Options::const_iterator i ( _options.find ( Usul::Network::Names::FORMAT ) );
  const std::string format ( ( _options.end() == i ) ? "" : i->second );

  // Determine file extension.
  std::string ext ( ( format.size() > 6 && '/' == format.at(5) ) ? std::string ( format.begin() + 6, format.end() ) : format );
  ext = ( ( "jpeg" == ext ) ? "jpg" : ext );
  return ext;
}
