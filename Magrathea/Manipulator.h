//////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MAGRATHEA_MANIPULATOR_H__
#define __MAGRATHEA_MANIPULATOR_H__

#include "Export.h"

#include "ossimPlanet/ossimPlanetManipulator.h"

namespace Magrathea {

class MAGRATHEA_EXPORT Manipulator : public ossimPlanetManipulator
{
public:
  typedef ossimPlanetManipulator BaseClass;

  Manipulator();

  void gotoLocation( double lat, double lon, double height );

protected:
  virtual ~Manipulator();
};

}

#endif // __MAGRATHEA_MANIPULATOR_H__
