
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_INTERFACES_IFIT_LINE_TERRAIN_H__
#define __MINERVA_INTERFACES_IFIT_LINE_TERRAIN_H__

#include "Usul/Interfaces/IUnknown.h"

#include "Usul/Math/Vector3.h"

#include <vector>

namespace Minerva {
namespace Interfaces {


struct IFitLineTerrain : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IFitLineTerrain );

  /// Id for this interface.
  enum { IID = 3444504269u };

  typedef Usul::Math::Vec3d                Vertex;
  typedef std::vector < Vertex >           Vertices;

  virtual void resample ( const Vertices& input, Vertices& output, unsigned int maximumDepth, Usul::Interfaces::IUnknown *caller = 0x0 ) = 0;

}; // struct IFitLineTerrain


} // End namespace Interfaces
} // End namespace Minerva

#endif // __MINERVA_INTERFACES_IFIT_LINE_TERRAIN_H__
