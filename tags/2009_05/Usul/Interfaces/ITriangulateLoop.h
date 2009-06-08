
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

#ifndef __USUL_INTERFACES_TRIANGULATE_LOOP_H__
#define __USUL_INTERFACES_TRIANGULATE_LOOP_H__

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Math/Vector2.h"

#include <vector>
#include <list>

namespace Usul {
namespace Interfaces {


struct ITriangulateLoop : public Usul::Interfaces::IUnknown
{
  // Useful typedefs.
  typedef std::vector< Usul::Math::Vec2d >  Vertices;
  typedef std::vector< unsigned int >       UIntArray;
  typedef std::list  < Vertices >           InnerLoops;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ITriangulateLoop );

  /// Id for this interface.
  enum { IID = 3661762526u };

  // Triangulate the loop.
  virtual void triangulateLoop ( const Vertices &in, InnerLoops& inner, Vertices &out, UIntArray& indices ) = 0;
};


}
}


#endif // __USUL_INTERFACES_TRIANGULATE_H__
