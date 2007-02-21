
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Plugins/TriangulateVTK/TriangulateComponent.h"

#include "vtkSmartPointer.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkDelaunay2D.h"
#include "vtkTriangle.h"
#include "vtkPolyDataMapper.h"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( TriangulateComponent, TriangulateComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////////////////////////

TriangulateComponent::TriangulateComponent()
{
}


///////////////////////////////////////////////////////////////////////////////
//
// Destructor
//
///////////////////////////////////////////////////////////////////////////////

TriangulateComponent::~TriangulateComponent() 
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *TriangulateComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::ITriangulate::IID:
    return static_cast < Usul::Interfaces::ITriangulate*>(this);
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin* > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void TriangulateComponent::triangulate ( const Vertices &in, Vertices &out, Vertices3D &normalsOut )
{
  vtkSmartPointer < vtkPoints > points  ( vtkPoints::New() );
  points->SetDataTypeToDouble();
  points->GetData()->SetNumberOfComponents(3);
  points->Allocate ( in.size() );

  for( Vertices::const_iterator iter = in.begin(); iter != in.end(); ++iter )
    points->InsertNextPoint( (*iter)[0], (*iter)[1], 0.0 );

  vtkSmartPointer < vtkCellArray > cellArray ( vtkCellArray::New() );
  cellArray->InsertNextCell( in.size() );

  for( unsigned int i = 0; i < in.size(); ++i )
    cellArray->InsertCellPoint ( i );

  vtkSmartPointer < vtkPolyData > polyData ( vtkPolyData::New() );
  polyData->SetPoints( points.GetPointer() );
  polyData->SetPolys( cellArray.GetPointer() );

  vtkSmartPointer < vtkDelaunay2D > delaunay ( vtkDelaunay2D::New() );
  delaunay->SetInput( polyData.GetPointer() );
  delaunay->SetSource( polyData.GetPointer() );

  delaunay->Update();

  // Clear what we have
  out.clear();
  normalsOut.clear();

  // Get the results.
  vtkSmartPointer < vtkPolyData > newPolys ( delaunay->GetOutput() );
  vtkSmartPointer < vtkPoints > newPoints ( newPolys->GetPoints() );
  vtkSmartPointer < vtkCellArray > newTris ( newPolys->GetPolys() );

  const unsigned int numTriangles ( newTris->GetNumberOfCells() );
  const unsigned int numVertices ( numTriangles * 3 );

  // Build the container of vertex-usage counts.
  vtkIdType numPoints ( 0 );
  vtkIdType* ids ( 0x0 );

  // Make room.
  out.reserve( numVertices );
  normalsOut.reserve ( numTriangles );

  // Loop throught the triangles.
  for ( newTris->InitTraversal(); newTris->GetNextCell( numPoints,ids ); )
  {
    USUL_ASSERT ( numPoints == 3 );

    unsigned int i0 ( ids[0] );
    unsigned int i1 ( ids[1] );
    unsigned int i2 ( ids[2] );

    double *d0 ( points->GetPoint ( i0 ) );
    double *d1 ( points->GetPoint ( i1 ) );
    double *d2 ( points->GetPoint ( i2 ) );

    Usul::Math::Vec2d v0 ( d0[0], d0[1] );
    Usul::Math::Vec2d v1 ( d1[0], d1[1] );
    Usul::Math::Vec2d v2 ( d2[0], d2[1] );

    // Add the vertices.
    out.push_back( v0 );
    out.push_back( v1 );
    out.push_back( v2 );

    // Add the normal.
    normalsOut.push_back ( Usul::Math::Vec3d ( 0.0, 0.0, 1.0 ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void TriangulateComponent::triangulate3D ( const Vertices3D &in, Vertices3D &out, Vertices3D &normalsOut )
{
  vtkSmartPointer < vtkPoints > points  ( vtkPoints::New() );
  points->SetDataTypeToDouble();
  points->GetData()->SetNumberOfComponents(3);
  points->Allocate ( in.size() );

  for( Vertices3D::const_iterator iter = in.begin(); iter != in.end(); ++iter )
    points->InsertNextPoint( (*iter)[0], (*iter)[1], (*iter)[2] );

  vtkSmartPointer < vtkCellArray > cellArray ( vtkCellArray::New() );
  cellArray->InsertNextCell( in.size() );

  for( unsigned int i = 0; i < in.size(); ++i )
    cellArray->InsertCellPoint ( i );

  vtkSmartPointer < vtkPolyData > polyData ( vtkPolyData::New() );
  polyData->SetPoints( points.GetPointer() );
  polyData->SetPolys( cellArray.GetPointer() );

  vtkSmartPointer < vtkDelaunay2D > delaunay ( vtkDelaunay2D::New() );
  delaunay->SetInput( polyData.GetPointer() );
  delaunay->SetSource( polyData.GetPointer() );

  // Clear what we have
  out.clear();
  normalsOut.clear();

  // Get the results.
  vtkSmartPointer < vtkPolyData > newPolys ( delaunay->GetOutput() );
  vtkSmartPointer < vtkPoints > newPoints ( newPolys->GetPoints() );
  vtkSmartPointer < vtkCellArray > newTris ( newPolys->GetPolys() );

  const unsigned int numTriangles ( newTris->GetNumberOfCells() );
  const unsigned int numVertices ( numTriangles * 3 );

  // Build the container of vertex-usage counts.
  vtkIdType numPoints ( 0 );
  vtkIdType* ids ( 0x0 );

  // Make room.
  out.reserve( numVertices );
  normalsOut.reserve ( numTriangles );

  // Loop throught the triangles.
  for ( newTris->InitTraversal(); newTris->GetNextCell( numPoints,ids ); )
  {
    USUL_ASSERT ( numPoints == 3 );

    unsigned int i0 ( ids[0] );
    unsigned int i1 ( ids[1] );
    unsigned int i2 ( ids[2] );

    double *d0 ( points->GetPoint ( i0 ) );
    double *d1 ( points->GetPoint ( i1 ) );
    double *d2 ( points->GetPoint ( i2 ) );

    Usul::Math::Vec3d v0 ( d0[0], d0[1], d0[2] );
    Usul::Math::Vec3d v1 ( d1[0], d1[1], d1[2] );
    Usul::Math::Vec3d v2 ( d2[0], d2[1], d2[2] );

    /// Compute the normal.
    double normal[3];
    vtkTriangle::ComputeNormal( v0.get(), v1.get(), v2.get(), normal );

    // Add the vertices.
    out.push_back( v0 );
    out.push_back( v1 );
    out.push_back( v2 );

    // Add the normal.
    normalsOut.push_back ( normal );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Usul::Interfaces::IPlugin implementation
//
///////////////////////////////////////////////////////////////////////////////

std::string TriangulateComponent::getPluginName() const 
{
  return "Triangulate";
}
