
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

#include "SubdivideTriangles.h"
#include "Convert.h"
#include "Progress.h"

#include "Usul/System/Clock.h"
#include "Usul/Resources/ProgressBar.h"

#include "OsgTools/Triangles/TriangleSet.h"

#include "osg/Array"
#include "osg/PrimitiveSet"

#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkLoopSubdivisionFilter.h"


///////////////////////////////////////////////////////////////////////////////
//
// Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SubdivideTriangles::SubdivideTriangles() 
{
}


///////////////////////////////////////////////////////////////////////////////
//
// Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SubdivideTriangles::~SubdivideTriangles() 
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Subdivide the triangles.
//
///////////////////////////////////////////////////////////////////////////////

void SubdivideTriangles::operator() ( OsgTools::Triangles::TriangleSet *triangleSet, unsigned int numSubdivisions )
{
  vtkSmartPointer < vtkPolyData > data ( vtkPolyData::New() );

  VTKTools::Convert::triangleSetToPolyData( triangleSet, data );
  
  vtkSmartPointer < vtkLoopSubdivisionFilter > subdivide	( vtkLoopSubdivisionFilter::New() );
  subdivide->SetInput ( data );
  subdivide->SetNumberOfSubdivisions( numSubdivisions );

  vtkSmartPointer < VTKTools::Progress > progress ( new VTKTools::Progress );
  progress->progressBar ( Usul::Resources::progressBar() );

  subdivide->AddObserver( vtkCommand::StartEvent, progress.GetPointer() );
  subdivide->AddObserver( vtkCommand::ProgressEvent, progress.GetPointer() );
  subdivide->AddObserver( vtkCommand::EndEvent, progress.GetPointer() );

  subdivide->Update();
  
  vtkSmartPointer < vtkPolyData > nData ( subdivide->GetOutput() );

  VTKTools::Convert::polyDataToTriangleSet ( nData, triangleSet );

  subdivide->RemoveObservers( vtkCommand::StartEvent );
  subdivide->RemoveObservers( vtkCommand::ProgressEvent );
  subdivide->RemoveObservers( vtkCommand::EndEvent );

  // Why do I need this extra un register??
  progress->UnRegister();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Subdivide the vertices.
//
///////////////////////////////////////////////////////////////////////////////

void SubdivideTriangles::operator() ( osg::Array *v, osg::DrawElementsUInt *indices,
                                      osg::Array *nT, osg::Array *nV, unsigned int numSubdivisions )
{
  osg::ref_ptr< osg::Vec3Array > vertices ( dynamic_cast < osg::Vec3Array * > ( v ) );

  vtkSmartPointer < vtkPolyData > data ( vtkPolyData::New() );

  VTKTools::Convert::verticesToPolyData( vertices.get(), indices, data );
  
  vtkSmartPointer < vtkLoopSubdivisionFilter > subdivide	( vtkLoopSubdivisionFilter::New() );
  subdivide->SetInput ( data );
  subdivide->SetNumberOfSubdivisions( numSubdivisions );

  vtkSmartPointer < VTKTools::Progress > progress ( new VTKTools::Progress );
  progress->progressBar ( Usul::Resources::progressBar() );

  subdivide->AddObserver( vtkCommand::StartEvent, progress.GetPointer() );
  subdivide->AddObserver( vtkCommand::ProgressEvent, progress.GetPointer() );
  subdivide->AddObserver( vtkCommand::EndEvent, progress.GetPointer() );

  subdivide->Update();
  
  vtkSmartPointer < vtkPolyData > nData ( subdivide->GetOutput() );

  osg::ref_ptr< osg::Vec3Array > normalsT ( dynamic_cast < osg::Vec3Array * > ( nT ) );
  osg::ref_ptr< osg::Vec3Array > normalsV ( dynamic_cast < osg::Vec3Array * > ( nV ) );

  VTKTools::Convert::polyDataToVertices ( nData, *vertices, *indices, *normalsT, *normalsV );

  subdivide->RemoveObservers( vtkCommand::StartEvent );
  subdivide->RemoveObservers( vtkCommand::ProgressEvent );
  subdivide->RemoveObservers( vtkCommand::EndEvent );

  // Why do I need this extra un register??
  progress->UnRegister();
}
