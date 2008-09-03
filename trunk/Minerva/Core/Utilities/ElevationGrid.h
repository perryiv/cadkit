
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_DATA_UTILITIES_ELEVATION_GRID_H__
#define __MINERVA_CORE_DATA_UTILITIES_ELEVATION_GRID_H__

#include "Minerva/Core/Extents.h"

#include "osg/Image"
#include "osg/Vec2d"


namespace Minerva {
namespace Core {
namespace Utilities {

class ElevationGrid
{
public:
  
  // Typedefs.
  typedef Minerva::Core::Extents<osg::Vec2d> Extents;
  typedef osg::ref_ptr<osg::Image> ImagePtr;
  
  /// Constructor.
  ElevationGrid ( ImagePtr image, const Extents& e ) : _extents ( e ), _image ( image )
  {
  }
  
  
  /// Get the elevation.  TODO: add ability to have different interpoling schemes.
  template<class Vertex>
  double operator() ( const Vertex& p ) const
  {
    if ( _image.valid() )
    {
      // Size of the elevation grid.
      const unsigned int width  ( _image->s() );
      const unsigned int height ( _image->t() );
      
      // Shortcuts.
      const Extents::Vertex &mn ( _extents.minimum() );
      const Extents::Vertex &mx ( _extents.maximum() );
      
      const double u ( ( p[0] - mn[0] ) / ( mx[0] - mn[0] ) );
      const double v ( ( p[1] - mn[1] ) / ( mx[1] - mn[1] ) );
      
      const unsigned int i ( Usul::Math::minimum ( width - 1,  static_cast <unsigned int> ( ::floor ( ( u * width ) + 0.5 ) ) ) );
      const unsigned int j ( Usul::Math::minimum ( height - 1, static_cast <unsigned int> ( ::floor ( ( v * height ) + 0.5 ) ) ) );
      
      // Set the elevation data.
      return *reinterpret_cast<float*> ( _image->data ( i, j ) );
    }
    
    return 0;
  }
  
private:
  
  Extents _extents;
  ImagePtr _image;
};

  
}
}
}

#endif // __MINERVA_CORE_DATA_UTILITIES_ELEVATION_GRID_H__
