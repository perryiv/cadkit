
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

#include "VRV/Devices/TrackerDevice.h"

using namespace VRV::Devices;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

TrackerDevice::TrackerDevice ( const std::string &name ) : 
  BaseClass(),
  _pi(),
  _matrix(),
  _time(0.0)
{
  // Initialize.
  _pi.init ( name );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

TrackerDevice::~TrackerDevice()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the internal state.
//
///////////////////////////////////////////////////////////////////////////////

void TrackerDevice::update()
{
  _matrix = _pi->getData();
  _time = _pi->getTimeStamp().secd();
}
