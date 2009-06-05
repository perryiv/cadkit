
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

#ifndef __DECIMATETRIANGLESCOMPONENT_H__
#define __DECIMATETRIANGLESCOMPONENT_H__

namespace Usul { namespace Interfaces { struct IUnknown; } }
namespace OsgTools { namespace Triangles { class TriangleSet; } }
namespace osg { class Array; class DrawElementsUInt; }

class DecimateTriangles
{
public:
  
  /// Constructor/Destructor.
  DecimateTriangles();
	~DecimateTriangles();

  void operator() ( osg::Array *inVerts, osg::DrawElementsUInt *inIndices,
                    osg::Array *outNormalsT, osg::Array *outNormalsV, float reduction,
                    Usul::Interfaces::IUnknown* caller = 0x0 );

  void operator() ( OsgTools::Triangles::TriangleSet*, float reduction, Usul::Interfaces::IUnknown* caller = 0x0 );
};

#endif /* __DECIMATETRIANGLESCOMPONENT_H__  */		
	
