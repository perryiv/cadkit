
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Helper functions to parse xml and create DataObjects.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_OPEN_STREET_MAP_FUNCTIONS_H__
#define __MINERVA_LAYERS_OPEN_STREET_MAP_FUNCTIONS_H__

#include "Minerva/Layers/OSM/Common.h"

#include <string>

namespace Minerva {
namespace Layers {
namespace OSM {

// Create a data object.
DataObject*          createForAllNodes ( const Nodes& nodes );
DataObject*          createForNode ( const Minerva::Layers::OSM::Node& node );
DataObject*          createForWay  ( const Minerva::Layers::OSM::Way&  way  );

}
}
}

#endif // __MINERVA_LAYERS_OPEN_STREET_MAP_FUNCTIONS_H__
