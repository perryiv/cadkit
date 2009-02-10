
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Job to build elevation.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MINERVA_CORE_JOBS_BUILD_ELEVATION_CLASS_H_
#define _MINERVA_CORE_JOBS_BUILD_ELEVATION_CLASS_H_

#include "Minerva/Core/TileEngine/Tile.h"

#include "Usul/Jobs/Job.h"

namespace Minerva {
namespace Core {
namespace Jobs {


class BuildElevation : public Usul::Jobs::Job
{
public:

  typedef Usul::Jobs::Job BaseClass;
  typedef Minerva::Core::TileEngine::Tile Tile;
  
  // Type information.
  USUL_DECLARE_TYPE_ID ( BuildElevation );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( BuildElevation );

  BuildElevation ( Tile::RefPtr tile );

protected:

  virtual ~BuildElevation();

  virtual void      _started();

private:

  Tile::RefPtr _tile;
};


} // namespace Jobs
} // namespace Core
} // namespace Minerva


#endif // _MINERVA_CORE_JOBS_BUILD_ELEVATION_CLASS_H_
