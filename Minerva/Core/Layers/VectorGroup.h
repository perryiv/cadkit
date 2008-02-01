
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_VECTOR_GROUP_H__
#define __MINERVA_LAYERS_VECTOR_GROUP_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Layers/Vector.h"

#include "Usul/Interfaces/ILayerList.h"

#include <vector>

namespace Minerva {
namespace Core { 
    
class Visitor;
    
namespace Layers {
      
class MINERVA_EXPORT VectorGroup : public Minerva::Core::Layers::Vector,
                                   public Usul::Interfaces::ILayerList
{
public:
  // Typedefs.
  typedef Minerva::Core::Layers::Vector     BaseClass;
  typedef std::vector<Vector::RefPtr>       Layers;
  
  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( VectorGroup );
  USUL_DECLARE_IUNKNOWN_MEMBERS;
  
  VectorGroup();
  
  /// Accept the visitor.
  virtual void                accept ( Minerva::Core::Visitor& visitor );
  
  /// Traverse all DataObjects.
  virtual void                traverse ( Minerva::Core::Visitor& visitor );
  
  /// Build the scene.
  virtual osg::Node *         buildScene ( const Options &options, Usul::Interfaces::IUnknown *caller = 0x0 );
  
  /// Add a layer.
  void                        addLayer ( Vector* layer );
  
  /// Clear layers.
  void                        clearLayers();
  
  /// Get the number of layers (ILayer).
  virtual unsigned int                  numberLayers() const;
  
  /// Get the layer at position i (ILayer).
  virtual Usul::Interfaces::ILayer*     layer ( unsigned int i );
protected:
  virtual ~VectorGroup();
  
private:
  Layers _layers;
  
  SERIALIZE_XML_DEFINE_MEMBERS ( VectorGroup );

};

}
}
}

#endif // __MINERVA_LAYERS_VECTOR_GROUP_H__
