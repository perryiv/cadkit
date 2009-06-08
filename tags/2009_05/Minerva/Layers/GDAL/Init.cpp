
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/Strings/Format.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/Guard.h"

#include "gdal.h"
#include "ogr_api.h"
#include "cpl_csv.h"

#include <map>
#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
//
//  Initialize Gdal.
//
///////////////////////////////////////////////////////////////////////////////

namespace
{
  const char * buildFilename ( const char* base );
  
  struct Init
  {
    typedef Usul::Threads::Mutex Mutex;
    typedef Usul::Threads::Guard<Mutex> Guard;
    
    ///  Constructor.
    Init() : _filenames(), _mutex()
    {
      ::GDALAllRegister();
      ::OGRRegisterAll();
      
      // Set the callback to create a full path for given filename.
      // Linux works fine, so don't do it there.
      #ifndef __GNUC__
      ::SetCSVFilenameHook ( buildFilename );
      #endif
    }
    
    ///  Destructor.
    ~Init()
    {
      ::GDALDestroyDriverManager();
      ::OGRCleanupAll();
    }
    
    
    /// Build the filename.
    const char * buildFilenameImpl ( const char* base )
    {
      Guard guard ( _mutex );
      
      // See if we have this filename already.
      Filenames::iterator iter ( _filenames.begin() );
      if ( iter == _filenames.end() )
      {
        // Build the filename.
#if __APPLE__
        const std::string path ( "/../Frameworks/GDAL.framework/Resources/gdal/" );
#elif _MSC_VER
        const std::string path ( "/proj4/" );
#else 
        const std::string path ( "/" );
#endif
        const std::string directory ( Usul::Strings::format ( Usul::CommandLine::Arguments::instance().directory(), path, base ) );
        String filename ( directory.begin(), directory.end() );
        filename.push_back ( '\0' );
        
        // Insert the result into the map.
        typedef std::pair<Filenames::iterator, bool> Result;
        Result result ( _filenames.insert ( std::make_pair ( base, filename ) ) );
        
        if ( false == result.second )
          return 0x0;
        
        // Re-set the iterator to our result.
        iter = result.first;
      }
      
      return &( iter->second[0] );
    }
    
  private:
    typedef std::vector<char> String;
    typedef std::map<std::string, String> Filenames;
    
    Filenames _filenames;
    Usul::Threads::Mutex _mutex;
    
  } _init;
  
  
  // Redirect.
  const char* buildFilename ( const char* base )
  {
    return _init.buildFilenameImpl ( base );
  }
}
