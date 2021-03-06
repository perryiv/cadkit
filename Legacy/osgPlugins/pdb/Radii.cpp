
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry Miller
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Radii.h"

#include <stdexcept>
#include <sstream>


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Radii::Radii ( double scale ) : _scale ( scale )
{
  // Populate the map. Source of data:
  // http://www.monroecc.edu/wusers/flanzafame/PerRadii.pdf
  _map["H"] = 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Radii::~Radii()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for known radii. Throws if not found.
//
///////////////////////////////////////////////////////////////////////////////

double Radii::find ( const std::string &type ) const
{
  // Look for the radius.
  Map::const_iterator i = _map.find ( type );

  // If we didn't find it...
  if ( _map.end() == i )
  {
    std::stringstream message;
    message << "Error 3143216753:\n\tType '" << type << "' is not known";
    throw std::runtime_error ( message.str() );
  }

  // Return the radius.
  return i->second;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add new radius, returns (scaled) value.
//
///////////////////////////////////////////////////////////////////////////////

double Radii::add ( const std::string &type, double radius )
{
  double value ( radius * _scale );
  _map[type] = value;
  return value;
}
