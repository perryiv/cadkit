
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


#include "DecimateTrianglesComponent.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Resources/ProgressBar.h"

#include "OsgTools/Triangles/TriangleSet.h"

#include "Usul/Scope/Timer.h"

#include "VTKTools/Commands/Progress.h"

#include "osg/Array"

// Disable deprecated warning in Visual Studio 8
#if defined ( _MSC_VER ) && _MSC_VER == 1400
#pragma warning ( disable : 4996 )
#endif

#include "VTKTools/Convert/PolyData.h"

#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkDecimatePro.h"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( DecimateTrianglesComponent , DecimateTrianglesComponent::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////////////////////////

DecimateTrianglesComponent::DecimateTrianglesComponent() 
{
}

///////////////////////////////////////////////////////////////////////////////
//
// Destructor
//
///////////////////////////////////////////////////////////////////////////////

DecimateTrianglesComponent::~DecimateTrianglesComponent() 
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *DecimateTrianglesComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IDecimateTriangles::IID:
    return static_cast < Usul::Interfaces::IDecimateTriangles*>(this);
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin* > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Usul/Interfaces/IPlugin.h implementation
//
///////////////////////////////////////////////////////////////////////////////

std::string DecimateTrianglesComponent::getPluginName() const 
{
  return "Decimate Triangles";
}


///////////////////////////////////////////////////////////////////////////////
//
// Decimate triangles.
//
///////////////////////////////////////////////////////////////////////////////

void DecimateTrianglesComponent::decimateTriangles ( OsgTools::Triangles::TriangleSet *triangleSet )
{
  // Typedef.
  typedef VTKTools::Convert::PolyData::PolyDataVector PolyDataVector;

  vtkSmartPointer < vtkPolyData > data ( vtkPolyData::New() );

  {
    //Usul::Scope::Timer timer ( "Time to convert to polydata" );
    
    VTKTools::Convert::PolyData::triangleSetToPolyData( triangleSet, data );
  }

  vtkSmartPointer < vtkPolyData > nData ( 0x0 );
  {
    //Usul::Scope::Timer timer ( "Time to decimate" );
    
    vtkDecimatePro *decimate	= vtkDecimatePro::New();
    decimate->SetInput ( data );
    decimate->SetTargetReduction( 0.50 );

    vtkSmartPointer < VTKTools::Commands::Progress > progress ( new VTKTools::Commands::Progress );
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

    VTKTools::Convert::PolyData::polyDataToTriangleSet ( nData, triangleSet );
  }
}


void DecimateTrianglesComponent::decimateTriangles ( osg::Array *inVerts_, osg::DrawElementsUInt *indices,
                                                     osg::Array *nT, osg::Array *nV, float reduction )
{
  osg::ref_ptr < osg::Vec3Array > inVerts ( dynamic_cast < osg::Vec3Array * > ( inVerts_ ) );

  vtkSmartPointer < vtkPolyData > data ( vtkPolyData::New() );

  // Convert to polydata.
  VTKTools::Convert::PolyData::verticesToPolyData( inVerts.get(), indices, data.GetPointer() );

  vtkSmartPointer < vtkDecimatePro > decimate	= vtkDecimatePro::New();
  decimate->SetInput ( data );
  decimate->SetTargetReduction( reduction );

  vtkSmartPointer < VTKTools::Commands::Progress > progress ( new VTKTools::Commands::Progress );
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

  VTKTools::Convert::PolyData::polyDataToVertices ( nData.GetPointer(), *inVerts, *indices, *normalsT, *normalsV );
}
