
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __STAR_SYSTEM_ELEVATION_LAYER_DEM_H__
#define __STAR_SYSTEM_ELEVATION_LAYER_DEM_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Layers/RasterLayer.h"

#include "Usul/Interfaces/IElevationDatabase.h"
#include "Usul/Interfaces/IRead.h"

class ossimProjection; class ossimDemGrid;

namespace Minerva {
namespace Core {
namespace Layers {


class MINERVA_EXPORT ElevationLayerDem : public RasterLayer,
                                         public Usul::Interfaces::IElevationDatabase,
                                         public Usul::Interfaces::IRead
{
public:
  typedef RasterLayer BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;

  USUL_DECLARE_QUERY_POINTERS ( ElevationLayerDem );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  ElevationLayerDem();
  
  /// Clone.
  virtual IUnknown*     clone() const;
  
  /// Read.
  virtual void          read ( const std::string& filename, Usul::Interfaces::IUnknown* caller = 0x0, Usul::Interfaces::IUnknown* progress = 0x0 );

  /// Get the texture
  virtual ImagePtr      texture ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job *, IUnknown *caller );

  // Get the value at the lat, lon location.  May return null pixel value.
  double                value ( double lon, double lat ) const;
  
  /// Deserialize.
  virtual void          deserialize ( const XmlTree::Node &node );

  // Get the value at the lat, lon location (IElevationDatabase).
  virtual double        elevationAtLatLong ( double lat, double lon ) const;
  
protected:
  virtual ~ElevationLayerDem();
  
  ElevationLayerDem ( const ElevationLayerDem& );

  virtual ImagePtr      _createBlankImage ( unsigned int width, unsigned int height ) const;
  
  /// Open the dem.
  void                  _open ( const std::string& filename );
  
private:
  ElevationLayerDem& operator= ( const ElevationLayerDem& );
  
  std::string _filename;
  bool _loaded;
  ossimDemGrid *_grid;
  ossimProjection *_projection;

  SERIALIZE_XML_CLASS_NAME( ElevationLayerDem ) 
};


} // namespace Layers
} // namespace Core
} // namespace Minerva

#endif // __STAR_SYSTEM_ELEVATION_LAYER_DEM_H__