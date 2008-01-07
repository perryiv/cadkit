
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

#ifndef __STAR_SYSTEM_BUILD_TILES_H__
#define __STAR_SYSTEM_BUILD_TILES_H__

#include "StarSystem/Export.h"
#include "StarSystem/Tile.h"

#include "Usul/Jobs/Job.h"

namespace StarSystem {
  
class STAR_SYSTEM_EXPORT BuildTiles : public Usul::Jobs::Job
{
public:
  typedef Usul::Jobs::Job BaseClass;
  
  BuildTiles ( Tile::RefPtr tile );
  
protected:
  virtual ~BuildTiles();
  
  virtual void _started();
  
private:
  Tile::RefPtr _tile;
};

  
}

#endif // __STAR_SYSTEM_BUILD_TILES_H__
