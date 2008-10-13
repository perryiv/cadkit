
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_FUNCTIONS_PARAMETERIZE_EXTENTS_H__
#define __MINERVA_CORE_FUNCTIONS_PARAMETERIZE_EXTENTS_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Extents.h"

#include "Usul/Math/Vector4.h"

#include "osg/Vec2d"

namespace Minerva {
namespace Core {
namespace Functions {

  typedef Minerva::Core::Extents<osg::Vec2d> Extents;

  MINERVA_EXPORT Usul::Math::Vec4d paramertizeExtents ( const Extents& extents, const Extents& request );

}
}
}

#endif // __MINERVA_CORE_FUNCTIONS_PARAMETERIZE_EXTENTS_H__
