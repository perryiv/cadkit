
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_POLYGON_LAYER_H__
#define __MINERVA_LAYERS_POLYGON_LAYER_H__

#include "Minerva/Layers/PostGIS/Layer.h"
#include "Minerva/Interfaces/IPolygonLayer.h"

#include "Usul/Math/Vector4.h"

namespace Minerva {
namespace Layers {
namespace PostGIS {

class MINERVA_POSTGIS_EXPORT PolygonLayer : public Minerva::Layers::PostGIS::Layer,
                                            public Minerva::Interfaces::IPolygonLayer
{
public:
  typedef Minerva::Layers::PostGIS::Layer BaseClass;
  typedef Usul::Math::Vec4f Color;

  USUL_DECLARE_QUERY_POINTERS ( PolygonLayer );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  PolygonLayer();

  /// Get/Set the show interior flag.
  void                    showInterior( bool b );
  bool                    showInterior() const;

  /// Get/Set the show border flag.
  void                    showBorder( bool b );
  bool                    showBorder() const;

  /// Get/Set the border color.
  void                    borderColor( const Color& color );
  Color                   borderColor() const;

  /// Get/Set the border width.
  void                    borderWidth ( float width );
  float                   borderWidth () const;

protected:
  virtual ~PolygonLayer();

  PolygonLayer( const PolygonLayer& layer );

  void _registerMembers();
  
  virtual void                _setGeometryMembers   ( Geometry* geometry, const Minerva::DataSources::Result &result );

  /// Clone the this layer.
  virtual Usul::Interfaces::IUnknown*          clone() const;

private:

  bool        _showInterior;
  bool        _showBorder;
  Color       _borderColor;
  float       _borderWidth;

  SERIALIZE_XML_CLASS_NAME ( PolygonLayer );
};


}
}
}

#endif // __MINERVA_LAYERS_POLYGON_LAYER_H__
