
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MAGRATHEA_FRAME_H__
#define __MAGRATHEA_FRAME_H__

class Frame
{
public:

private:
  // Lat/Lon position.
  osg::Vec3 _position;

  double _elevation;
  osg::Quat _rotation;
}

#endif // __MAGRATHEA_FRAME_H__
