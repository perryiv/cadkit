
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for smoothing triangles.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_SMOOTH_TRIANGLES_H_
#define _USUL_INTERFACE_SMOOTH_TRIANGLES_H_

#include "Usul/Interfaces/IUnknown.h"

namespace OsgTools { namespace Triangles { class TriangleSet; } }

namespace Usul {
namespace Interfaces {


struct ISmoothTriangles : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ISmoothTriangles );

  /// Id for this interface.
  enum { IID = 1100046596u };

  virtual void smoothTriangles ( OsgTools::Triangles::TriangleSet* ) = 0;

}; // class ISmoothTriangles


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_SMOOTH_TRIANGLES_H_
