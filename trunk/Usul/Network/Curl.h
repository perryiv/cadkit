
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

#include "Usul/Math/MinMax.h"
#include "Usul/Strings/Format.h"

#include "curl/curl.h"

#include <string>
#include <fstream>
#include <limits>
#include <vector>

#define CURL_OUTPUT if ( 0x0 != out ) (*out)
const unsigned int CURL_ERROR_BUFFER_SIZE ( 2 * CURL_ERROR_SIZE );


namespace Usul {
namespace Network {


class Curl
{
public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  Curl ( const std::string &url, const std::string &file ) :
    _url  ( url ),
    _file ( file ),
    _error ( CURL_ERROR_BUFFER_SIZE, '\0' )
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
    _error ( CURL_ERROR_BUFFER_SIZE, '\0' )
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
  //  Nested class to initialize the clean up the curl library.
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
    CURL_OUTPUT << "Downloading '" << _url << "' to file '" << _file << " ... " << std::endl;

    // Get handle.
    Curl::Handle handle;

    // Open file.
    std::ofstream file ( _file.c_str(), std::ofstream::binary | std::ofstream::out );
    if ( false == file.is_open() )
    {
      throw std::runtime_error ( "Error 2742979881: Failed to open file '" + _file + "' for writing" );
    }

    // Set properties.
    this->_check ( ::curl_easy_setopt ( handle.handle(), CURLOPT_ERRORBUFFER, &_error[0] ) );
    this->_check ( ::curl_easy_setopt ( handle.handle(), CURLOPT_URL, _url.c_str() ) );
    this->_check ( ::curl_easy_setopt ( handle.handle(), CURLOPT_WRITEDATA, &file ) );
    this->_check ( ::curl_easy_setopt ( handle.handle(), CURLOPT_WRITEFUNCTION, &Curl::_writeDataCB ) );
    this->_check ( ::curl_easy_setopt ( handle.handle(), CURLOPT_NOSIGNAL, TRUE ) );

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
    std::ofstream *file ( reinterpret_cast<std::ofstream *> ( userData ) );
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
  //  Check return code.
  //
  /////////////////////////////////////////////////////////////////////////////

  void _check ( CURLcode code )
  {
    if ( 0 != code )
    {
      const std::string error ( &_error[0] );
      const std::string message ( "Error 1884185898: " + ( ( error.empty() ) ? Usul::Strings::format ( "curl error code = ", code ) : error ) );
      throw std::runtime_error ( message );
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Data members.
  //
  /////////////////////////////////////////////////////////////////////////////

  std::string _url;
  std::string _file;
  std::vector<char> _error;
};


} // namespace Curl
} // namespace Usul


#undef CURL_OUTPUT


#endif // _USUL_NETWORK_CURL_H_
