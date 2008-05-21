
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

#include "Minerva/Core/Layers/Vector.h"
#include "Minerva/Core/Geometry/Geometry.h"

#include "Usul/Interfaces/IRead.h"

class OGRLayer;
class OGRCoordinateTransformation;
class OGRGeometry;
class OGRPoint;

namespace Minerva {
namespace Layers {
namespace GDAL {
  
class OGRVectorLayer : public Minerva::Core::Layers::Vector,
                       public Usul::Interfaces::IRead
{
public:

  /// Typedefs.
  typedef Minerva::Core::Layers::Vector              BaseClass;
  typedef Minerva::Core::Geometry::Geometry          Geometry;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( OGRVectorLayer );
  USUL_DECLARE_IUNKNOWN_MEMBERS;
  
  OGRVectorLayer();

  // Read the file.
  virtual void                read ( const std::string &filename, Usul::Interfaces::IUnknown *caller = 0x0, Usul::Interfaces::IUnknown *progress = 0x0 );

protected:

  virtual ~OGRVectorLayer();

  void                        _addLayer ( OGRLayer* layer );

  Geometry*                   _createGeoemtry ( OGRGeometry* geometry, OGRCoordinateTransformation *transform );
  Geometry*                   _createPoint ( OGRPoint* geometry, OGRCoordinateTransformation *transform );

private:
  std::string _filename;
};

}
}
}

#endif // __MINERVA_LAYERS_GDAL_SHAPE_FILE_LAYER_H__
