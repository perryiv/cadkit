
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_ALGORITHMS_RESAMPLE_ELEVATION_H__
#define __MINERVA_CORE_ALGORITHMS_RESAMPLE_ELEVATION_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Extents.h"

#include "Usul/Math/Vector4.h"

#include "osg/Image"
#include "osg/Vec2d"

namespace Minerva {
namespace Core {
namespace Algorithms {

  typedef Minerva::Core::Extents<osg::Vec2d> Extents;

  MINERVA_EXPORT osg::Image* resampleElevation ( const osg::Image& image, const Extents& extents, const Extents& request, const Usul::Math::Vec4d& region );

}
}
}

#endif // __MINERVA_CORE_ALGORITHMS_RESAMPLE_ELEVATION_H__
