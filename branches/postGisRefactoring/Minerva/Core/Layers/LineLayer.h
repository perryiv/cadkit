
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __WNV_LAYERS_LINE_LAYER_H__
#define __WNV_LAYERS_LINE_LAYER_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Layers/Layer.h"

#include "OsgTools/Utilities/Vec4Serialize.h"

#include "osg/Vec4"

#include "boost/serialization/base_object.hpp"
#include "boost/serialization/nvp.hpp"
#include "boost/serialization/version.hpp"
namespace boost { namespace serialization { class access; } }

namespace Minerva {
namespace Core {
namespace Layers {

class MINERVA_EXPORT LineLayer : public Minerva::Core::Layers::Layer
{
public:
  typedef Minerva::Core::Layers::Layer BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( LineLayer );

  LineLayer();

  /// Clone the this layer.
  virtual Layer*              clone() const;

  /// Build the data objects.
  virtual void            buildDataObjects( Usul::Interfaces::IUnknown *caller );

  virtual void            modify( Usul::Interfaces::IUnknown *caller = 0x0 );

  /// Get/Set the line width
  void                    lineWidth( float );
  float                   lineWidth() const;

  /// Set data members from given layer.
  virtual void            setDataMembers ( Layer * );

protected:
  /// Use reference counting.
  virtual ~LineLayer();

  /// Copy Constructor.
  LineLayer ( const LineLayer& layer );

  void  _registerMembers();

private:
  friend class boost::serialization::access;
  template < class Archive > void serialize( Archive &ar, const unsigned int version );

  float _lineWidth;

  SERIALIZE_XML_DEFINE_MEMBERS ( LineLayer );
};

template < class Archive >
void LineLayer::serialize( Archive &ar, const unsigned int version )
{
  ar & boost::serialization::make_nvp( "BaseLayer", boost::serialization::base_object< BaseClass >(*this) );
  ar & boost::serialization::make_nvp( "LineWidth", _lineWidth );
}


//BOOST_CLASS_EXPORT_GUID( LineLayer, "LineLayer" )

}
}
}

#endif // __WNV_LAYERS_LINE_LAYER_H__
