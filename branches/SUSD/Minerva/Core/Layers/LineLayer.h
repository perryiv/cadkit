
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

protected:
  /// Use reference counting.
  virtual ~LineLayer();

  /// Copy Constructor.
  LineLayer ( const LineLayer& layer );

  void  _registerMembers();

private:

  float _lineWidth;

  SERIALIZE_XML_DEFINE_MEMBERS ( LineLayer );
};


}
}
}

#endif // __WNV_LAYERS_LINE_LAYER_H__
