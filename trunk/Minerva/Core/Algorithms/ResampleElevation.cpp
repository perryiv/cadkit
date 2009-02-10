
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Algorithms/ResampleElevation.h"
#include "Minerva/Core/ElevationData.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Return elevation data for the request extents.
//
///////////////////////////////////////////////////////////////////////////////

Minerva::Interfaces::IElevationData* Minerva::Core::Algorithms::resampleElevation ( Tile::RefPtr tile, const Extents& request )
{
  if ( false == tile.valid() )
    return 0x0;
  
  const Tile::MeshSize meshSize ( tile->meshSize() );
  const Tile::MeshSize::value_type rows ( meshSize[0] );
  const Tile::MeshSize::value_type columns ( meshSize[1] );
  
  Minerva::Core::ElevationData::RefPtr answer ( new Minerva::Core::ElevationData ( rows, columns ) );
  
  const Extents::Vertex &mn ( request.minimum() );
  const Extents::Vertex &mx ( request.maximum() );
    
  // For each lat/lon location in the image, ask the parent for it's value.
  for ( int i = rows - 1; i >= 0; --i )
  {
    const double u ( 1.0 - static_cast<double> ( i ) / ( rows - 1 ) );
    
    for ( Tile::MeshSize::value_type j = 0; j < columns; ++j )
    {
      const double v ( static_cast<double> ( j ) / ( columns - 1 ) );
      
      const double lon ( mn[0] + u * ( mx[0] - mn[0] ) );
      const double lat ( mn[1] + v * ( mx[1] - mn[1] ) );
      
      const double heightAboveSeaLevel ( tile->elevation ( lat, lon ) );
      answer->value ( rows - i - 1, j, heightAboveSeaLevel );
    }
  }
  
  return answer.release();
}
