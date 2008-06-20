
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class to transform vertices from one coordinate system to another.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_DATA_TRANSFORM_H__
#define __MINERVA_CORE_DATA_TRANSFORM_H__

#include "Minerva/Core/Export.h"

#include "Usul/Math/Vector3.h"
#include "Usul/Math/Vector2.h"

#include <string>

class OGRCoordinateTransformation;

namespace Minerva {
namespace Core {
namespace Data {


class MINERVA_EXPORT Transform
{
public:
  
  Transform ( const std::string& from, const std::string& to );
  ~Transform();

  // Transform.
  Usul::Math::Vec3d operator() ( const Usul::Math::Vec3d& ) const;
  Usul::Math::Vec2d operator() ( const Usul::Math::Vec2d& ) const;
  
  static std::string wgs84AsWellKnownText();
private:
  OGRCoordinateTransformation *_transform;
};


}
}
}

#endif // __MINERVA_CORE_DATA_TRANSFORM_H__
