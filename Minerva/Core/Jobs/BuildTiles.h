
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Build four sub-tiles.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_JOBS_BUILD_TILES_H__
#define __MINERVA_CORE_JOBS_BUILD_TILES_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/TileEngine/Tile.h"

#include "Usul/Jobs/Job.h"


namespace Minerva {
namespace Core {
namespace Jobs {
  

class MINERVA_EXPORT BuildTiles : public Usul::Jobs::Job
{
public:

  typedef Usul::Jobs::Job BaseClass;
  typedef Minerva::Core::TileEngine::Tile Tile;

  // Type information.
  USUL_DECLARE_TYPE_ID ( BuildTiles );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( BuildTiles );
  
  BuildTiles ( Tile::RefPtr tile );

  virtual bool        success() const;

protected:

  virtual ~BuildTiles();
  
  virtual void        _started();
  
private:

  Tile::RefPtr _tile;
  bool _success;
};

  
} // namespace Jobs
} // namespace Core
} // namespace Minerva


#endif // __MINERVA_CORE_JOBS_BUILD_TILES_H__
