
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_INTERFACES_H__
#define __MINERVA_CORE_INTERFACES_H__

#include "Usul/Interfaces/IRawPointer.h"

namespace Minerva {
  namespace Core {

    /// Forward Declarations.
    namespace Layers 
    {
      class LineLayer;
      class PointLayer;
      class PointTimeLayer;
      class PolygonLayer;
      class PolygonTimeLayer;
      class Rlayer;
    }

    /// RawPointer typedefs.
    typedef Usul::Interfaces::IRawPointer< Minerva::Core::Layers::LineLayer,        2407192739u > ILineLayerRawPointer;
    typedef Usul::Interfaces::IRawPointer< Minerva::Core::Layers::PointLayer,       2322413855u > IPointLayerRawPointer;
    typedef Usul::Interfaces::IRawPointer< Minerva::Core::Layers::PointTimeLayer,   1381527312u > IPointTimeLayerRawPointer;
    typedef Usul::Interfaces::IRawPointer< Minerva::Core::Layers::PolygonLayer,     3724205550u > IPolygonLayerRawPointer;
    typedef Usul::Interfaces::IRawPointer< Minerva::Core::Layers::PolygonTimeLayer, 3137747044u > IPolygonTimeLayerRawPointer;
    typedef Usul::Interfaces::IRawPointer< Minerva::Core::Layers::Rlayer,           1666628749u > IRlayerRawPointer;
  }
}

#endif // __MINERVA_CORE_INTERFACES_H__
