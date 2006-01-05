
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

#include "OsgTools/Triangles/TriangleSet.h"

#include "Usul/System/Clock.h"

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
  vtkPolyData *data ( vtkPolyData::New() );

  double start ( Usul::System::Clock::milliseconds() );
  VTKTools::Convert::PolyData::triangleSetToPolyData( triangleSet, data );
  ::printf ( "%8.4f seconds .... Time to convert to polydata.\n", static_cast < double > ( Usul::System::Clock::milliseconds() - start ) * 0.001 ); ::fflush ( stdout );

  start = Usul::System::Clock::milliseconds();
  vtkDecimatePro *decimate	= vtkDecimatePro::New();
  decimate->SetInput ( data );
  decimate->SetTargetReduction( 0.50 );
  decimate->Update();
  vtkPolyData *nData ( decimate->GetOutput() );
  ::printf ( "%8.4f seconds .... Time to decimate.\n", static_cast < double > ( Usul::System::Clock::milliseconds() - start ) * 0.001 ); ::fflush ( stdout );

  start = Usul::System::Clock::milliseconds();
  VTKTools::Convert::PolyData::polyDataToTriangleSet ( nData, triangleSet );
  ::printf ( "%8.4f seconds .... Time to convert to triangle set.\n", static_cast < double > ( Usul::System::Clock::milliseconds() - start ) * 0.001 ); ::fflush ( stdout );

  decimate->Delete();
  data->Delete();
}
