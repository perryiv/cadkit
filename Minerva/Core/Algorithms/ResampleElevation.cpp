
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Algorithms/ResampleElevation.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Return elevation data for the request extents.
//
///////////////////////////////////////////////////////////////////////////////

osg::Image* Minerva::Core::Algorithms::resampleElevation ( Tile::RefPtr tile, const Extents& request )
{
  if ( false == tile.valid() )
    return 0x0;
  
  // Create blank image.
  const Tile::MeshSize meshSize ( tile->meshSize() );
  const Tile::MeshSize::value_type rows ( meshSize[0] );
  const Tile::MeshSize::value_type columns ( meshSize[1] );
  
  osg::ref_ptr<osg::Image> answer ( new osg::Image );
  answer->allocateImage ( rows, columns, 1, GL_LUMINANCE, GL_FLOAT );
  ::memset ( answer->data(), 0, answer->getImageSizeInBytes() );
  
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
      
      // The osg::Image stores it's data as char*.  However, in this case the data of the image is float.
      // The data function will calculate and return the pointer to the beginning of the float.  The pointer needs to be cast to a float pointer so the proper value is accessed.
      *reinterpret_cast < float * > ( answer->data ( rows - i - 1, j ) ) = static_cast<float> ( heightAboveSeaLevel );
    }
  }
  
  return answer.release();
}
