
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Decision Theater at Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////


#ifndef __LAT_LONG_GRID_H__
#define __LAT_LONG_GRID_H__

#include "Export.h"

#include "osg/Geode"

namespace Magrathea {

class MAGRATHEA_EXPORT LatLongGrid : public osg::Geode
{
public:
  typedef osg::Geode BaseClass;

  LatLongGrid();

protected:
  virtual ~LatLongGrid();
};

}

#endif // __LAT_LONG_GRID_H__
