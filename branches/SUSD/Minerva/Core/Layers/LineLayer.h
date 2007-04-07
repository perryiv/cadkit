
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

#include "Usul/Interfaces/ILineLayer.h"

namespace Minerva {
namespace Core {
namespace Layers {

class MINERVA_EXPORT LineLayer : public Minerva::Core::Layers::Layer,
                                 public Usul::Interfaces::ILineLayer
{
public:
  typedef Minerva::Core::Layers::Layer BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( LineLayer );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  LineLayer();

  /// Clone the this layer.
  virtual Layer*          clone() const;

  /// Build the data objects.
  virtual void            buildDataObjects( Usul::Interfaces::IUnknown *caller = 0x0 );

  virtual void            modify( Usul::Interfaces::IUnknown *caller = 0x0 );

  /// Get/Set the line width.
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

  SERIALIZE_XML_CLASS_NAME ( LineLayer );
};


}
}
}

#endif // __WNV_LAYERS_LINE_LAYER_H__
