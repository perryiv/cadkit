
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

#ifndef __SUBDIVIDE_TRIANGLES_H__
#define __SUBDIVIDE_TRIANGLES_H__

namespace OsgTools { namespace Triangles { class TriangleSet; } }
namespace osg { class Array; class DrawElementsUInt; }

class SubdivideTriangles
{
public:
  
  /// Constructor/Destructor.
  SubdivideTriangles();
	~SubdivideTriangles();

  void operator() ( osg::Array *vertices, osg::DrawElementsUInt *indices,
                    osg::Array *normalsT, osg::Array *normalsV, unsigned int numIterations );

  void operator() ( OsgTools::Triangles::TriangleSet *triangleSet, unsigned int numSubdivisions );
};

#endif /* __SUBDIVIDE_TRIANGLES_H__  */		
