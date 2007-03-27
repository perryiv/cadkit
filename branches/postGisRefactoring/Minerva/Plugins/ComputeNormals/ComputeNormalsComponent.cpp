
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

#include "Minerva/Plugins/ComputeNormals/ComputeNormalsComponent.h"

#include "vtkSmartPointer.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkTriangle.h"
#include "vtkPolyData.h"
#include "vtkPolyDataNormals.h"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( ComputeNormalsComponent, ComputeNormalsComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////////////////////////

ComputeNormalsComponent::ComputeNormalsComponent()
{
}


///////////////////////////////////////////////////////////////////////////////
//
// Destructor
//
///////////////////////////////////////////////////////////////////////////////

ComputeNormalsComponent::~ComputeNormalsComponent() 
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *ComputeNormalsComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IComputeTriangleNormals::IID:
    return static_cast < Usul::Interfaces::IComputeTriangleNormals*>(this);
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

void ComputeNormalsComponent::computeNormalsPerTriangle ( const Vertices& vertices, Normals& normals )
{
  unsigned int numVertices ( vertices.size() );
  unsigned int numTriangles ( vertices.size() / 3 );

  /*vtkSmartPointer < vtkPoints > points  ( vtkPoints::New() );
  points->SetDataTypeToDouble();
  points->GetData()->SetNumberOfComponents(3);
  points->Allocate ( numVertices );


  vtkSmartPointer < vtkCellArray > tris ( vtkCellArray::New() );
  tris->Initialize();
  tris->Allocate ( numTriangles );

  for ( Vertices::const_iterator iter = vertices.begin(); iter != vertices.end(); iter += 3 )
  {
    points->InsertNextPoint ( &(*iter)[0] );
    points->InsertNextPoint ( &(*iter)[1] );
    points->InsertNextPoint ( &(*iter)[2] );

    tris->InsertNextCell( 3 );
  }

  vtkSmartPointer < vtkPolyData > polyData ( vtkPolyData::New() );
  polyData->SetPoints( points.GetPointer() );
  polyData->SetPolys( tris.GetPointer() );

  vtkSmartPointer < vtkPolyDataNormals > polyDataNormals ( vtkPolyDataNormals::New () );
  polyDataNormals->ComputeCellNormalsOn ();
  polyDataNormals->SplittingOff();
  polyDataNormals->SetInput( polyData.GetPointer() );
  polyDataNormals->Update();

  vtkSmartPointer < vtkPolyData > newPolyData ( polyDataNormals->GetOutput() );

  vtkSmartPointer < vtkDataArray > newNormals ( newPolyData->GetCellData()->GetNormals() );
  unsigned int numNormals ( newNormals->GetNumberOfTuples() );*/

  // Clear.
  normals.clear();

  // Make room.
  normals.reserve ( numTriangles );

  /*for ( unsigned int i = 0; i < numNormals; ++i )
  {
    double *normal = newNormals->GetTuple( i );
    normals.push_back( normal );
  }

  vtkIdType numPoints ( 3 );
  vtkIdType ids [3];*/

  for( Vertices::const_iterator iter = vertices.begin(); iter != vertices.end(); iter += 3)
  {
    Vertices::value_type v0 ( *iter );
    Vertices::value_type v1 ( *(iter + 1 ) );
    Vertices::value_type v2 ( *(iter + 2 ) );

    Normals::value_type normal ( 0.0, 0.0, 0.0 );
    vtkTriangle::ComputeNormal( &v0[0], &v1[0], &v2[0], &normal[0] );

    // Get the center of the triangle.
    Vertices::value_type center ( v0 + v1 + v2 ); 
    center /= 3;

    // Get the center of the model.
    Vertices::value_type cg ( 0.0, 0.0, 0.0 );

    // Make the reference vector.
    Vertices::value_type refVector ( center - cg );

    // Flip the normal if it's pointing towards the center. 
    // The '*' operator is overloaded as the dot product for osg::Vec3.
    if ( ( refVector.dot ( normal ) ) < 0 )
      normal = -normal;

    // Add the normal.
    normals.push_back ( normal );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Usul::Interfaces::IPlugin implementation
//
///////////////////////////////////////////////////////////////////////////////

std::string ComputeNormalsComponent::getPluginName() const 
{
  return "Compute Normals";
}
