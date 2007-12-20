
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

#include "Usul/Jobs/Job.h"

namespace StarSystem {

  class Tile;
  
class STAR_SYSTEM_EXPORT BuildTiles : public Usul::Jobs::Job
{
public:
  typedef Usul::Jobs::Job BaseClass;
  
  BuildTiles ( Tile* tile );
  
protected:
  virtual ~BuildTiles();
  
  virtual void _started();
  
private:
  Tile* _tile;
};

  
}

#endif // __STAR_SYSTEM_BUILD_TILES_H__
