
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

#ifndef __USUL_INTERFACES_IDECIMATETRIANGLES_H__
#define __USUL_INTERFACES_IDECIMATETRIANGLES_H__

#include "Usul/Interfaces/IUnknown.h"

namespace OsgTools { namespace Triangles { class TriangleSet; } }

namespace osg { class Array; class DrawElementsUInt; }

namespace Usul {
namespace Interfaces {


struct IDecimateTriangles : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IDecimateTriangles );

  /// Id for this interface.
  enum { IID = 1321132304u };

  virtual void decimateTriangles ( osg::Array *inVerts, osg::DrawElementsUInt *inIndices,
                                   osg::Array *outNormalsT, osg::Array *outNormalsV, float reduction ) = 0;

  virtual void decimateTriangles ( OsgTools::Triangles::TriangleSet*, float reduction ) = 0;

}; // struct IDecimateTriangles


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_IDECIMATETRIANGLES_H__ */
