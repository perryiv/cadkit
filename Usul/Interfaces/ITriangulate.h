
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Mike Jackson
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
// Interface for triangulating a 2D loop of vertices.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_TRIANGULATE_H__
#define __USUL_INTERFACES_TRIANGULATE_H__

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Math/Vector2.h"

#include <vector>


namespace Usul {
namespace Interfaces {


struct ITriangulate : public Usul::Interfaces::IUnknown
{
  // Useful typedef(s).
  typedef std::vector<Usul::Math::Vec2d> Vertices;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ITriangulate );

  /// Id for this interface.
  enum { IID = 3661762526u };

  // Triangulate the loop.
  virtual void triangulate ( const Vertices &in, Vertices &out ) = 0;
};


}
}


#endif // __USUL_INTERFACES_TRIANGULATE_H__
