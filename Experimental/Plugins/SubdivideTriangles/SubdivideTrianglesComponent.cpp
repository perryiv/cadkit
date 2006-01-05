
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


#include "SubdivideTrianglesComponent.h"

#include "Usul/System/Clock.h"
#include "OsgTools/Triangles/TriangleSet.h"

#include "VTKTools/Convert/PolyData.h"

// Disable deprecated warning in Visual Studio 8
#if defined ( _MSC_VER ) && _MSC_VER == 1400
#pragma warning ( disable : 4996 )
#endif

#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkLoopSubdivisionFilter.h"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( SubdivideTrianglesComponent , SubdivideTrianglesComponent::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////////////////////////

SubdivideTrianglesComponent::SubdivideTrianglesComponent() 
{
}

///////////////////////////////////////////////////////////////////////////////
//
// Destructor
//
///////////////////////////////////////////////////////////////////////////////

SubdivideTrianglesComponent::~SubdivideTrianglesComponent() 
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *SubdivideTrianglesComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin* > ( this );
  case Usul::Interfaces::ISubdivideTriangles::IID:
    return static_cast < Usul::Interfaces::ISubdivideTriangles* > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Usul/Interfaces/IPlugin.h implementation
//
///////////////////////////////////////////////////////////////////////////////

std::string SubdivideTrianglesComponent::getPluginName() const 
{
  return "Subdivide Triangles";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Subdivide the triangles.
//
///////////////////////////////////////////////////////////////////////////////

void SubdivideTrianglesComponent::subdivideTriangles ( OsgTools::Triangles::TriangleSet *triangleSet )
{
  vtkPolyData *data ( vtkPolyData::New() );

  double start ( Usul::System::Clock::milliseconds() );
  VTKTools::Convert::PolyData::triangleSetToPolyData( triangleSet, data );
  ::printf ( "%8.4f seconds .... Time to convert to polydata.\n", static_cast < double > ( Usul::System::Clock::milliseconds() - start ) * 0.001 ); ::fflush ( stdout );

  start = Usul::System::Clock::milliseconds();
  vtkLoopSubdivisionFilter *subdivide	= vtkLoopSubdivisionFilter::New();
  subdivide->SetInput ( data );
  subdivide->SetNumberOfSubdivisions( 2 );
  subdivide->Update();
  vtkPolyData *nData ( subdivide->GetOutput() );
  ::printf ( "%8.4f seconds .... Time to subdivide.\n", static_cast < double > ( Usul::System::Clock::milliseconds() - start ) * 0.001 ); ::fflush ( stdout );

  start = Usul::System::Clock::milliseconds();
  VTKTools::Convert::PolyData::polyDataToTriangleSet ( nData, triangleSet );
  ::printf ( "%8.4f seconds .... Time to convert to triangle set.\n", static_cast < double > ( Usul::System::Clock::milliseconds() - start ) * 0.001 ); ::fflush ( stdout );

  subdivide->Delete();
  data->Delete();
}
