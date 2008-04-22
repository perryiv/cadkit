
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class for using an InterSense tracker station (e.g., wand).
//
///////////////////////////////////////////////////////////////////////////////

#include "InterSense/Station.h"
#include "InterSense/Tracker.h"
#include "InterSense/Functions.h"
#include "InterSense/Exceptions.h"
#include "InterSense/Defines.h"
#include "InterSense/Constants.h"

#include "/Usul/Errors/Assert.h"
#include "/Usul/MPL/SameType.h"

#include "isense/isense.h"

using namespace InterSense;


///////////////////////////////////////////////////////////////////////////////
//
//  Default constructor.
//
///////////////////////////////////////////////////////////////////////////////

Station::Station ( Tracker *tracker, Station::ID which ) : BaseClass(),
  _tracker     ( tracker ),
  _which       ( which ),
  _of          ( Station::UNSET ),
  _position    ( 0, 0, 0 ),
  _orientation ( 0, 0, 0, 0 ),
  _origin      ( InterSense::DEFAULT_ORIGIN_X, InterSense::DEFAULT_ORIGIN_Y, InterSense::DEFAULT_ORIGIN_Z ),
  _matrix      (),
  _buttons     ( 0 ),
  _stick       ( 0, 0 ),
  _lengthConv  ( InterSense::METERS_TO_FEET ),
  _units       ( Station::FEET ),
  _map()
{
  USUL_ASSERT_SAME_TYPE ( ID, ::WORD );
  USUL_ASSERT ( _which < ISD_MAX_STATIONS );

  // Initialize the map.
  _map[Station::BUTTON0] = OFF;
  _map[Station::BUTTON1] = OFF;
  _map[Station::BUTTON2] = OFF;
  _map[Station::BUTTON3] = OFF;
  _map[Station::BUTTON4] = OFF;
  _map[Station::BUTTON5] = OFF;

  // Make sure the matrix is identity.
  gmtl::identity ( _matrix );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Station::~Station()
{
  // Cannot have inline destructor if you only forward-declare Tracker.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the orientation format.
//
///////////////////////////////////////////////////////////////////////////////

void Station::orientationFormat ( Station::OrientationFormat format )
{
  assert ( _which < ISD_MAX_STATIONS );
  assert ( _tracker.get() );

  // See if it is already set.
  if ( format == _of )
    return;

  // Initialize the station-info struct.
  ITL_DECLARE_AND_INIT ( ISD_STATION_INFO_TYPE, station );

  // Get the configuration of the station.
  Detail::getStationConfig ( *_tracker, _which, station );

  // Ask the station to change.
  station.AngleFormat = ( QUATERNION == format ) ? ISD_QUATERNION : ISD_EULER;
  Detail::setStationConfig ( *_tracker, _which, station );

  // Cache the setting.
  _of = format;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the orientation format.
//
///////////////////////////////////////////////////////////////////////////////

Station::OrientationFormat Station::orientationFormat() const
{
  assert ( _which < ISD_MAX_STATIONS );
  assert ( _tracker.get() );

  // Return the cached value unless this is the first time.
  if ( UNSET != _of )
    return _of;

  // Initialize the station-info struct.
  ITL_DECLARE_AND_INIT ( ISD_STATION_INFO_TYPE, station );

  // Get the configuration of the station.
  Detail::getStationConfig ( *_tracker, _which, station );

  // Return the format flag.
  _of = ( ISD_QUATERNION == station.AngleFormat ) ? QUATERNION : EULER;
  return _of;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Turn on/off the button and joystick input.
//
///////////////////////////////////////////////////////////////////////////////

void Station::buttonAndJoystickInput ( bool state )
{
  assert ( _which < ISD_MAX_STATIONS );
  assert ( _tracker.get() );

  // Initialize the station-info struct.
  ITL_DECLARE_AND_INIT ( ISD_STATION_INFO_TYPE, station );

  // Get the configuration of the station.
  Detail::getStationConfig ( *_tracker, _which, station );

  // If we are already set...
  if ( state == ( TRUE == station.GetInputs ) )
    return;

  // Ask the station to change.
  station.GetInputs = state;
  Detail::setStationConfig ( *_tracker, _which, station );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is button and joystick input on or off?
//
///////////////////////////////////////////////////////////////////////////////

bool Station::buttonAndJoystickInput() const
{
  assert ( _which < ISD_MAX_STATIONS );
  assert ( _tracker.get() );

  // Initialize the station-info struct.
  ITL_DECLARE_AND_INIT ( ISD_STATION_INFO_TYPE, station );

  // Get the configuration of the i'th station.
  Detail::getStationConfig ( *_tracker, _which, station );

  // Return the format flag.
  return ( TRUE == station.GetInputs );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update this station.
//
///////////////////////////////////////////////////////////////////////////////

void Station::update()
{
  assert ( _which < ISD_MAX_STATIONS );
  assert ( _tracker.get() );

  // Initialize the station-info struct.
  ITL_DECLARE_AND_INIT ( ISD_TRACKER_DATA_TYPE, data );

  // Get the station's data.
  Detail::getStationData ( *(_tracker.get()), data );

  // Call the other one.
  this->_update ( &(data.Station[_which]) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the data. Note the difference in coordinate systems.
//
//  Tracker's coordinate system       OpenGL's coordinate system.
//
//            x                                y
//           /                                 |
//          /                                  |
//          ---- y                              ---- x
//         |                                  /
//         |                                 /
//         z                                z
//
//  The origin of the tracker's coordinate system is the top-left corner,
//  closest to the screen.
//
///////////////////////////////////////////////////////////////////////////////

void Station::_update ( const void *stationState )
{
  BOOST_STATIC_ASSERT ( 8 == ISD_MAX_BUTTONS );
  BOOST_STATIC_ASSERT ( 2 <= ISD_MAX_CHANNELS );
  assert ( _which < ISD_MAX_STATIONS );
  assert ( _lengthConv > 0 );
  assert ( stationState );

  // Cast to the struct we need.
  void *temp1 = const_cast<void*>(stationState);
  ISD_STATION_STATE_TYPE *temp2 = reinterpret_cast<ISD_STATION_STATE_TYPE*>(temp1);
  ISD_STATION_STATE_TYPE &data = *temp2;

  // Set our position. Note the signs, index ordering, and the origin offset.
  _position[0] =   data.Position[1] * _lengthConv - _origin[0];
  _position[1] = - data.Position[2] * _lengthConv - _origin[1];
  _position[2] = - data.Position[0] * _lengthConv - _origin[2];

  // Set the translation matrix. We'll need the rotation matrix below.
  gmtl::Matrix44f R, T;
  gmtl::setTrans ( T, _position );

  // Set the orientation. Note the index ordering. 
  if ( QUATERNION == _of )
  {
    // The tracker's scalar component is in the 0'th place. The index switching
    // is the same as the position (above), but with one added to each index.
    _orientation[0] =   data.Orientation[2];
    _orientation[1] = - data.Orientation[3];
    _orientation[2] = - data.Orientation[1];
    _orientation[3] =   data.Orientation[0];

    // Set the rotation matrix.    
    const gmtl::Quatf q ( _orientation[0], _orientation[1], _orientation[2], _orientation[3] );
    gmtl::setRot ( R, q );
  }

  else // Euler
  {
    // This is untested.
    #ifdef _DEBUG
    std::cout << "Warning: Euler angles may be in the wrong order, and no rotation matrix is being set." << std::endl;
    #endif
    _orientation[0] =   data.Orientation[1];
    _orientation[1] = - data.Orientation[2];
    _orientation[2] = - data.Orientation[0];
    _orientation[3] =   data.Orientation[3];
  }

  // Set the matrix.
  _matrix = T * R;

  // The button state.
  _buttons = 0;
  _buttons |= ( data.ButtonState[0] ) ? Station::BUTTON0 : 0;
  _buttons |= ( data.ButtonState[1] ) ? Station::BUTTON1 : 0;
  _buttons |= ( data.ButtonState[2] ) ? Station::BUTTON2 : 0;
  _buttons |= ( data.ButtonState[3] ) ? Station::BUTTON3 : 0;
  _buttons |= ( data.ButtonState[4] ) ? Station::BUTTON4 : 0;
  _buttons |= ( data.ButtonState[5] ) ? Station::BUTTON5 : 0;

  // Set the map of button-states.
  this->_setState ( data.ButtonState[0], Station::BUTTON0 );
  this->_setState ( data.ButtonState[1], Station::BUTTON1 );
  this->_setState ( data.ButtonState[2], Station::BUTTON2 );
  this->_setState ( data.ButtonState[3], Station::BUTTON3 );
  this->_setState ( data.ButtonState[4], Station::BUTTON4 );
  this->_setState ( data.ButtonState[5], Station::BUTTON5 );

  // Joystick values. Documentation says that range is [0,255] with 127 
  // representing the center. I need 127 to map to 0.5, so I am using 
  // 254 as the max.
  float maximum ( 254 );
  float horizontal ( data.AnalogData[0] );
  float vertical   ( data.AnalogData[1] );
  horizontal = ( std::min ( maximum, std::max ( 0.0f, horizontal ) ) ) / maximum;
  vertical   = ( std::min ( maximum, std::max ( 0.0f, vertical   ) ) ) / maximum;

  // Now we are in the range [0,1] so adjust to [-1,1].
  _stick[0] = 2.0f * horizontal - 1.0f;
  _stick[1] = 2.0f * vertical   - 1.0f;

  #if 1
  const char *f1 = "%7.3f";
  ::printf ( "pos = " );
  ::printf ( f1, _position[0] );
  ::printf ( f1, _position[1] );
  ::printf ( f1, _position[2] );
  const char *f2 = "%6.2f";
  ::printf ( "  orien = " );
  ::printf ( f2, _orientation[0] );
  ::printf ( f2, _orientation[1] );
  ::printf ( f2, _orientation[2] );
  ::printf ( f2, _orientation[3] );
  ::printf ( "\r" );
  ::fflush ( stdout );
  #endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Specify the length units.
//
///////////////////////////////////////////////////////////////////////////////

void Station::lengthUnits ( Units units )
{
  switch ( units )
  {
  case MILLIMETERS:
    _lengthConv = METERS_TO_MILLIMETERS;
    break;
  case CENTIMETERS:
    _lengthConv = METERS_TO_CENTIMETERS;
    break;
  case METERS:
    _lengthConv = 1;
    break;
  case KILOMETERS:
    _lengthConv = METERS_TO_KILOMETERS;
    break;
  case INCHES:
    _lengthConv = METERS_TO_INCHES;
    break;
  case FEET:
    _lengthConv = METERS_TO_FEET;
    break;
  case YARDS:
    _lengthConv = METERS_TO_YARDS;
    break;
  case MILES:
    _lengthConv = METERS_TO_MILES;
    break;
  default:
    throw UnknownUnits();
  }

  _units = units;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Determine the button state for our map based on what whether or not the 
//  actual button is pressed (state) and what the previous state was 
//  (from the map).
//
///////////////////////////////////////////////////////////////////////////////

void Station::_setState ( bool state, Station::ButtonID id )
{
  ButtonState current ( this->button ( id ) );
  if ( state ) // on
    _map[id] = ( Station::OFF == current ) ? Station::TOGGLE_ON  : Station::ON;
  else // off
    _map[id] = ( Station::ON  == current ) ? Station::TOGGLE_OFF : Station::OFF;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the button state.
//
///////////////////////////////////////////////////////////////////////////////

Station::ButtonState Station::button ( Station::ButtonID id ) const
{
  ButtonMap::const_iterator i = _map.find ( id );
  if ( _map.end() == i ) 
    throw InvalidButtonID();
  return i->second;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the origin. This will vary depending on where the tracker is 
//  (i.e., the bars hanging from the ceiling).
//
///////////////////////////////////////////////////////////////////////////////

void Station::origin ( float x, float y, float z )
{
  _origin[0] = x;
  _origin[1] = y;
  _origin[2] = z;
}
