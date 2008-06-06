
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
//  Test program for CURL.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
# ifndef NOMINMAX
#  define NOMINMAX
# endif
#endif

#include "WmsJob.h"

#include "Threads/OpenThreads/Mutex.h"
#include "Threads/OpenThreads/Thread.h"

#include "Usul/CommandLine/Arguments.h"
#include "Usul/CommandLine/Parser.h"
#include "Usul/File/Make.h"
#include "Usul/File/Path.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Math/Absolute.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Network/WMS.h"
#include "Usul/Network/Curl.h"
#include "Usul/Strings/Format.h"
#include "Usul/Strings/Split.h"
#include "Usul/Threads/Manager.h"
#include "Usul/Trace/Trace.h"

#include <iostream>
#include <fstream>


///////////////////////////////////////////////////////////////////////////////
//
//  Constants.
//
///////////////////////////////////////////////////////////////////////////////

const std::string BASE_URL      ( "base_url"  );
const std::string BASE_FILE     ( "base_file" );
const std::string LAT_MIN       ( "lat_min"   );
const std::string LAT_MAX       ( "lat_max"   );
const std::string LON_MIN       ( "lon_min"   );
const std::string LON_MAX       ( "lon_max"   );
const std::string NUM_ROWS      ( "num_rows"  );
const std::string NUM_COLS      ( "num_cols"  );
const std::string POOL_SIZE     ( "pool_size" );
const std::string SKIP_EXISTING ( "skip_existing_files" );
const unsigned int LAT_INDEX    ( 0 );
const unsigned int LON_INDEX    ( 1 );


///////////////////////////////////////////////////////////////////////////////
//
//  Program class.
//
///////////////////////////////////////////////////////////////////////////////

class Program
{
public:

  // Typedefs.
  typedef Usul::CommandLine::Options Options;

  // Constructor and destructor.
  Program ( int argc, char **argv, std::ostream *trace );
  ~Program();

  // Run the test.
  void          run();

private:

  char          _getSign ( double value );

  std::string   _makeBoundingBox ( double minLat, double minLon, double maxLat, double maxLon );
  std::string   _makeFileName ( double minLat, double minLon, double maxLat, double maxLon );

  std::string _baseUrl;
  std::string _baseFile;
  Usul::Math::Vec2d _min;
  Usul::Math::Vec2d _max;
  Usul::Math::Vec2ui _size;
  unsigned int _poolSize;
  bool _skipExisting;
  Options _options;
  std::string _dir;
  Usul::Network::Curl::Life _curl;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Program::Program ( int argc, char **argv, std::ostream *trace ) :
  _baseUrl ( "http://onearth.jpl.nasa.gov/wms.cgi" ),
  _baseFile ( "oneearth_jpl_nasa_gov_" ),
  _min ( -90, -180 ),
  _max (  90,  180 ),
  _size ( 11, 34 ),
  _poolSize ( 10 ),
  _skipExisting ( true ),
  _options(),
  _dir ( Usul::File::fullPath ( "./cache/" ) ),
  _curl()
{
  USUL_TRACE_SCOPE;

  // Set mutex thread factory.
  Usul::Threads::Mutex::createFunction ( &Threads::OT::newOpenThreadsMutex );
  Usul::Threads::Manager::instance().factory ( &Threads::OT::newOpenThreadsThread );

  // Set trace file.
  Usul::Trace::Print::init ( trace );

  // Grab the command-line arguments.
  Usul::CommandLine::Arguments::instance().set ( argc, argv );

  // Make sure the cache directory exists.
  Usul::File::make ( _dir );

  // Make sure there are these default options.
  _options.insert ( Usul::Network::Names::LAYERS, _options.get<std::string> ( Usul::Network::Names::LAYERS, "global_mosaic" ) );
  _options.insert ( Usul::Network::Names::STYLES, _options.get<std::string> ( Usul::Network::Names::STYLES, "visual" ) );

  // Get the command-line options.
  Usul::CommandLine::Parser parser ( argv, argv + argc );
  Options options = parser.options();

  // Merge the two sets of options.
  _options.insert ( options );

  // Get members from command-line.
  _baseUrl          = _options.get ( BASE_URL,      _baseUrl          );
  _baseFile         = _options.get ( BASE_FILE,     _baseFile         );
  _min[LAT_INDEX]   = _options.get ( LAT_MIN,       _min[LAT_INDEX]   );
  _min[LON_INDEX]   = _options.get ( LON_MIN,       _min[LON_INDEX]   );
  _max[LAT_INDEX]   = _options.get ( LAT_MAX,       _max[LAT_INDEX]   );
  _max[LON_INDEX]   = _options.get ( LON_MAX,       _max[LON_INDEX]   );
  _size[LAT_INDEX]  = _options.get ( NUM_ROWS,      _size[LAT_INDEX]  );
  _size[LON_INDEX]  = _options.get ( NUM_COLS,      _size[LON_INDEX]  );
  _poolSize         = _options.get ( POOL_SIZE,     _poolSize         );
  _skipExisting     = _options.get ( SKIP_EXISTING, _skipExisting     );

  // Print options.
  std::cout << "Options:" << std::endl;
  for ( Options::Map::const_iterator i = _options.begin(); i != _options.end(); ++i )
    std::cout << i->first << '=' << i->second << std::endl;

  // Remove the url and file from the options.
  _options.remove ( BASE_URL );
  _options.remove ( BASE_FILE );
  _options.remove ( LAT_MIN );
  _options.remove ( LON_MIN );
  _options.remove ( LAT_MAX );
  _options.remove ( LON_MAX );
  _options.remove ( NUM_ROWS );
  _options.remove ( NUM_COLS );
  _options.remove ( POOL_SIZE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Program::~Program()
{
  USUL_TRACE_SCOPE;

  // There should not be any threads running.
  TRACE_AND_PRINT ( "Waiting for all threads to finish...\n" );
  Usul::Threads::Manager::instance().wait();
  TRACE_AND_PRINT ( "All threads have finished.\n" );

  // Set the mutex factory to null so that we can find late uses of it.
  Usul::Threads::Mutex::createFunction ( 0x0 );

  // Clean up the thread manager.
  Usul::Threads::Manager::destroy();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the appropriate character.
//
///////////////////////////////////////////////////////////////////////////////

char Program::_getSign ( double value )
{
  return ( ( value < 0 ) ? 'N' : 'P' );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

std::string Program::_makeBoundingBox ( double minLat, double minLon, double maxLat, double maxLon )
{
  const std::string bbox ( Usul::Strings::format ( minLon, ',', minLat, ',', maxLon, ',', maxLat ) );
  return bbox;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the file name.
//
///////////////////////////////////////////////////////////////////////////////

std::string Program::_makeFileName ( double minLat, double minLon, double maxLat, double maxLon )
{
  std::vector<char> file ( 2048, '\0' );
  ::sprintf ( &file[0], "%s%s%c%10.6f_%c%10.6f_%c%10.6f_%c%10.6f", 
    _dir.c_str(),
    _baseFile.c_str(), 
    this->_getSign ( minLon ),
    Usul::Math::absolute ( minLon ),
    this->_getSign ( minLat ),
    Usul::Math::absolute ( minLat ),
    this->_getSign ( maxLon ),
    Usul::Math::absolute ( maxLon ),
    this->_getSign ( maxLat ),
    Usul::Math::absolute ( maxLat ) );
  file.at ( file.size() - 1 ) = '\0';
  std::replace ( file.begin(), file.end(), ' ', '0' );
  return std::string ( &file[0] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the program.
//
///////////////////////////////////////////////////////////////////////////////

void Program::run()
{
  // Declare a local job manager.
  Usul::Jobs::Manager manager ( _poolSize );

  // Initialize the bounding box minimum.
  double minLat ( _min[LAT_INDEX] );
  double minLon ( _min[LON_INDEX] );

  // Grid size.
  const unsigned int numRows ( _size[LAT_INDEX] + 1 );
  const unsigned int numCols ( _size[LON_INDEX] + 1 );

  // Lat/lon range.
  Usul::Math::Vec2d range ( _max[LAT_INDEX] - _min[LAT_INDEX], _max[LON_INDEX] - _min[LON_INDEX] );

  // Make WMS options from command-line options.
  Usul::Network::WMS::Options options ( _options.begin(), _options.end() );

  // Loop through the rows.
  for ( unsigned int i = 1; i < numRows; ++i )
  {
    // Determine maximum latitude.
    double u ( static_cast<double> ( i ) / static_cast<double> ( numRows - 1 ) );
    double maxLat ( _min[LAT_INDEX] + ( range[LAT_INDEX] * u ) );

    // Loop through the columns.
    for ( unsigned int j = 1; j < numCols; ++j )
    {
      // Determine maximum longitude.
      double v ( static_cast<double> ( j ) / static_cast<double> ( numCols - 1 ) );
      double maxLon ( _min[LON_INDEX] + ( range[LON_INDEX] * v ) );

      // Make the file.
      const std::string file ( this->_makeFileName ( minLat, minLon, maxLat, maxLon ) );

      // Add the bounding box.
      options[Usul::Network::Names::BBOX] = this->_makeBoundingBox ( minLat, minLon, maxLat, maxLon );

      // Make the job.
      WmsJob::RefPtr job ( new WmsJob ( manager.nextJobId(), _baseUrl, file, options ) );

      // Check if it exists.
      const bool exists ( Usul::Predicates::FileExists::test ( job->file() ) );

      // Should we download?
      if ( true == exists && true == _skipExisting )
      {
        std::cout << Usul::Strings::format ( "Skipping existing file: ", job->file(), '\n' ) << std::flush;
      }

      // Otherwise...
      else
      {
        manager.addJob ( job.get() );
      }

      // Reset the min.
      minLon = maxLon;
    }

    // Reset the min.
    minLat = maxLat;
  }

  // Wait until all jobs finish.
  TRACE_AND_PRINT ( "Waiting for jobs to finish...\n" );
  manager.wait();
  TRACE_AND_PRINT ( "All jobs have finished.\n" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the test.
//
///////////////////////////////////////////////////////////////////////////////

void _test ( int argc, char **argv, std::ostream *trace )
{
  USUL_TRACE_SCOPE_STATIC;
  Program program ( argc, argv, trace );
  program.run();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Main function.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
#ifdef _DEBUG

  // Trace file.
  std::ofstream trace ( "trace.csv" );

  // Call test function.
  Usul::Functions::safeCallV1V2V3 ( _test, argc, argv, &trace, "1894923704" );

#else

  // Call test function.
  Usul::Functions::safeCallV1V2V3 ( _test, argc, argv, static_cast<std::ostream *> ( 0x0 ), "3470131298" );

#endif

  return 0;
}
