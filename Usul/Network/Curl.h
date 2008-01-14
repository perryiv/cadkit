
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
//  Wraps up calls to the Curl library.
//  See http://curl.haxx.se/libcurl/c/libcurl-tutorial.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_NETWORK_CURL_H_
#define _USUL_NETWORK_CURL_H_

#include "Usul/Interfaces/GUI/IProgressBar.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Scope/Reset.h"
#include "Usul/Strings/Format.h"

#include "curl/curl.h"

#include <string>
#include <fstream>
#include <limits>
#include <vector>

#define CURL_OUTPUT if ( 0x0 != _output ) (*_output)
const unsigned int CURL_ERROR_BUFFER_SIZE ( 2 * CURL_ERROR_SIZE );


namespace Usul {
namespace Network {


class Curl
{
public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Typedefs.
  //
  /////////////////////////////////////////////////////////////////////////////

  typedef std::pair < std::string, std::ofstream * > File;
  typedef Usul::Interfaces::IUnknown Unknown;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  Curl ( const std::string &url, const std::string &file, unsigned long totalSize = 0, Unknown *caller = 0x0 ) :
    _url  ( url ),
    _file ( file, 0x0 ),
    _error ( CURL_ERROR_BUFFER_SIZE, '\0' ),
    _totalSize ( 0 ),
    _caller ( caller ),
    _output ( 0x0 )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Copy constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  Curl ( const Curl &curl ) :
    _url  ( curl._url ),
    _file ( curl._file ),
    _error ( CURL_ERROR_BUFFER_SIZE, '\0' ),
    _totalSize ( curl._totalSize ),
    _caller ( curl._caller ),
    _output ( 0x0 )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Assignment.
  //
  /////////////////////////////////////////////////////////////////////////////

  Curl &operator = ( const Curl &curl )
  {
    _url  = curl._url;
    _file = curl._file;
    _totalSize = curl._totalSize;
    _caller = curl._caller;
    _output = 0x0;
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Destructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  ~Curl()
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Nested class to initialize and clean up the curl library.
  //
  /////////////////////////////////////////////////////////////////////////////

  struct Life
  {
    Life()
    {
      ::curl_global_init ( CURL_GLOBAL_ALL );
    }
    ~Life()
    {
      ::curl_global_cleanup();
    }
  };


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Download the file.
  //
  /////////////////////////////////////////////////////////////////////////////

  void download ( std::ostream *out = 0x0 )
  {
    // Make sure this is set and unset.
    Usul::Scope::Reset<std::ostream*> scopedOutputStream ( _output, out, 0x0 );

    CURL_OUTPUT << "Downloading '" << _url << "' to file '" << _file.first << " ... " << std::endl;

    // Get handle.
    Curl::Handle handle;

    // Open file.
    std::ofstream stream ( _file.first.c_str(), std::ofstream::binary | std::ofstream::out );
    if ( false == stream.is_open() )
    {
      throw std::runtime_error ( "Error 2742979881: Failed to open file '" + _file.first + "' for writing" );
    }

    // Make sure this is set and unset.
    Usul::Scope::Reset<File::second_type> scopedFileHandle ( _file.second, &stream, 0x0 );

    // Set properties.
    this->_check ( ::curl_easy_setopt ( handle.handle(), CURLOPT_ERRORBUFFER, &_error[0] ) );
    this->_check ( ::curl_easy_setopt ( handle.handle(), CURLOPT_URL, _url.c_str() ) );
    this->_check ( ::curl_easy_setopt ( handle.handle(), CURLOPT_WRITEDATA, this ) );
    this->_check ( ::curl_easy_setopt ( handle.handle(), CURLOPT_WRITEFUNCTION, &Curl::_writeDataCB ) );
    this->_check ( ::curl_easy_setopt ( handle.handle(), CURLOPT_PROGRESSDATA, this ) );
    this->_check ( ::curl_easy_setopt ( handle.handle(), CURLOPT_PROGRESSFUNCTION, &Curl::_progressCB ) );
    this->_check ( ::curl_easy_setopt ( handle.handle(), CURLOPT_NOPROGRESS, false ) );
    this->_check ( ::curl_easy_setopt ( handle.handle(), CURLOPT_NOSIGNAL, true ) );

    // Get the data.
    this->_check ( ::curl_easy_perform ( handle.handle() ) );
  }

private:


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Wrapper around curl handle.
  //
  /////////////////////////////////////////////////////////////////////////////

  struct Handle
  {
    Handle() : _curl ( ::curl_easy_init() )
    {
      if ( 0x0 == _curl )
      {
        throw std::runtime_error ( "Error 2234813705: Failed to open curl easy handle" );
      }
    }
    ~Handle()
    {
      if ( 0x0 != _curl )
      {
        ::curl_easy_cleanup ( _curl );
      }
    }
    ::CURL *handle()
    {
      return _curl;
    }
  private:
    ::CURL *_curl;
  };


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Called during a download.
  //
  /////////////////////////////////////////////////////////////////////////////

  static size_t _writeDataCB ( void *buffer, size_t sizeOfOne, size_t numElements, void *userData )
  {
    Curl *me ( reinterpret_cast<Curl *> ( userData ) );
    return ( ( 0x0 == me ) ? 0 : me->_writeData ( buffer, sizeOfOne, numElements ) );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Called during a download.
  //
  /////////////////////////////////////////////////////////////////////////////

  size_t _writeData ( void *buffer, size_t sizeOfOne, size_t numElements )
  {
    std::ofstream *file ( _file.second );
    if ( 0x0 == file )
    {
      return 0;
    }

    // Initialize.
    const size_t totalBytes ( sizeOfOne * numElements );
    const char *bytes ( reinterpret_cast<const char *> ( buffer ) );

    // Since size_t is unsigned but std::streamsize is signed, 
    // we have to write in pieces.
    while ( static_cast<size_t> ( ( bytes - ( reinterpret_cast<const char *> ( buffer ) ) ) ) < totalBytes )
    {
      // Determine the max we can write.
      const std::streamsize writeSize ( static_cast<std::streamsize> ( Usul::Math::minimum ( static_cast<size_t> ( std::numeric_limits<std::streamsize>::max() ), totalBytes ) ) );

      // Write the bytes.
      file->write ( bytes, writeSize );

      // Offset the buffer.
      bytes += writeSize;
    }

    // If we get to here then we should have written the entire buffer.
    return totalBytes;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Called during a download to indicate progress.
  //
  /////////////////////////////////////////////////////////////////////////////

  static int _progressCB ( void *userData, double thisDownload, double totalDownloaded, double thisUpload, double totalUploaded )
  {
    Curl *me ( reinterpret_cast<Curl *> ( userData ) );
    return ( ( 0x0 == me ) ? 0 : me->_progress ( thisDownload, totalDownloaded, thisUpload, totalUploaded ) );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Called during a download to indicate progress.
  //
  /////////////////////////////////////////////////////////////////////////////

  int _progress ( double thisDownload, double totalDownloaded, double thisUpload, double totalUploaded )
  {
    if ( thisDownload > 0 )
    {
      CURL_OUTPUT << Usul::Strings::format ( "Download: ", totalDownloaded, " of ", thisDownload, " bytes" );
      Usul::Interfaces::IProgressBar::QueryPtr progress ( _caller );
      if ( true == progress.valid() )
      {
        const double value ( thisDownload / _totalSize );
        progress->updateProgressBar ( static_cast < unsigned int > ( value ) );
      }
    }

    if ( thisUpload > 0 )
    {
      CURL_OUTPUT << Usul::Strings::format ( "Uploaded: ", thisUpload, " of ", totalUploaded, " bytes" ) << std::endl;
      Usul::Interfaces::IProgressBar::QueryPtr progress ( _caller );
      if ( true == progress.valid() )
      {
        const double value ( thisDownload / _totalSize );
        progress->updateProgressBar ( static_cast < unsigned int > ( value ) );
      }
    }

    // Keeps things going. Non-zero return results in CURLE_ABORTED_BY_CALLBACK.
    return 0;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Check return code.
  //
  /////////////////////////////////////////////////////////////////////////////

  void _check ( CURLcode code )
  {
    if ( 0 != code )
    {
      const std::string error ( &_error[0] );
      const std::string message ( Usul::Strings::format ( 
        "Error 1884185898: ", 
        ( ( error.empty() ) ? Usul::Strings::format ( "curl error code = ", code ) : error ), 
        ", URL = ", _url ) );
      throw std::runtime_error ( message );
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Data members.
  //
  /////////////////////////////////////////////////////////////////////////////

  std::string _url;
  File _file;
  std::vector<char> _error;
  unsigned long _totalSize;
  Unknown::QueryPtr _caller;
  std::ostream *_output;
};


} // namespace Curl
} // namespace Usul


#undef CURL_OUTPUT


#endif // _USUL_NETWORK_CURL_H_
