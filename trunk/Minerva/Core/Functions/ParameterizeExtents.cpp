
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Functions/ParameterizeExtents.h"

#include <stdexcept>

///////////////////////////////////////////////////////////////////////////////
//
//  Returns min (u,v) and max (u,v) for the request extents.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Math::Vec4d Minerva::Core::Functions::paramertizeExtents ( const Extents& extents, const Extents& request )
{
  if ( false == extents.contains ( request.minimum() ) || false == extents.contains ( request.maximum() ) )
    throw std::invalid_argument ( "Error 1543233925: Invalid request extents." );

  // Length in x and y direction.
  const double lengthX ( extents.maximum()[0] - extents.minimum()[0] );
  const double lengthY ( extents.maximum()[1] - extents.minimum()[1] );

  // Return the answer.
  return Usul::Math::Vec4d ( request.minimum()[0] / lengthX, request.maximum()[0] / lengthX, request.minimum()[1] / lengthY, request.maximum()[1] / lengthY );
}
