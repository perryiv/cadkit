
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

#ifndef __USUL_INTERFACES_ICALCULATEPERVERTEXNORMALS_H__
#define __USUL_INTERFACES_ICALCULATEPERVERTEXNORMALS_H__

#include "Usul/Interfaces/IUnknown.h"

namespace osg { class Array; class DrawElementsUInt; }
namespace OsgTools { namespace Triangles { class TriangleSet; } }

namespace Usul {
namespace Interfaces {


struct ICalculatePerVertexNormals : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ICalculatePerVertexNormals );

  /// Id for this interface.
  enum { IID = 1708593750 };

  virtual void    calculatePerVertexNormals ( osg::Array *vertices, osg::DrawElementsUInt *elements, osg::Array *normalsT, osg::Array *normalsV ) = 0;
  virtual void    calculatePerVertexNormals ( OsgTools::Triangles::TriangleSet *triangleSet ) = 0;

}; // struct ICalculatePerVertexNormals


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_ICALCULATEPERVERTEXNORMALS_H__ */
