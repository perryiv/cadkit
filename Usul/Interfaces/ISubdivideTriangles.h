
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_ISUBDIVIDETRIANGLES_H__
#define __USUL_INTERFACES_ISUBDIVIDETRIANGLES_H__

#include "Usul/Interfaces/IUnknown.h"

namespace OsgTools { namespace Triangles { class TriangleSet; } }

namespace Usul {
namespace Interfaces {


struct ISubdivideTriangles : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ISubdivideTriangles );

  /// Id for this interface.
  enum { IID = 1136432950u };

  virtual void subdivideTriangles ( OsgTools::Triangles::TriangleSet *triangleSet ) = 0;

}; // struct ISubdivideTriangles


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_ISUBDIVIDETRIANGLES_H__ */
