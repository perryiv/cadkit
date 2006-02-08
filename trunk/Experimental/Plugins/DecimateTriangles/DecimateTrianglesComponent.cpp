
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

#include "Usul/Scope/Timer.h"

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

  //vtkPolyData *data ( vtkPolyData::New() );

  VTKTools::Convert::PolyData::PolyDataVector polydata;

  {
    Usul::Scope::Timer timer ( "Time to convert to polydata" );
    
    VTKTools::Convert::PolyData::triangleSetToPolyData( triangleSet, polydata );
  }

  VTKTools::Convert::PolyData::PolyDataVector nPolydata;
  {
    Usul::Scope::Timer timer ( "Time to decimate" );
    
    for ( PolyDataVector::iterator iter = polydata.begin(); iter != polydata.end(); ++iter )
    {
      vtkDecimatePro *decimate	= vtkDecimatePro::New();
      decimate->SetInput ( *iter );
      decimate->SetTargetReduction( 0.50 );
      decimate->Update();
      //vtkPolyData *nData ( decimate->GetOutput() );
      nPolydata.push_back ( decimate->GetOutput() );
      decimate->Delete();

      // No longer need the poly data.  Delete to clear up space.
      //(*iter)->Delete();
    }

  }

  {
    Usul::Scope::Timer timer ( "Time to convert to triangle set" );

    VTKTools::Convert::PolyData::polyDataToTriangleSet ( nPolydata, triangleSet );
  }
  
  //data->Delete();
}
