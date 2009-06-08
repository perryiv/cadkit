
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Job to fetch an image.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MINERVA_CORE_JOBS_BUILD_RASTER_CLASS_H_
#define _MINERVA_CORE_JOBS_BUILD_RASTER_CLASS_H_

#include "Minerva/Core/TileEngine/Tile.h"

#include "Usul/Jobs/Job.h"

namespace Minerva {
namespace Core {
namespace Jobs {


class BuildRaster : public Usul::Jobs::Job
{
public:

  typedef Usul::Jobs::Job BaseClass;
  typedef Minerva::Core::TileEngine::Tile Tile;
  
  // Type information.
  USUL_DECLARE_TYPE_ID ( BuildRaster );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( BuildRaster );

  BuildRaster ( Tile::RefPtr tile );

protected:

  virtual ~BuildRaster();

  virtual void      _started();

private:

  Tile::RefPtr _tile;
};


} // namespace Jobs
} // namespace Core
} // namespace Minerva


#endif // _MINERVA_CORE_JOBS_BUILD_RASTER_CLASS_H_
