
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Component class to smooth triangles
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __SMOOTH_TRIANGLES_COMPONENT_H__
#define __SMOOTH_TRIANGLES_COMPONENT_H__

namespace OsgTools { namespace Triangles { class TriangleSet; } }
namespace osg { class Array; class DrawElementsUInt; }

class SmoothTriangles
{
public:
  
  /// Constructor/Destructor.
  SmoothTriangles();
  ~SmoothTriangles();

  void operator() ( osg::Array *vertices, osg::DrawElementsUInt *indices,
                    osg::Array *normalsT, osg::Array *normalsV, unsigned int numIterations );

  void operator() ( OsgTools::Triangles::TriangleSet*, unsigned int numIteration );
};

#endif /* __SMOOTH_TRIANGLES_COMPONENT_H__  */		
	
