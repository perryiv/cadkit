
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

#ifndef _INTERSENSE_TRACKER_DEVICE_CLASS_H_
#define _INTERSENSE_TRACKER_DEVICE_CLASS_H_

#include "InterSense/Station.h"

#include "boost/intrusive_ptr.hpp"

#include <vector>
#include <memory>


namespace InterSense {


class Station;


class Tracker : public Referenced
{
public:

  // Typedefs.
  typedef int Handle;
  typedef unsigned long Port;
  typedef boost::intrusive_ptr<Station> StationPtr;

  // Construction is initialization.
  explicit Tracker ( Port port = 0, 
                     Station::OrientationFormat of = Station::QUATERNION, 
                     Station::ID numStations = 8,
                     bool verbose = true );

  // Get the number of stations.
  Station::ID            numStations() const;

  // Get the i'th station.
  const Station*         station ( Station::ID i ) const;
  Station*               station ( Station::ID i );

  // Set the origin for all stations. This will vary depending on 
  // where the tracker is (i.e., the bars hanging from the ceiling).
  void                   origin ( float x, float y, float z );

  // Set the orientation format for all stations.
  void                   orientationFormat ( Station::OrientationFormat format );

  // Turn on/off the button and joystick input for all stations.
  void                   buttonAndJoystickInput ( bool state );

  // Set the length units for all stations.
  void                   lengthUnits ( Station::Units units );

  // Is this a valid handle?
  static bool            isHandle ( Handle handle ) { return handle >= 1; }

  // Access to the members.
  Handle                 handle()  const { return _handle; }
  bool                   verbose() const { return _verbose; }

  // Update the all the stations.
  void                   update();

protected:

  virtual ~Tracker();

  // Do not try to copy an instance of this class.
  Tracker ( const Tracker &tm );
  Tracker &              operator = ( const Tracker &tm );

  Station::ID            _initStations ( Station::ID numToQuery );

  typedef std::vector<StationPtr> Stations;

  Handle _handle;
  Port _port;
  bool _verbose;
  Stations _stations;
};


}; // namespace InterSense


#endif // _INTERSENSE_TRACKER_DEVICE_CLASS_H_
