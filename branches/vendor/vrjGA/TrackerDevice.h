
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Wrapper class for a single gadget::PositionInterface.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VRJGA_TRACKER_DEVICE_H_
#define _VRJGA_TRACKER_DEVICE_H_

#include "vrjGA/Referenced.h"

#include "gadget/Type/PositionInterface.h"

#include "gmtl/Matrix.h"

#include <string>


namespace vrjGA {


class VRJ_GA_EXPORT TrackerDevice : public Referenced
{
public:

  // Useful typedefs.
  typedef Referenced BaseClass;
  typedef gadget::PositionInterface PI;

  // Smart-pointer definitions.
  VRJGA_DECLARE_POINTER ( TrackerDevice );

  // Constructor.
  TrackerDevice ( const std::string &name );

  // Get the device's matrix.
  const gmtl::Matrix44f&   matrix() const { return _matrix; }

  // Get the time
  double time() { return _time; }

  // Update the internal state.
  void                      update();

  // Get the position.
  float                     x() const { return _matrix ( 0, 3 ); }
  float                     y() const { return _matrix ( 1, 3 ); }
  float                     z() const { return _matrix ( 2, 3 ); }

protected:
  virtual ~TrackerDevice();

private:

  // Not copyable.
  TrackerDevice ( const TrackerDevice & );
  TrackerDevice& operator = ( const TrackerDevice & );

  PI _pi;
  gmtl::Matrix44f _matrix;
  double _time;
};


}; // namespace vrjGA


#endif // _VRJGA_TRACKER_DEVICE_H_
