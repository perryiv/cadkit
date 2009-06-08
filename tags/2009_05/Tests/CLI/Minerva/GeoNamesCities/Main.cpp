
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Program to test retrieving cities from GeoNames web service.
//  TODO: Need to add command line options.
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/GeoNames/CityLayer.h"


int main ( int argc, char** argv )
{
  // Typedefs.
  typedef Minerva::Layers::GeoNames::CityLayer GeoNamesCities;
  typedef GeoNamesCities::Cities Cities;
  typedef GeoNamesCities::Extents Extents;

  // Make the layer.
  GeoNamesCities::RefPtr layer ( new GeoNamesCities );

  Cities cities ( layer->citiesGet ( Extents ( -110, 33.5, -111, 34.0 ), 0, 10 ) );

  for ( Cities::const_iterator iter = cities.begin(); iter != cities.end(); ++iter )
  {
    std::cout << "Name: " << iter->name() << " Location: " << iter->location()[1] << " " << iter->location()[0] << std::endl;
  }

  return 0;
}
