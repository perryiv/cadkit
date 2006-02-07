
#ifndef __VTK_TOOLS_CONVERT_POLY_DATA_H__
#define __VTK_TOOLS_CONVERT_POLY_DATA_H__

#include "VTKTools/Export/Export.h"

class vtkPolyData;
#include "vtkSmartPointer.h"

namespace OsgTools { namespace Triangles { class TriangleSet; } }

#include <vector>

namespace VTKTools {
namespace Convert {

class VTK_TOOLS_EXPORT PolyData
{
public:
  /// Typedefs.
  typedef vtkSmartPointer < vtkPolyData > PolyDataPtr;
  typedef std::vector< PolyDataPtr > PolyDataVector;

  /// Convert vtkPolyData to the TriangleSet.
  static void polyDataToTriangleSet ( vtkPolyData*, OsgTools::Triangles::TriangleSet* );

  /// Convert a vector vtkPolyData to the TriangleSet.  Each poly data will be it's own group.
  static void polyDataToTriangleSet ( PolyDataVector &, OsgTools::Triangles::TriangleSet* );

  /// Convert a TriangleSet to vtkPolyData.
  static void triangleSetToPolyData ( OsgTools::Triangles::TriangleSet*, vtkPolyData* );

  /// Convert a TriangleSet to a vector of vtkPolyData.  Each poly data will be from a group.
  static void triangleSetToPolyData ( OsgTools::Triangles::TriangleSet*, PolyDataVector & );
};

}
}

#endif // __VTK_TOOLS_CONVERT_POLY_DATA_H__

