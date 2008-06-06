
#include "InterSense/Tracker.h"
#include "InterSense/Exceptions.h"

#include "boost/intrusive_ptr.hpp"

#include <iostream>
#include <iomanip>
#include <string>
#include <unistd.h>

using namespace InterSense;


const char *makeBinaryNumber ( unsigned int num )
{
  const unsigned int size ( Station::NUM_BUTTONS );
  static char buf[size+1];
  for ( unsigned int i = 0; i < size; ++i )
  {
    unsigned int bit ( 1 << i );
    buf[i] = ( ( num & bit ) == bit ) ? '1' : '0';
  }
  buf[size] = '\0';
  return (const char *) buf;
}


void test ( Tracker::Port port, Station::ID numStationsToQuery, Station::OrientationFormat of )
{
  // Declare a tracker.
  boost::intrusive_ptr<Tracker> tracker ( new Tracker ( port, of, numStationsToQuery ) );

  // See how many stations there are.
  Station::ID numStations ( tracker->getNumStations() );

  // Formatting.
  const char *format = "%10.4f";
  const char *angles = ( ( Station::EULER == of ) ? "euler" : "quat" );

  // Loop forever.
  while ( true )
  {
    // Print the number of stations.
    ::printf ( "Number of stations = %d\n", numStations );

    // Tell the tracker to update itself.
    tracker->update();

    // For each station...
    for ( Station::ID i = 0; i < numStations; ++i )
    {
      ::printf ( "%d: ", i );

      // Get the i'th station.
      Tracker::StationPtr station = tracker->getStation ( i );

      // Get the data.
      const gmtl::Vec3f &position    = station->position();
      const gmtl::Vec4f &orientation = station->orientation();
      const gmtl::Vec2f &stick       = station->joystick();
      unsigned int buttons           = station->buttons();

      // Print the station's data.
      ::printf ( "  pos: " ); 
      ::printf ( format, position[0] );
      ::printf ( format, position[1] );
      ::printf ( format, position[2] );
      ::printf ( "  %s: ", angles );
      ::printf ( format, orientation[0] );
      ::printf ( format, orientation[1] );
      ::printf ( format, orientation[2] );
      ::printf ( format, orientation[3] );
      ::printf ( "  buttons: " );
      ::printf ( "%s", makeBinaryNumber ( station->buttons() ) );
      ::printf ( "  stick: " ); 
      ::printf ( format, stick[0] );
      ::printf ( format, stick[1] );
      ::printf ( "\n" );
    }

    // Let's see it!
    ::fflush ( stdout );

    // Wait...
    ::usleep ( 1000 );
  }
}


int main ( int argc, char **argv )
{
  if ( 4 != argc )
  {
    std::cout << "Usage: " << argv[0] << " <port number> <num stations to query> <'euler' or 'quat'>" << std::endl;
    std::cout << "\tNote: pass 0 to query all ports" << std::endl;
    return 0;
  }

  // Get the arguments.
  Tracker::Port port ( ::atoi ( argv[1] ) );
  Station::ID num    ( ::atoi ( argv[2] ) );
  std::string angle  ( argv[3] );
  Station::OrientationFormat format ( ( "euler" == angle ) ? Station::EULER : Station::QUATERNION );

  // The tracker library throws exceptions.
  try
  {
    test ( port, num, format );
  }

  // Did we fail to connect?
  catch ( const FailedToConnect &e )
  {
    std::cout << "No tracker found on port " << port << std::endl;
  }

  // Is the tracker one we recognize?
  catch ( const UnsupportedTrackerModel &e )
  {
    std::cout << "Unsupported tracker model." << std::endl;
  }

  // Did we detect the tracker configuration?
  catch ( const FailedToDetectTrackerConfig &e )
  {
    std::cout << "Error detecting tracker configuration." << std::endl;
  }

  // Did we fail to detect a station's configuration?
  catch ( const FailedToDetectStationConfig &e )
  {
    std::cout << "Error detecting station configuration." << std::endl;
  }

  // Did we fail to configure a station?
  catch ( const FailedToConfigureStation &e )
  {
    std::cout << "Error configuring tracker station." << std::endl;
  }

  // Did we specify unknown unit
  catch ( const UnknownUnits &e )
  {
    std::cout << "Unknown units specified." << std::endl;
  }

  // Did we fail to get tracker data?
  catch ( const FailedToGetTrackerData &e )
  {
    std::cout << "Failed to get tracker data." << std::endl;
  }

  // Did we forget one?
  catch ( const TrackerException &e )
  {
    std::cout << "Generic tracker exception." << std::endl;
  }

  // Unknown exceptions.
  catch ( const std::exception &e )
  {
    std::cout << "Standard exception: " << e.what() << std::endl;
  }

  // Unknown exceptions.
  catch ( ... )
  {
    std::cout << "Unknown exception." << std::endl;
  }

  return 0;
}
