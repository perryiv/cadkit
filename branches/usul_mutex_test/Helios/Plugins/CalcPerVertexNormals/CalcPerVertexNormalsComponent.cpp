
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


#include "CalcPerVertexNormalsComponent.h"

#include "VTKTools/Convert/PolyData.h"

#include "OsgTools/Triangles/TriangleSet.h"

#include "osg/Array"
#include "osg/PrimitiveSet"

#include "vtkPolyData.h"
#include "vtkPolyDataNormals.h"
#include "vtkCellData.h"
#include "vtkPointData.h"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( CalcPerVertexNormalsComponent , CalcPerVertexNormalsComponent::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////////////////////////

CalcPerVertexNormalsComponent::CalcPerVertexNormalsComponent() 
{
}

///////////////////////////////////////////////////////////////////////////////
//
// Destructor
//
///////////////////////////////////////////////////////////////////////////////

CalcPerVertexNormalsComponent::~CalcPerVertexNormalsComponent() 
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *CalcPerVertexNormalsComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin* > ( this );
  case Usul::Interfaces::ICalculatePerVertexNormals::IID:
    return static_cast < Usul::Interfaces::ICalculatePerVertexNormals* > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Usul/Interfaces/IPlugin.h implementation
//
///////////////////////////////////////////////////////////////////////////////

std::string CalcPerVertexNormalsComponent::getPluginName() const 
{
  return "CalcPerVertexNormals";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create per vertex normals.
//
///////////////////////////////////////////////////////////////////////////////

void CalcPerVertexNormalsComponent::calculatePerVertexNormals ( osg::Array *v, osg::DrawElementsUInt *elements, osg::Array *nT, osg::Array *nV )
{
  osg::ref_ptr< osg::Vec3Array > vertices ( dynamic_cast < osg::Vec3Array * > ( v ) );

  vtkSmartPointer < vtkPolyData > data ( vtkPolyData::New() );

  VTKTools::Convert::PolyData::verticesToPolyData( vertices.get(), elements, data );

#if 0
  vtkSmartPointer < vtkPolyDataNormals > calcNormals ( vtkPolyDataNormals::New() );
  calcNormals->SetInput ( data.GetPointer() );

  calcNormals->Update();

  vtkSmartPointer < vtkPolyData > nData ( calcNormals->GetOutput() );
  calcNormals->ComputePointNormalsOn ();
  calcNormals->SplittingOff ();
  calcNormals->ConsistencyOff ();

  vtkSmartPointer < vtkDataArray > normals ( nData->GetPointData()->GetNormals() );
  //vtkSmartPointer < vtkDataArray > normals ( nData->GetCellData()->GetNormals() );

  osg::ref_ptr< osg::Vec3Array > normalsV ( dynamic_cast < osg::Vec3Array * > ( nV ) );

  unsigned int numNormals ( normals->GetNumberOfTuples() );

  normalsV->resize( normals->GetNumberOfTuples() );

  unsigned int size ( normals->GetSize() );

  for ( unsigned int i = 0; i < numNormals; ++i )
  {
    osg::Vec3 &n ( normalsV->at( i ) );
    double *normal = normals->GetTuple( i );
    n[0] = normal[0];
    n[1] = normal[1];
    n[2] = normal[2];
  }

  //::memcpy( &normalsV->front(), normals->GetVoidPointer(0), normals->GetSize() );
#endif

  osg::ref_ptr< osg::Vec3Array > normalsV ( dynamic_cast < osg::Vec3Array * > ( nV ) );
  osg::ref_ptr< osg::Vec3Array > normalsT ( dynamic_cast < osg::Vec3Array * > ( nT ) );

  VTKTools::Convert::PolyData::polyDataToVertices( data.GetPointer(), *vertices, *elements, *normalsT, *normalsV );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create per vertex normals.
//
///////////////////////////////////////////////////////////////////////////////

void CalcPerVertexNormalsComponent::calculatePerVertexNormals ( OsgTools::Triangles::TriangleSet *triangleSet )
{
  vtkSmartPointer < vtkPolyData > data ( vtkPolyData::New() );
  VTKTools::Convert::PolyData::triangleSetToPolyData ( triangleSet, data );

  vtkSmartPointer < vtkPolyDataNormals > calcNormals ( vtkPolyDataNormals::New() );
  calcNormals->SetInput ( data.GetPointer() );

  calcNormals->Update();

  vtkSmartPointer < vtkPolyData > nData ( calcNormals->GetOutput() );
  calcNormals->ComputePointNormalsOn ();
  //calcNormals->SplittingOff ();
  //calcNormals->ConsistencyOff ();

  VTKTools::Convert::PolyData::polyDataToTriangleSet ( nData.GetPointer(), triangleSet );
}
