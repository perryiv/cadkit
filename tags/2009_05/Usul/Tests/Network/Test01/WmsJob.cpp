
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Test job.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
# ifndef NOMINMAX
#  define NOMINMAX
# endif
#endif

#include "WmsJob.h"

#include "Usul/Math/Vector2.h"
#include "Usul/Strings/Format.h"
#include "Usul/Trace/Trace.h"

#include <string>
#include <vector>


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

WmsJob::WmsJob ( unsigned long id, const std::string &url, const std::string &file, const Options &options ) : 
  BaseClass(), 
  _wms ( url, file, options.begin(), options.end() )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

WmsJob::~WmsJob()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the output file name.
//
///////////////////////////////////////////////////////////////////////////////

std::string WmsJob::file() const
{
  USUL_TRACE_SCOPE;
  const std::string file ( Usul::Strings::format ( _wms.file(), '.', _wms.extension() ) );
  return file;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the job starts.
//
///////////////////////////////////////////////////////////////////////////////

void WmsJob::_started()
{
  USUL_TRACE_SCOPE;

  const std::string file ( this->file() );

  for ( unsigned int i = 0; i < 10; ++i )
  {
    try
    {
      _wms.download ( 100, &std::cout );
      this->_writeGeomFile();
      break;
    }
    catch ( const std::exception &e )
    {
      std::cout << e.what() << std::endl;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the OSSIM geom file.
//
///////////////////////////////////////////////////////////////////////////////

void WmsJob::_writeGeomFile()
{
  USUL_TRACE_SCOPE;

  // Get the lat/lon tie points.
  double lonMin ( 0 ), latMin ( 0 ), lonMax ( 0 ), latMax ( 0 );
  char comma;
  const std::string bbox ( _wms.get<std::string> ( Usul::Network::Names::BBOX, "" ) );
  std::istringstream in ( bbox );
  in >> lonMin >> comma >> latMin >> comma >> lonMax >> comma >> latMax;
  Usul::Math::Vec2d tiePoint ( latMax, lonMin );

  // Get decimals per degree.
  Usul::Math::Vec2d degreesPerPixel 
    ( ( latMax - latMin ) / _wms.get<double> ( Usul::Network::Names::HEIGHT, 1 ), 
      ( lonMax - lonMin ) / _wms.get<double> ( Usul::Network::Names::WIDTH,  1 ) );

  // Write to file.
  const std::string file ( Usul::Strings::format ( _wms.file(), ".geom" ) );
  std::ofstream out ( file.c_str() );
  out << "type: ossimLlxyProjection" << std::endl;
  out << "decimal_degrees_per_pixel_lat: " << degreesPerPixel[0] << std::endl;
  out << "decimal_degrees_per_pixel_lon: " << degreesPerPixel[1] << std::endl;
  out << "tie_point_lat: " << tiePoint[0] << std::endl;
  out << "tie_point_lon: " << tiePoint[1] << std::endl;
  out << "datum: WGE" << std::endl;
}
