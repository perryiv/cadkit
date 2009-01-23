
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_INTERFACES_ITILE_H__
#define __MINERVA_INTERFACES_ITILE_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Minerva { namespace Core { namespace TileEngine { class Tile; } } }


namespace Minerva {
namespace Interfaces {


struct ITile : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ITile );

  /// Id for this interface.
  enum { IID = 2877982655u };

  /// Get the tile.
  virtual Minerva::Core::TileEngine::Tile *     tile() = 0;
};


} // end namespace Interfaces
} // end namespace Minerva


#endif // __MINERVA_INTERFACES_ITILE_H__ 
