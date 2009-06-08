
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Program to take lat/lon as input and output x,y,z coordinate.
//  (Also, I'm playing around with the Boost Program Options library.)
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/TileEngine/LandModelEllipsoid.h"

#include "Usul/Print/Vector.h"

#include "osg/CoordinateSystemNode"

#include "boost/program_options/options_description.hpp"
#include "boost/program_options/parsers.hpp"
#include "boost/program_options/variables_map.hpp"

#include <iostream>

///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to get the value from the variable map.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template <class T>
  inline T getValue ( const boost::program_options::variables_map& map, const std::string& key, const T& defaultValue )
  {
    return ( map.count ( key ) > 0 ? map[key].as<T>() : defaultValue );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Main function.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char** argv )
{
  // Declare the supported options.
  boost::program_options::options_description desc ( "Allowed options" );

  // Add the options.
  desc.add_options()
      ("help", "Produce help message.")
      ("lat", boost::program_options::value<double>(), "Latitude value.")
      ("lon", boost::program_options::value<double>(), "Longitude value.")
      ("elevation", boost::program_options::value<double>(), "Elevation value.");

  // Parse the command line.
  boost::program_options::variables_map variableMap;
  boost::program_options::store  ( boost::program_options::parse_command_line ( argc, argv, desc ), variableMap );
  boost::program_options::notify ( variableMap );

  // Check to see if help was selected.
  if ( variableMap.count ( "help" ) > 0 )
  {
    std::cout << desc << std::endl;
    return 1;
  }

  // Default settings (TODO: add options for setting the ellipsoid values).
  const double polarRadius ( osg::WGS_84_RADIUS_EQUATOR ), equatorRadius ( osg::WGS_84_RADIUS_POLAR );

  // See if the user selected latitude.
  const double lat ( Helper::getValue ( variableMap, "lat", 0.0 ) );

  // See if the user selected longitude.
  const double lon ( Helper::getValue ( variableMap, "lon", 0.0 ) );

  // See if the user selected elevation.
  const double elevation ( Helper::getValue ( variableMap, "elevation", 0.0 ) );

  // Make the land model.
  typedef Minerva::Core::TileEngine::LandModelEllipsoid Land;
  Land::Vec2d radii ( polarRadius, equatorRadius );
  Land::RefPtr land ( new Land ( radii ) );

  // Calculate.
  osg::Vec3d point;
  land->latLonHeightToXYZ ( lat, lon, elevation, point.x(), point.y(), point.z() );

  // Output the result.
  Usul::Print::vector ( "", point[0], point[1], point[2], std::cout, 30, 20 );

  // Success.
  return 0;
}
