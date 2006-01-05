
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

#include "SmoothTrianglesComponent.h"

#include "Usul/System/Clock.h"
#include "Usul/Errors/Assert.h"

#include "OsgTools/Triangles/TriangleSet.h"

#include "VTKTools/Convert/PolyData.h"

// Disable deprecated warning in Visual Studio 8
#if defined ( _MSC_VER ) && _MSC_VER == 1400
#pragma warning ( disable : 4996 )
#endif

#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkWindowedSincPolyDataFilter.h" // Smoothing algorithm

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( SmoothTrianglesComponent , SmoothTrianglesComponent::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////////////////////////

SmoothTrianglesComponent::SmoothTrianglesComponent() 
{
}

///////////////////////////////////////////////////////////////////////////////
//
// Destructor
//
///////////////////////////////////////////////////////////////////////////////

SmoothTrianglesComponent::~SmoothTrianglesComponent() 
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *SmoothTrianglesComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::ISmoothTriangles::IID:
    return static_cast < Usul::Interfaces::ISmoothTriangles*>(this);
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

std::string SmoothTrianglesComponent::getPluginName() const 
{
  return "Smooth Triangles";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Smooth the triangle set.
//
///////////////////////////////////////////////////////////////////////////////

void SmoothTrianglesComponent::smoothTriangles ( OsgTools::Triangles::TriangleSet *triangleSet )
{
  vtkPolyData *data ( vtkPolyData::New() );

  double start ( Usul::System::Clock::milliseconds() );
  VTKTools::Convert::PolyData::triangleSetToPolyData( triangleSet, data );
  ::printf ( "%8.4f seconds .... Time to convert to polydata.\n", static_cast < double > ( Usul::System::Clock::milliseconds() - start ) * 0.001 ); ::fflush ( stdout );

  start = Usul::System::Clock::milliseconds();
  vtkWindowedSincPolyDataFilter *smooth	= vtkWindowedSincPolyDataFilter::New();
  smooth->SetInput ( data );
  smooth->SetNumberOfIterations( 50 );
  smooth->Update();
  vtkPolyData *nData ( smooth->GetOutput() );
  ::printf ( "%8.4f seconds .... Time to smooth.\n", static_cast < double > ( Usul::System::Clock::milliseconds() - start ) * 0.001 ); ::fflush ( stdout );

  start = Usul::System::Clock::milliseconds();
  VTKTools::Convert::PolyData::polyDataToTriangleSet ( nData, triangleSet );
  ::printf ( "%8.4f seconds .... Time to convert to triangle set.\n", static_cast < double > ( Usul::System::Clock::milliseconds() - start ) * 0.001 ); ::fflush ( stdout );

  smooth->Delete();
  data->Delete();
}
