
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_LAYER_INFO_H__
#define __MINERVA_CORE_LAYER_INFO_H__

#include "Minerva/Core/Extents.h"

#include "osg/Vec2d"

namespace Minerva {
namespace Core {
namespace Layers {

  
struct LayerInfo
{
  typedef Minerva::Core::Extents<osg::Vec2d> Extents;
  
  std::string name;
  std::string style;
  std::string title;
  Extents extents;
};

  
}
}
}

#endif // __MINERVA_CORE_LAYER_INFO_H__
