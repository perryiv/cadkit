
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class for detecting and using an InterSense tracker.
//
///////////////////////////////////////////////////////////////////////////////

#include "InterSense/Tracker.h"
#include "InterSense/Functions.h"
#include "InterSense/Exceptions.h"
#include "InterSense/Defines.h"

#include "isense/isense.h"

#include "boost/mpl/assert_is_same.hpp"

#include <assert.h>

using namespace InterSense;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Tracker::Tracker ( 
  Port port, 
  Station::OrientationFormat of, 
  Station::ID numStations, 
  bool verbose ) : 
  Referenced(),
  _handle ( ::ISD_OpenTracker ( 0x0, port, 0, 0 ) ),
  _port ( _port ),
  _verbose ( verbose ),
  _stations()
{
  BOOST_MPL_ASSERT_IS_SAME ( Port,   ::DWORD );
  BOOST_MPL_ASSERT_IS_SAME ( Handle, ::ISD_TRACKER_HANDLE );

  // Make sure we connected.
  if ( false == isHandle ( _handle ) )
    throw FailedToConnect();

  // Initialize the tracker-info struct.
  ITL_DECLARE_AND_INIT ( ISD_TRACKER_INFO_TYPE, tracker );

  // Get the tracker's configuration info.
  if ( FALSE == ::ISD_GetTrackerConfig ( _handle, &tracker, verbose ) )
    throw FailedToDetectTrackerConfig();

  // Set the port.
  _port = tracker.Port;

  // Make sure we have the right kind of tracker.
  if ( ISD_IS900 != tracker.TrackerModel )
    throw UnsupportedTrackerModel();

  // Initialize the stations.
  numStations = this->_initStations ( numStations );

  // Configure all the stations to default settings.
  for ( Station::ID i = 0; i < numStations; ++i )
  {
    _stations[i]->buttonAndJoystickInput ( true );
    _stations[i]->orientationFormat ( of );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Tracker::~Tracker()
{
  if ( isHandle ( _handle ) )
    ::ISD_CloseTracker ( _handle );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the stations.
//
///////////////////////////////////////////////////////////////////////////////

Station::ID Tracker::_initStations ( Station::ID numToQuery )
{
  BOOST_STATIC_ASSERT ( sizeof ( Station::ID ) <= sizeof ( Stations::size_type ) );
  assert ( 0 == this->numStations() );
  assert ( isHandle ( _handle ) );

  // Initialize the station-info struct.
  ITL_DECLARE_AND_INIT ( ISD_STATION_INFO_TYPE, station );

  // Query the stations. Note: the index starts at one.
  while ( ( numToQuery-- ) && 
          ( TRUE == ::ISD_GetStationConfig ( _handle, &station, this->numStations() + 1, _verbose ) ) )
  {
    // Put the new station in our list.
    _stations.push_back ( new Station ( this, this->numStations() ) );
  }

  // Return the number of stations.
  return this->numStations();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of stations.
//
///////////////////////////////////////////////////////////////////////////////

Station::ID Tracker::numStations() const
{
  BOOST_STATIC_ASSERT ( sizeof ( Station::ID ) <= sizeof ( Stations::size_type ) );
  assert ( _stations.size() <= ISD_MAX_STATIONS );

  return (Station::ID) _stations.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update all the stations.
//
///////////////////////////////////////////////////////////////////////////////

void Tracker::update()
{
  assert ( isHandle ( _handle ) );

  // Initialize the station-info struct.
  ITL_DECLARE_AND_INIT ( ISD_TRACKER_DATA_TYPE, data );

  // Get the station's data.
  Detail::getStationData ( *this, data );

  // Tell all the stations to update.
  Station::ID numStations ( this->numStations() );
  for ( Station::ID i = 0; i < numStations; ++i )
    this->station ( i )->_update ( &(data.Station[i]) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the i'th station.
//
///////////////////////////////////////////////////////////////////////////////

const Station* Tracker::station ( Station::ID i ) const
{
  assert ( i < _stations.size() );
  assert ( i < ISD_MAX_STATIONS );
  return _stations[i].get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the i'th station.
//
///////////////////////////////////////////////////////////////////////////////

Station* Tracker::station ( Station::ID i )
{
  assert ( i < _stations.size() );
  assert ( i < ISD_MAX_STATIONS );
  return _stations[i].get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the orientation format.
//
///////////////////////////////////////////////////////////////////////////////

void Tracker::orientationFormat ( Station::OrientationFormat format )
{
  for ( Stations::iterator i = _stations.begin(); i != _stations.end(); ++i )
    (*i)->orientationFormat ( format );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Turn on/off the button and joystick input.
//
///////////////////////////////////////////////////////////////////////////////

void Tracker::buttonAndJoystickInput ( bool state )
{
  for ( Stations::iterator i = _stations.begin(); i != _stations.end(); ++i )
    (*i)->buttonAndJoystickInput ( state );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Specify the length units.
//
///////////////////////////////////////////////////////////////////////////////

void Tracker::lengthUnits ( Station::Units units )
{
  for ( Stations::iterator i = _stations.begin(); i != _stations.end(); ++i )
    (*i)->lengthUnits ( units );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the origin. This will vary depending on where the tracker is 
//  (i.e., the bars hanging from the ceiling).
//
///////////////////////////////////////////////////////////////////////////////

void Tracker::origin ( float x, float y, float z )
{
  for ( Stations::iterator i = _stations.begin(); i != _stations.end(); ++i )
    (*i)->origin ( x, y, z );
}
