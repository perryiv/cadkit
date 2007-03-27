
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

namespace boost { namespace serialization { class access; } }

namespace Minerva {
namespace Core {
namespace Layers {

class MINERVA_EXPORT PointLayer : public Minerva::Core::Layers::Layer
{
public:
  typedef Minerva::Core::Layers::Layer BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( PointLayer );

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

  /// Set data members from given layer.
  virtual void            setDataMembers ( Layer * );

protected:
  virtual ~PointLayer();

  PointLayer( const PointLayer& layer );

  void _registerMembers();

private:
  friend class boost::serialization::access;
  template < class Archive > void serialize( Archive &ar, const unsigned int version );

  Usul::Types::Uint32 _primitiveID;
  float _size;
  bool _stackPoints;

  SERIALIZE_XML_DEFINE_MEMBERS ( PointLayer );
};


template < class Archive >
void PointLayer::serialize( Archive &ar, const unsigned int version )
{
  ar & boost::serialization::make_nvp( "BaseLayer", boost::serialization::base_object< BaseClass >(*this) );
  ar & boost::serialization::make_nvp( "PrimitiveID", _primitiveID );
  ar & boost::serialization::make_nvp( "Size", _size );
  ar & boost::serialization::make_nvp( "StackPoints", _stackPoints );
}

}
}
}

#endif // __WNV_LAYERS_POINT_LAYER_H__
