
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __STAR_SYSTEM_ELEVATION_LAYER_ARC_ASCII_H__
#define __STAR_SYSTEM_ELEVATION_LAYER_ARC_ASCII_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Layers/RasterLayer.h"

#include "Usul/Math/Vector2.h"

class ossimProjection; class ossimDemGrid;

namespace Minerva {
namespace Core {
namespace Layers {


class MINERVA_EXPORT ElevationLayerArcAscii : public RasterLayer
{
public:
  typedef RasterLayer BaseClass;
  typedef std::vector< double > GridRow;
  typedef std::vector< GridRow > ArcGrid;

  USUL_DECLARE_REF_POINTERS ( ElevationLayerArcAscii );

  ElevationLayerArcAscii();

  virtual IUnknown*     clone() const;
  
  /// Open the dem.
  void                  open ( const std::string& filename );

  /// Get the texture
 virtual ImagePtr       texture ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job *, Usul::Interfaces::IUnknown *caller );

  // Get the value at the lat, lon location.  May return null pixel value.
  double                value ( double lon, double lat ) const;

  
protected:
  virtual ~ElevationLayerArcAscii();

  //virtual ImagePtr      _createBlankImage ( unsigned int width, unsigned int height ) const;

  void                  _read( const std::string& filename );
  
private:
  bool                  _loaded;
  ArcGrid               _grid;
  ossimProjection*      _projection;
  
  
  Usul::Math::Vec2ui    _gridResolution;
  Usul::Math::Vec2d     _lowerLeft;
  double                _cellSize;
  double                _noDataDouble;
  double                _minElevation;


  SERIALIZE_XML_DEFINE_MEMBERS ( ElevationLayerArcAscii );
};


} // namespace Layers
} // namespace Core
} // namespace Minerva

#endif // __STAR_SYSTEM_ELEVATION_LAYER_ARC_ASCII_H__
