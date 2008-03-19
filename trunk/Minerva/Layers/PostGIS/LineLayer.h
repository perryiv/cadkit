
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_LINE_LAYER_H__
#define __MINERVA_LAYERS_LINE_LAYER_H__

#include "Minerva/Layers/PostGIS/Layer.h"

#include "Minerva/Interfaces/ILineLayer.h"

namespace Minerva {
namespace Layers {
namespace PostGIS {

class MINERVA_POSTGIS_EXPORT LineLayer : public Minerva::Layers::PostGIS::Layer,
                                         public Minerva::Interfaces::ILineLayer
{
public:
  typedef Minerva::Layers::PostGIS::Layer BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( LineLayer );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  LineLayer();

  /// Accept the visitor.
  virtual void            accept ( Minerva::Core::Visitor& visitor );

  /// Build the data objects.
  virtual void            buildDataObjects( Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *progress );

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

  /// Clone the this layer.
  virtual Usul::Interfaces::IUnknown*          clone() const;

private:

  float _lineWidth;

  SERIALIZE_XML_CLASS_NAME ( LineLayer );
};


}
}
}

#endif // __MINERVA_LAYERS_LINE_LAYER_H__
