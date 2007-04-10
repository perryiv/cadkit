
#ifndef __VTKUTIL_VTK_TO_OSG_H__
#define __VTKUTIL_VTK_TO_OSG_H__

//#define VTK5 

#ifdef VTK5
	#define VTK4
		typedef double vtkReal;
	#else
		typedef float vtkReal;
#endif

#include "vtkActor.h"
#include "vtkPolyDataMapper.h"
#include "vtkCellArray.h"

#include "Export.h"

namespace osg
{
	class Geode;
	class Geometry;
}

namespace SimFlow
{
	namespace VTKUtil
	{
		class VTKUTIL_EXPORT VTKToOSG
		{
			public:
				///////////////////////////////////////////////////////////////////////
				//
				// Transform vtkActor to osg::Geode.  
				//
				///////////////////////////////////////////////////////////////////////
				static osg::Geode* vtkActorToOSG( vtkActor* actor, int verbose=0 );

				///////////////////////////////////////////////////////////////////////
				//
				// Process primitives in a vtk actor and return a osg::Geometry object.
				//
				///////////////////////////////////////////////////////////////////////
				static osg::Geometry* processPrimitive( vtkActor *actor, vtkCellArray *prims, int pType, int v );
		};
	}
}

#endif // __VTKUTIL_VTK_TO_OSG_H__

