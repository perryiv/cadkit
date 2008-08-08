
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __VTK_TOOLS_CONVERT_POLY_DATA_H__
#define __VTK_TOOLS_CONVERT_POLY_DATA_H__

class vtkPolyData;
#include "vtkSmartPointer.h"

#include "osg/Array"

#include <vector>

namespace OsgTools { namespace Triangles { class TriangleSet; } }
namespace osg { class DrawElementsUInt; }

namespace VTKTools {

class Convert
{
public:

  /// Convert vtkPolyData to the TriangleSet.
  static void polyDataToTriangleSet ( vtkPolyData*, OsgTools::Triangles::TriangleSet* );

  /// Convert a TriangleSet to vtkPolyData.
  static void triangleSetToPolyData ( OsgTools::Triangles::TriangleSet*, vtkPolyData* );

  /// Convert an osg::Vec3Array and DrawElements to a vtkPolyData
  static void verticesToPolyData ( const osg::Vec3Array *vertices, const osg::DrawElementsUInt *indices, vtkPolyData *data);

  /// Convert vtkPolyData to osg data.
  static void polyDataToVertices ( vtkPolyData *data, 
                                   osg::Vec3Array &vertices, 
                                   osg::DrawElementsUInt &indices, 
                                   osg::Vec3Array &normalsT, 
                                   osg::Vec3Array &normalsV );
};

}

#endif // __VTK_TOOLS_CONVERT_POLY_DATA_H__
