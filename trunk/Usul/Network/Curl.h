
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

#include "Usul/Exceptions/Canceled.h"
#include "Usul/Exceptions/TimedOut.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/GUI/IProgressBar.h"
#include "Usul/Interfaces/ICancel.h"
#include "Usul/Interfaces/ICanceledStateGet.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Scope/Reset.h"
#include "Usul/Scope/RemoveFile.h"
#include "Usul/Strings/Format.h"

#include "curl/curl.h"
#include "curl/curlver.h"

#include <string>
#include <fstream>
#include <limits>
#include <vector>
#include <cstring>

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
      this->cleanup();
    }
    void cleanup()
    {
      ::CURL *curlHandle ( _curl );
      _curl = 0x0;

      if ( 0x0 != curlHandle )
      {
        ::curl_easy_cleanup ( curlHandle );
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
  //  Encode a url.
  //
  /////////////////////////////////////////////////////////////////////////////

  static std::string encode ( const std::string& url )
  {
#if LIBCURL_VERSION_MAJOR >= 7 && LIBCURL_VERSION_MINOR >= 15 && LIBCURL_VERSION_PATCH >= 4
    Curl::Handle h;
    char *encodedUrl ( ::curl_easy_escape ( h.handle(), url.c_str(), url.size() ) );
#else
    char *encodedUrl ( ::curl_escape ( url.c_str(), url.size() ) );
#endif
    
    std::string result;
    
    try
    {
      result.assign ( encodedUrl, encodedUrl + ::strlen ( encodedUrl ) );
      ::curl_free ( encodedUrl );
    }
    USUL_DEFINE_SAFE_CALL_CATCH_BLOCKS ( "1112159745" );
    
    return result;
  }

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Download the file.
  //
  /////////////////////////////////////////////////////////////////////////////

  void download ( unsigned int timeoutMilliSeconds, std::ostream *out, const std::string& post )
  {
    // Make sure this is set and unset.
    Usul::Scope::Reset<std::ostream*> scopedOutputStream ( _output, out, 0x0 );

    CURL_OUTPUT << "Downloading '" << _url << "' to file '" << _file.first << " ... " << std::endl;

    // This will remove the file is there's an exception.
    Usul::Scope::RemoveFile removeFile ( _file.first );

    // Need local scope.
    {
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
      this->_check ( ::curl_easy_setopt ( handle.handle(), CURLOPT_FOLLOWLOCATION, true ) );

      // Add timeout if it's valid.
      // I'm not sure when CURLOPT_TIMEOUT_MS was introduced, but it's not in 7.16.
#if LIBCURL_VERSION_MAJOR >= 7 && LIBCURL_VERSION_MINOR > 16
      if ( timeoutMilliSeconds > 0 )
        this->_check ( ::curl_easy_setopt ( handle.handle(), CURLOPT_TIMEOUT_MS, static_cast<long> ( timeoutMilliSeconds ) ) );
#endif

      // Add post data if it's there.
      if ( false == post.empty() )
        this->_check ( ::curl_easy_setopt ( handle.handle(), CURLOPT_POSTFIELDS, post.c_str() ) );

      // Get the data.
      this->_check ( ::curl_easy_perform ( handle.handle() ) );
    }

    // Keep the file.
    removeFile.remove ( false );
  }

private:


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the canceled state.
  //
  /////////////////////////////////////////////////////////////////////////////

  bool _isCanceled()
  {
    Usul::Interfaces::ICanceledStateGet::QueryPtr canceledState ( _caller );
    return ( ( true == canceledState.valid() ) ? canceledState->canceled() : false );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Check the canceled state and cancel (throw) if we should.
  //
  /////////////////////////////////////////////////////////////////////////////

  void _checkCanceledState()
  {
    if ( true == this->_isCanceled() )
    {
      Usul::Interfaces::ICancel::QueryPtr cancelJob ( _caller );
      if ( true == cancelJob.valid() )
      {
        cancelJob->cancel();
      }
    }
  }


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

      // Check to see if we've been canceled.
      if ( true == this->_isCanceled() )
      {
        // Return zero size to stop downloading. Results in CURLE_WRITE_ERROR.
        return 0;
      }
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

    // Check to see if we've been canceled.
    if ( true == this->_isCanceled() )
    {
      return CURLE_ABORTED_BY_CALLBACK;
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
    // Check canceled state and throw if we should.
    this->_checkCanceledState();

    // No error.
    if ( 0 == code )
      return;

    // Convert curl error to a message.
    const std::string error ( &_error[0] );
    const std::string message ( Usul::Strings::format ( 
      ( ( error.empty() ) ? Usul::Strings::format ( "curl error code = ", code ) : error ), 
      ", URL = ", _url ) );

    // Did it time out?
    if ( CURLE_OPERATION_TIMEDOUT == code )
      throw Usul::Exceptions::TimedOut::NetworkDownload ( "Error 1884185898: " + message );

    // It didn't time out so throw a standard exception.
    throw std::runtime_error ( "Error 3886626924: " + message );
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
