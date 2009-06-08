
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

#ifndef _INTERSENSE_TRACKER_STATION_CLASS_H_
#define _INTERSENSE_TRACKER_STATION_CLASS_H_

#include "Usul/Base/Referenced.h"
#include "Usul/Math/Vector4.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Math/Matrix44.h"

#include <map>


namespace InterSense {


class Tracker;


class Station : public Usul::Base::Referenced
{
public:

  // Useful typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Math::Vec2f Vec2f;
  typedef Usul::Math::Vec3f Vec3f;
  typedef Usul::Math::Vec4f Vec4f;Referenced
  typedef Usul::Math::Matrix44f Matrix44f;

  // Orientation format.
  enum OrientationFormat { EULER, QUATERNION, UNSET };

  // Position and orientation indices.
  enum { YAW = 0, PITCH, ROLL };
  enum { X = 0, Y, Z, W };

  // Length units.
  enum Units { MILLIMETERS, CENTIMETERS, METERS, KILOMETERS, 
               INCHES, FEET, YARDS, MILES };

  // The buttons.
  enum ButtonID
  {
    BUTTON0         = 0x00000001,
    BUTTON1         = 0x00000002,
    BUTTON2         = 0x00000004,
    BUTTON3         = 0x00000008,
    BUTTON4         = 0x00000010,
    BUTTON5         = 0x00000020,
    BUTTON_YELLOW   = BUTTON0,
    BUTTON_RED      = BUTTON1,
    BUTTON_GREEN    = BUTTON2,
    BUTTON_BLUE     = BUTTON3,
    BUTTON_JOYSTICK = BUTTON4,
    BUTTON_TRIGGER  = BUTTON5
  };

  // The number of buttons.
  enum { NUM_BUTTONS = 6 };

  // Button states.
  enum ButtonState { OFF, ON, TOGGLE_OFF, TOGGLE_ON };

  // Typedef for station id.
  typedef unsigned short ID;

  // Construction is initialization.
  explicit Station ( Tracker *tracker, ID which );

  // Get/set the origin. This will vary depending on where the 
  // tracker is (i.e., the bars hanging from the ceiling).
  void                   origin ( float x, float y, float z );
  const Vec3f &          origin() const { return _origin; }

  // Set/get the orientation format.
  void                   orientationFormat ( OrientationFormat format );
  OrientationFormat      orientationFormat() const;

  // Turn on/off and query the button and joystick input.
  void                   buttonAndJoystickInput ( bool state );
  bool                   buttonAndJoystickInput() const;

  // Set/get the length units.
  void                   lengthUnits ( Units units );
  Units                  lengthUnits() const { return _units; }

  // Get the position in the current units.
  const Vec3f &          position() const { return _position; }

  // Get the orientation. The 4 values will either be a quaternion 
  // or Euler angles, depending on the OrientationFormat.
  const Vec4f &          orientation() const { return _orientation; }

  // Get the state of all the buttons as a bit-mask.
  unsigned int           buttons() const { return _buttons; }

  // Get the state of the specific button.
  ButtonState            button ( ButtonID id ) const;

  // Get the matrix.
  const Matrix44f        matrix() const { return _matrix; }

  // Get the joystick values in the range [-1,1].
  const Vec2f &          joystick() const { return _stick; }

  // Update this station.
  void                   update();

protected:

  virtual ~Station();

  // Do not try to copy an instance of this class.
  Station ( const Station &tm );
  Station &              operator = ( const Station &tm );

  void                   _update ( const void *data );

  void                   _setState ( bool state, ButtonID id );

  typedef std::map < ButtonID, ButtonState > ButtonMap;

  Tracker *_tracker;
  ID _which;
  mutable OrientationFormat _of;
  Vec3f _position;
  Vec4f _orientation;
  Vec3f _origin;
  Matrix44f _matrix;
  unsigned int _buttons;
  Vec2f _stick;
  float _lengthConv;
  Units _units;
  ButtonMap _map;

  friend class Tracker;
};


}; // namespace InterSense


#endif // _INTERSENSE_TRACKER_STATION_CLASS_H_
