
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_POINT_LAYER_H__
#define __MINERVA_LAYERS_POINT_LAYER_H__

#include "Minerva/Layers/PostGIS/Layer.h"

#include "Minerva/Interfaces/IPointLayer.h"

namespace Minerva {
namespace Layers {
namespace PostGIS {

class MINERVA_POSTGIS_EXPORT PointLayer : public Minerva::Layers::PostGIS::Layer,
                                          public Minerva::Interfaces::IPointLayer
{
public:
  typedef Minerva::Layers::PostGIS::Layer BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( PointLayer );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  PointLayer();

  /// Get/Set primitive id.
  void                    primitiveID( Usul::Types::Uint32 );
  Usul::Types::Uint32     primitiveID() const;

  /// Get/Set the size.
  void                    primitiveSize( float );
  float                   primitiveSize() const;

  /// Get/Set the secondary size.
  float                   secondarySize () const;
  void                    secondarySize ( float );

  /// Get/Set primitive size column.
  void                    primitiveSizeColumn ( const std::string& );
  const std::string&      primitiveSizeColumn () const;

  /// Get/Set stack points.
  void                    stackPoints ( bool b );
  bool                    stackPoints() const;

  /// Get/Set quality
  void                    quality( float value );
  float                   quality() const;

  /// Get/Set use auto transform flag.
  void                    autotransform ( bool b );
  bool                    autotransform () const;

  /// Get the default query.
  virtual std::string     defaultQuery() const;

protected:
  virtual ~PointLayer();

  PointLayer( const PointLayer& layer );

  virtual void            _setGeometryMembers ( Geometry* geometry, const Minerva::DataSources::Result &result );

  /// Clone the this layer.
  virtual Usul::Interfaces::IUnknown*              clone() const;

private:

  Usul::Types::Uint32 _primitiveID;
  float               _size;
  float               _secondarySize;
  bool                _stackPoints;
  float               _quality;
  std::string         _primitiveSizeColumn;
  bool                _autotransform;

  SERIALIZE_XML_CLASS_NAME ( PointLayer );
};


}
}
}

#endif // __MINERVA_LAYERS_POINT_LAYER_H__
