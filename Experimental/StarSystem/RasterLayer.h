
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __STAR_SYSTEM_RASTER_LAYER_H__
#define __STAR_SYSTEM_RASTER_LAYER_H__

#include "StarSystem/Export.h"
#include "StarSystem/Extents.h"

#include "Usul/Base/Object.h"

#include "osg/Vec2d"

namespace osg { class Image; class Vec3d; }

namespace StarSystem {


class STAR_SYSTEM_EXPORT RasterLayer : public Usul::Base::Object
{
public:

  typedef Usul::Base::Object BaseClass;
  typedef StarSystem::Extents < osg::Vec2d > Extents;

  USUL_DECLARE_REF_POINTERS ( RasterLayer );

  RasterLayer();

  /// Get/Set the extents.
  void                  extents ( const Extents& extents );
  Extents               extents () const;

  /// Get the texture
  virtual osg::Image*   texture ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level ) = 0;

protected:
  virtual ~RasterLayer();

private:
  Extents _extents;
};


}

#endif // __STAR_SYSTEM_RASTER_LAYER_H__
