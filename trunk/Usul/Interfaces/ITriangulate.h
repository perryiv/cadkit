
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Mike Jackson
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_TRIANGULATE_H__
#define __USUL_INTERFACES_TRIANGULATE_H__

#include "Usul/Interfaces/IUnknown.h"
#include "osg/Array"
#include "Usul/Math/Vector2.h"


namespace osg { class Array; }

namespace Usul {
namespace Interfaces {


struct ITriangulate : public Usul::Interfaces::IUnknown
{

  typedef std::vector<Usul::Math::Vec2d> Vertices;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ITriangulate );

  /// Id for this interface.
  enum { IID = 3661762526u };

  // Get normal for the i'th vertex.
  virtual void triangulate( const Vertices &inVertices, Vertices &outVertices ) = 0;


}; //struct __USUL_INTERFACES_TRIANGULATE_H__



}
}


#endif // __USUL_INTERFACES_TRIANGULATE_H__
