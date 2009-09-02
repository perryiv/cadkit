
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Typedefs.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MINERVA_CORE_TYPEDEFS_H_
#define _MINERVA_CORE_TYPEDEFS_H_

#include "Usul/Math/Vector2.h"

#include "Minerva/Core/Extents.h"

#include "osg/Vec2d"

namespace Minerva {
namespace Core {
namespace TileEngine {

typedef Usul::Math::Vec2ui MeshSize;
typedef MeshSize ImageSize;
typedef Minerva::Core::Extents < osg::Vec2d > Extents;

}
}
}

#endif // _MINERVA_CORE_TYPEDEFS_H_
