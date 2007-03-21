
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __WNV_LAYERS_R_LAYER_H__
#define __WNV_LAYERS_R_LAYER_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Layers/Layer.h"

namespace Minerva {
namespace Core {
namespace Layers {

class MINERVA_EXPORT RLayer : public Minerva::Core::Layers::Layer
{
public:
  typedef Minerva::Core::Layers::Layer BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( RLayer );

  RLayer();

  /// Clone the this layer.
  virtual Layer*              clone() const;

  enum Mode
  {
    CYLINDER,
    POLYGONS
  };

  /// Get/Set the color.
  void                    color( const osg::Vec4& color );
  const osg::Vec4&        color() const;

  /// Get/Set the mode.
  void                    mode( Mode m );
  Mode                    mode() const;

  /// Build the scene.
  virtual void            buildScene( osg::Group* parent );

  virtual void            buildDataObjects( Usul::Interfaces::IUnknown *caller = 0x0 );

  virtual void            modify ( Usul::Interfaces::IUnknown *caller = 0x0 );

protected:
  virtual ~RLayer();

  RLayer( const RLayer& layer );

  void            _buildSceneCylinder( osg::Group* parent );
  void            _buildScenePolygons( osg::Group* parent );

  void            _buildDataObjectsCylinder( Usul::Interfaces::IUnknown *caller );
  void            _buildDataObjectsPolygons( Usul::Interfaces::IUnknown *caller );

  void            _registerMembers();

private:
  friend class boost::serialization::access;
  template < class Archive > void serialize( Archive &ar, const unsigned int version )
  {
    ar & boost::serialization::make_nvp( "BaseLayer", boost::serialization::base_object< BaseClass >(*this) );
    ar & boost::serialization::make_nvp( "Mode", _mode );
  }

  Mode _mode;

  SERIALIZE_XML_DEFINE_MEMBERS ( RLayer );
};

}
}
}


#endif // __WNV_LAYERS_R_LAYER_H__
