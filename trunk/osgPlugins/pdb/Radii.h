
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry Miller
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////


#ifndef __OSG_PDB_PLUGIN_RADII_H__
#define __OSG_PDB_PLUGIN_RADII_H__

#include <string>
#include <map>


class Radii 
{
public:

  typedef std::map < std::string, double > Map;

  // Constructor. Pass a scale for all radii.
  Radii ( double scale = 1.0 );

  // Destructor.
  ~Radii();

  // Query for known radii. Throws if not found.
  double find ( const std::string &type ) const;

  // Add new radius. Replaces existing value, if found. Returns (scaled) value.
  double add ( const std::string &type, double radius );

private:

  Map _map;
  double _scale;
};


#endif // __OSG_PDB_PLUGIN_RADII_H__
