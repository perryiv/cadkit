
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __WNV_LAYERS_POINT_LAYER_H__
#define __WNV_LAYERS_POINT_LAYER_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Layers/Layer.h"

#include "Usul/Interfaces/IPointLayer.h"

namespace Minerva {
namespace Core {
namespace Layers {

class MINERVA_EXPORT PointLayer : public Minerva::Core::Layers::Layer,
                                  public Usul::Interfaces::IPointLayer
{
public:
  typedef Minerva::Core::Layers::Layer BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( PointLayer );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  PointLayer();

  /// Clone the this layer.
  virtual Layer*              clone() const;

  /// Build the data objects.
  virtual void            buildDataObjects( Usul::Interfaces::IUnknown *caller = 0x0 );

  /// Modifty the data objects.
  virtual void            modify( Usul::Interfaces::IUnknown *caller = 0x0 );

  /// Get/Set primitive id.
  void                    primitiveID( Usul::Types::Uint32 );
  Usul::Types::Uint32     primitiveID() const;

  /// Get/Set the size.
  void                    size( float );
  float                   size() const;

  /// Get/Set stack points.
  void                    stackPoints ( bool b );
  bool                    stackPoints() const;

  /// Get/Set quality
  void                    quality( float value );
  float                   quality() const;

protected:
  virtual ~PointLayer();

  PointLayer( const PointLayer& layer );

  void _registerMembers();

  void _stack ();

private:

  Usul::Types::Uint32 _primitiveID;
  float _size;
  bool _stackPoints;
  float _quality;

  SERIALIZE_XML_CLASS_NAME ( PointLayer );
};


}
}
}

#endif // __WNV_LAYERS_POINT_LAYER_H__
