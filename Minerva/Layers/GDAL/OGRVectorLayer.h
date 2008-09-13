
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_GDAL_SHAPE_FILE_LAYER_H__
#define __MINERVA_LAYERS_GDAL_SHAPE_FILE_LAYER_H__

#include "Minerva/Core/Data/Container.h"
#include "Minerva/Core/Data/Geometry.h"
#include "Minerva/Core/Data/LineStyle.h"

#include "Usul/Interfaces/IRead.h"

class OGRLayer;
class OGRCoordinateTransformation;
class OGRGeometry;
class OGRPoint;
class OGRLineString;
class OGRPolygon;

namespace Minerva {
namespace Layers {
namespace GDAL {
  
class OGRVectorLayer : public Minerva::Core::Data::Container,
                       public Usul::Interfaces::IRead
{
public:

  /// Typedefs.
  typedef Minerva::Core::Data::Container                BaseClass;
  typedef Minerva::Core::Data::Geometry                 Geometry;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( OGRVectorLayer );
  USUL_DECLARE_IUNKNOWN_MEMBERS;
  
  OGRVectorLayer();

  // Read the file.
  virtual void                read ( const std::string &filename, Usul::Interfaces::IUnknown *caller = 0x0, Usul::Interfaces::IUnknown *progress = 0x0 );

protected:

  virtual ~OGRVectorLayer();

  void                        _addLayer ( OGRLayer* layer, Usul::Interfaces::IUnknown *progress );

  Geometry*                   _createGeoemtry ( OGRGeometry* geometry,   OGRCoordinateTransformation *transform ) const;
  Geometry*                   _createPoint    ( OGRPoint* geometry,      OGRCoordinateTransformation *transform ) const;
  Geometry*                   _createLine     ( OGRLineString* geometry, OGRCoordinateTransformation *transform ) const;
  Geometry*                   _createPolygon  ( OGRPolygon* geometry,    OGRCoordinateTransformation *transform ) const;

private:
  
  std::string _filename;
  Minerva::Core::Data::LineStyle::RefPtr _lineStyle;
};

}
}
}

#endif // __MINERVA_LAYERS_GDAL_SHAPE_FILE_LAYER_H__
