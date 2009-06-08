
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Misc functions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _INTERSENSE_TRACKER_FUNCTIONS_H_
#define _INTERSENSE_TRACKER_FUNCTIONS_H_

#ifndef _COMPILING_INTERSENSE_LIBRARY
# error "This header file is only used for compiling the library."
# error "It should not be included in client code."
#endif

#include "InterSense/Tracker.h"
#include "InterSense/Exceptions.h"

#include "isense/isense.h"


namespace InterSense {
namespace Detail {


///////////////////////////////////////////////////////////////////////////////
//
//  Get the data from all the stations.
//
///////////////////////////////////////////////////////////////////////////////

inline void getStationData ( const Tracker &tracker, ISD_TRACKER_DATA_TYPE &data )
{
  assert ( Tracker::isHandle ( tracker.handle() ) );

  // Get the data for all stations.
  if ( FALSE == ::ISD_GetData ( tracker.handle(), &data ) )
    throw FailedToGetTrackerData();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the station's configuration.
//
///////////////////////////////////////////////////////////////////////////////

inline void getStationConfig ( const Tracker &tracker, Station::ID i, ISD_STATION_INFO_TYPE &station )
{
  assert ( i < ISD_MAX_STATIONS );
  assert ( Tracker::isHandle ( tracker.handle() ) );

  // Get the configuration of the i'th station.
  if ( FALSE == ::ISD_GetStationConfig ( tracker.handle(), &station, i + 1, tracker.verbose() ) )
    throw FailedToDetectStationConfig();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the station's configuration.
//
///////////////////////////////////////////////////////////////////////////////

inline void setStationConfig ( Tracker &tracker, Station::ID i, ISD_STATION_INFO_TYPE &station )
{
  assert ( i < ISD_MAX_STATIONS );
  assert ( Tracker::isHandle ( tracker.handle() ) );

  // Ask the station to change.
  if ( FALSE == ::ISD_SetStationConfig ( tracker.handle(), &station, i + 1, tracker.verbose() ) )
    throw FailedToConfigureStation();
}


}; // namespace Detail
}; // namespace InterSense


#endif // _INTERSENSE_TRACKER_FUNCTIONS_H_
