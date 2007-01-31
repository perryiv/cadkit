
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Decision Theater at Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Manipulator.h"

using namespace Magrathea;


Manipulator::Manipulator() : BaseClass()
{
}

Manipulator::~Manipulator()
{
}

void Manipulator::gotoLocation( double lat, double lon, double height )
{
  theNavigator->setLatLonHeight( lat, lon, height );
  theNavigator->update();
}
