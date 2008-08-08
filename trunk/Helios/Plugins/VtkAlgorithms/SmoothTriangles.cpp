
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Component class to smooth triangles.
//
///////////////////////////////////////////////////////////////////////////////

#include "SmoothTriangles.h"
#include "Convert.h"

#include "Usul/System/Clock.h"
#include "Usul/Errors/Assert.h"

#include "OsgTools/Triangles/TriangleSet.h"

#include "osg/Array"
#include "osg/PrimitiveSet"

#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkWindowedSincPolyDataFilter.h" // Smoothing algorithm


///////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////////////////////////

SmoothTriangles::SmoothTriangles() 
{
}


///////////////////////////////////////////////////////////////////////////////
//
// Destructor
//
///////////////////////////////////////////////////////////////////////////////

SmoothTriangles::~SmoothTriangles() 
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Smooth the triangle set.
//
///////////////////////////////////////////////////////////////////////////////

void SmoothTriangles::operator() ( OsgTools::Triangles::TriangleSet *triangleSet, unsigned int numIteration )
{
  if ( 0x0 == triangleSet )
    return;

  vtkPolyData *data ( vtkPolyData::New() );

  double start ( Usul::System::Clock::milliseconds() );
  VTKTools::Convert::triangleSetToPolyData( triangleSet, data );
  ::printf ( "%8.4f seconds .... Time to convert to polydata.\n", static_cast < double > ( Usul::System::Clock::milliseconds() - start ) * 0.001 ); ::fflush ( stdout );

  start = Usul::System::Clock::milliseconds();
  vtkWindowedSincPolyDataFilter *smooth	= vtkWindowedSincPolyDataFilter::New();
  smooth->SetInput ( data );
  smooth->SetNumberOfIterations( 50 );
  smooth->Update();
  vtkPolyData *nData ( smooth->GetOutput() );
  ::printf ( "%8.4f seconds .... Time to smooth.\n", static_cast < double > ( Usul::System::Clock::milliseconds() - start ) * 0.001 ); ::fflush ( stdout );

  start = Usul::System::Clock::milliseconds();
  VTKTools::Convert::polyDataToTriangleSet ( nData, triangleSet );
  ::printf ( "%8.4f seconds .... Time to convert to triangle set.\n", static_cast < double > ( Usul::System::Clock::milliseconds() - start ) * 0.001 ); ::fflush ( stdout );

  smooth->Delete();
  data->Delete();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Smooth the vertices.
//
///////////////////////////////////////////////////////////////////////////////

void SmoothTriangles::operator() ( osg::Array *v, osg::DrawElementsUInt *indices,
                                   osg::Array *nT, osg::Array *nV, unsigned int numIterations )
{
  osg::ref_ptr < osg::Vec3Array > vertices ( dynamic_cast < osg::Vec3Array * > ( v ) );
  osg::ref_ptr < osg::Vec3Array > normalsT ( dynamic_cast < osg::Vec3Array * > ( nT ) );
  osg::ref_ptr < osg::Vec3Array > normalsV ( dynamic_cast < osg::Vec3Array * > ( nV ) );
  if ( ( 0x0 == indices ) || ( false == vertices.valid() ) || ( false == normalsT.valid() ) || ( false == normalsV.valid() ) )
    return;

  vtkSmartPointer < vtkPolyData > data ( vtkPolyData::New() );
  VTKTools::Convert::verticesToPolyData ( vertices.get(), indices, data );

  vtkSmartPointer < vtkWindowedSincPolyDataFilter > smooth ( vtkWindowedSincPolyDataFilter::New() );
  smooth->SetInput ( data );
  smooth->SetNumberOfIterations ( numIterations );
  smooth->Update();
  vtkSmartPointer < vtkPolyData > nData ( smooth->GetOutput() );

  VTKTools::Convert::polyDataToVertices ( nData, *vertices, *indices, *normalsT, *normalsV );
}
