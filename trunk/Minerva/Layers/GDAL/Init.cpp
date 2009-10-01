
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Config.h"

#include "Usul/CommandLine/Arguments.h"
#include "Usul/System/Environment.h"

#include "gdal.h"
#include "ogr_api.h"
#include "cpl_csv.h"

#include "boost/filesystem.hpp"

#include <string>

///////////////////////////////////////////////////////////////////////////////
//
//  Initialize Gdal.
//
///////////////////////////////////////////////////////////////////////////////

namespace
{
  struct Init
  {
    ///  Constructor.
    Init()
    {
      boost::filesystem::path gdalDataDir;

      if ( Usul::System::Environment::has ( MINERVA_DATA_DIR_VARIABLE ) )
      {
        gdalDataDir = Usul::System::Environment::get ( MINERVA_DATA_DIR_VARIABLE );
      }

      else
      {
#if __APPLE__
        const std::string path ( "/../Frameworks/GDAL.framework/Resources/gdal/" );
#elif _MSC_VER
        const std::string path ( "/proj4/" );
#else 
        const std::string path ( "../share/minerva/gdal" );
#endif

        boost::filesystem::path programDir ( Usul::CommandLine::Arguments::instance().directory(), boost::filesystem::no_check );
        gdalDataDir = programDir / path;
      }
      if ( false == Usul::System::Environment::has ( "GDAL_DATA" ) && boost::filesystem::is_directory ( gdalDataDir ) )
      {
        Usul::System::Environment::set ( "GDAL_DATA", gdalDataDir.native_directory_string() );
      }

      /// If there are no drivers for gdal, assume that it hasn't been initialized yet.
      if ( 0 == ::GDALGetDriverCount() )
      {
        ::GDALAllRegister();
      }

      ::OGRRegisterAll();
    }
    
    ///  Destructor.
    ~Init()
    {
      ::GDALDestroyDriverManager();
      ::OGRCleanupAll();
    }
    
  } _init;
}
