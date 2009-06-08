
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
//  Wraps up calls to the WMS servers.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_NETWORK_WMS_H_
#define _USUL_NETWORK_WMS_H_

#include "Usul/Convert/Convert.h"
#include "Usul/File/Remove.h"
#include "Usul/Interfaces/ICancel.h"
#include "Usul/Interfaces/ICanceledStateGet.h"
#include "Usul/Network/Curl.h"
#include "Usul/Network/Names.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Strings/Convert.h"
#include "Usul/Strings/Format.h"
#include "Usul/System/Sleep.h"

#include <map>
#include <sstream>
#include <string>

#define WMS_OUTPUT if ( 0x0 != out ) (*out)


namespace Usul {
namespace Network {


///////////////////////////////////////////////////////////////////////////////
//
//  The WMS class.
//
///////////////////////////////////////////////////////////////////////////////

class WMS
{
public:

  // Useful typedefs.
  typedef std::map<std::string,std::string> Options;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  WMS ( const std::string &url, const std::string &file ) :
    _url ( url ),
    _file ( file ),
    _options()
  {
    this->defaults();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class Itr > WMS ( const std::string &url, const std::string &file, Itr first, Itr last ) :
    _url ( url ),
    _file ( file ),
    _options()
  {
    // Assign defaults first.
    this->defaults();

    // Now set given options.
    for ( Itr i = first; i != last; ++i )
    {
      const std::string name  ( i->first  );
      const std::string value ( i->second );
      _options[name] = value;
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Copy constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  WMS ( const WMS &wms ) :
    _url ( wms._url ),
    _file ( wms._file ),
    _options ( wms._options )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Assignment.
  //
  /////////////////////////////////////////////////////////////////////////////

  WMS& operator = ( const WMS &wms )
  {
    _url = wms._url;
    _file = wms._file;
    _options = wms._options;
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Destructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  ~WMS()
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Download the file.
  //
  /////////////////////////////////////////////////////////////////////////////

  void download ( unsigned int timeoutMilliSeconds, unsigned int maxNumAttempts = 10, std::ostream *out = 0x0, Usul::Interfaces::IUnknown *caller = 0x0 )
  {
		// Get the requested extension.
		const std::string extension ( Usul::File::extension ( _file ) );

    // Get file name.
		const std::string file ( ( true == extension.empty() ) ? Usul::Strings::format ( _file, '.', this->extension() ) : _file );

    // Get the full url.
    const std::string url ( this->fullUrl() );

    // Estimate the size of the download assuming no compression and 4 color 
    // channels, which should be more than the actual file size.
    typedef Usul::Convert::Type<std::string,Usul::Types::Uint32> Converter;
    unsigned long width  ( Converter::convert ( _options[Usul::Network::Names::WIDTH]  ) );
    unsigned long height ( Converter::convert ( _options[Usul::Network::Names::HEIGHT] ) );
    const unsigned long totalSize ( width * height * 4 );

    // Used below.
    const std::string xml ( "<?xml" );
    bool success ( false );

    for ( unsigned int i = 0; i < maxNumAttempts; ++i )
    {
      WMS_OUTPUT << Usul::Strings::format ( "Attempt: ", i, ", file: ", file, ", url = ", url, '\n' ) << std::flush;

      // Use curl to download the file.
      Usul::Network::Curl curl ( url, file, totalSize, caller );
      curl.download ( timeoutMilliSeconds, 0x0, std::string() );

      // Check for file existance.
      std::ifstream in ( file.c_str(), std::ifstream::binary | std::ifstream::in );
      if ( true == in.is_open() )
      {
        // Make sure the file is not xml
        std::vector<char> header ( xml.size() + 1, '\0' );
        in.read ( &header[0], static_cast<std::streamsize> ( xml.size() ) );
        const int compare ( ::strcmp ( &header[0], xml.c_str() ) );
        if ( 0 != compare )
        {
          // This is not an xml file, so assume we got the desired file.
          success = true;
          break;
        }
      }

      // Check to see if we've been canceled.
      Usul::Interfaces::ICanceledStateGet::QueryPtr canceledState ( caller );
      if ( ( true == canceledState.valid() ) && ( true == canceledState->canceled() ) )
      {
        Usul::Interfaces::ICancel::QueryPtr cancelJob ( caller );
        if ( true == cancelJob.valid() )
        {
          cancelJob->cancel();
        }
      }

      // If we get to here then sleep to give the server a chance to recover.
      Usul::System::Sleep::milliseconds ( 500 );
    }

    // If it didn't work...
    if ( false == success )
    {
      Usul::File::remove ( file );
      throw std::runtime_error ( "Error 1822747149: failed to download: " + url );
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the option.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class T > T get ( const std::string &name, const T &defaultValue ) const
  {
    Options::const_iterator i ( _options.find ( name ) );
    return ( ( _options.end() == i ) ? defaultValue : Usul::Strings::fromString<T> ( i->second ) );
  }
  std::string get ( const std::string &name, const std::string &defaultValue ) const
  {
    Options::const_iterator i ( _options.find ( name ) );
    return ( ( _options.end() == i ) ? defaultValue : i->second );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set the option.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class T > void set ( const std::string &name, const T &value )
  {
    _options[name] = Usul::Strings::toString ( value );
  }
  template < class T > void set ( const std::string &name, const std::string &value )
  {
    _options[name] = value;
  }
  template < class Itr > void set ( Itr first, Itr last )
  {
    _options.insert ( first, last );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get/set the option.
  //
  /////////////////////////////////////////////////////////////////////////////

  std::string &operator[] ( const std::string &name )
  {
    return _options[name];
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Clear all options.
  //
  /////////////////////////////////////////////////////////////////////////////

  void clear()
  {
    _options.clear();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set default options.
  //
  /////////////////////////////////////////////////////////////////////////////

  void defaults()
  {
    this->set ( Names::REQUEST, "GetMap"     );
    this->set ( Names::FORMAT,  "image/jpeg" );
    this->set ( Names::SRS,     "EPSG:4326"  );
    this->set ( Names::STYLES,  ""  );

    this->set<unsigned long> ( Names::WIDTH,  1024 );
    this->set<unsigned long> ( Names::HEIGHT, 1024 );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the base url.
  //
  /////////////////////////////////////////////////////////////////////////////

  std::string baseUrl() const
  {
    return _url;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the full url.
  //
  /////////////////////////////////////////////////////////////////////////////

  static std::string fullUrl ( const std::string &baseUrl, const Options &options )
  {
    std::ostringstream url;
    url << baseUrl;
    for ( Options::const_iterator i = options.begin(); i != options.end(); ++i )
    {
      const std::string name  ( i->first  );
      const std::string value ( i->second );
      url << ( ( options.begin() == i ) ? '?' : '&' ) << name << '=' << value;
    }
    return url.str();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the full url.
  //
  /////////////////////////////////////////////////////////////////////////////

  std::string fullUrl() const
  {
    return Usul::Network::WMS::fullUrl ( _url, _options );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the file.
  //
  /////////////////////////////////////////////////////////////////////////////

  std::string file() const
  {
    return _file;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the file extension.
  //
  /////////////////////////////////////////////////////////////////////////////

  std::string extension() const
  {
    // Determine file extension by skipping "image/".
    const std::string format ( this->get ( Names::FORMAT, std::string ( "image/jpeg" ) ) );
    std::string ext ( ( format.size() > 6 && '/' == format.at(5) ) ? std::string ( format.begin() + 6, format.end() ) : format );
    ext = ( ( "jpeg" == ext ) ? "jpg" : ext );
    return ext;
  }

private:

  std::string _url;
  std::string _file;
  Options _options;
};


} // namespace WMS
} // namespace Usul


#undef WMS_OUTPUT


#endif // _USUL_NETWORK_WMS_H_
