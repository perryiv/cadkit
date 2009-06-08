
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


#include "DecimateTriangles.h"
#include "Progress.h"
#include "Convert.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Resources/ProgressBar.h"
#include "Usul/Scope/Timer.h"

#include "OsgTools/Triangles/TriangleSet.h"

#include "osg/Array"

#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkDecimatePro.h"


///////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////////////////////////

DecimateTriangles::DecimateTriangles() 
{
}

///////////////////////////////////////////////////////////////////////////////
//
// Destructor
//
///////////////////////////////////////////////////////////////////////////////

DecimateTriangles::~DecimateTriangles() 
{
}


///////////////////////////////////////////////////////////////////////////////
//
// Decimate triangles.
//
///////////////////////////////////////////////////////////////////////////////

void DecimateTriangles::operator() ( OsgTools::Triangles::TriangleSet *triangleSet, float reduction )
{
  vtkSmartPointer < vtkPolyData > data ( vtkPolyData::New() );

  {
    //Usul::Scope::Timer timer ( "Time to convert to polydata" );
    
    VTKTools::Convert::triangleSetToPolyData( triangleSet, data );
  }

  vtkSmartPointer < vtkPolyData > nData ( 0x0 );
  {
    //Usul::Scope::Timer timer ( "Time to decimate" );
    
    vtkDecimatePro *decimate	= vtkDecimatePro::New();
    decimate->SetInput ( data );
    decimate->SetTargetReduction( reduction );

    vtkSmartPointer < VTKTools::Progress > progress ( new VTKTools::Progress );
    progress->progressBar ( Usul::Resources::progressBar() );

    decimate->AddObserver( vtkCommand::StartEvent, progress.GetPointer() );
    decimate->AddObserver( vtkCommand::ProgressEvent, progress.GetPointer() );
    decimate->AddObserver( vtkCommand::EndEvent, progress.GetPointer() );

    decimate->Update();
    nData = decimate->GetOutput();

    decimate->RemoveObservers( vtkCommand::StartEvent );
    decimate->RemoveObservers( vtkCommand::ProgressEvent );
    decimate->RemoveObservers( vtkCommand::EndEvent );

    // Why do I need this extra un register??
    progress->UnRegister();

    decimate->Delete();
  }

  {
    //Usul::Scope::Timer timer ( "Time to convert to triangle set" );

    VTKTools::Convert::polyDataToTriangleSet ( nData, triangleSet );
  }
}


void DecimateTriangles::operator() ( osg::Array *inVerts_, osg::DrawElementsUInt *indices,
                                     osg::Array *nT, osg::Array *nV, float reduction )
{
  osg::ref_ptr < osg::Vec3Array > inVerts ( dynamic_cast < osg::Vec3Array * > ( inVerts_ ) );

  vtkSmartPointer < vtkPolyData > data ( vtkPolyData::New() );

  // Convert to polydata.
  VTKTools::Convert::verticesToPolyData( inVerts.get(), indices, data.GetPointer() );

  vtkSmartPointer < vtkDecimatePro > decimate	= vtkDecimatePro::New();
  decimate->SetInput ( data );
  decimate->SetTargetReduction( reduction );

  vtkSmartPointer < VTKTools::Progress > progress ( new VTKTools::Progress );
  progress->progressBar ( Usul::Resources::progressBar() );

  decimate->AddObserver( vtkCommand::StartEvent, progress.GetPointer() );
  decimate->AddObserver( vtkCommand::ProgressEvent, progress.GetPointer() );
  decimate->AddObserver( vtkCommand::EndEvent, progress.GetPointer() );

  decimate->Update();
  vtkSmartPointer < vtkPolyData > nData ( decimate->GetOutput() );

  decimate->RemoveObservers( vtkCommand::StartEvent );
  decimate->RemoveObservers( vtkCommand::ProgressEvent );
  decimate->RemoveObservers( vtkCommand::EndEvent );

  // Why do I need this extra un register??
  progress->UnRegister();

  osg::ref_ptr < osg::Vec3Array > normalsT ( dynamic_cast < osg::Vec3Array * > ( nT ) );
  osg::ref_ptr < osg::Vec3Array > normalsV ( dynamic_cast < osg::Vec3Array * > ( nV ) );

  VTKTools::Convert::polyDataToVertices ( nData.GetPointer(), *inVerts, *indices, *normalsT, *normalsV );
}
