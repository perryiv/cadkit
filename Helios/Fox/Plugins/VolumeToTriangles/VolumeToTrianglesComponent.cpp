
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Component to convert a volume to triangles.
//
///////////////////////////////////////////////////////////////////////////////

#include "VolumeToTrianglesComponent.h"

#include "Usul/App/Controller.h"
#include "Usul/Documents/Document.h"
#include "Usul/Components/Manager.h"

#include "Usul/Interfaces/IImageList.h"
#include "Usul/Interfaces/ICreateTriangleDocument.h"

#include "OsgTools/Images/Image3d.h"
#include "OsgTools/Triangles/TriangleSet.h"

#include "VTKTools/Convert/ImageData.h"
#include "VTKTools/Convert/PolyData.h"

#include "osg/Image"

#include "vtkImageData.h"
#include "vtkPolyData.h"
#include "vtkDiscreteMarchingCubes.h"
#include "vtkTriangleFilter.h"
#include "vtkSmartPointer.h"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( VolumeToTrianglesComponent , VolumeToTrianglesComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////////////////////////

VolumeToTrianglesComponent::VolumeToTrianglesComponent() 
{
}


///////////////////////////////////////////////////////////////////////////////
//
// Destructor
//
///////////////////////////////////////////////////////////////////////////////


VolumeToTrianglesComponent::~VolumeToTrianglesComponent() 
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *VolumeToTrianglesComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin* > ( this );
  case Usul::Interfaces::ICommand::IID:
    return static_cast < Usul::Interfaces::ICommand* > ( this );
  case Usul::Interfaces::IMenuEntry::IID:
    return static_cast < Usul::Interfaces::IMenuEntry* > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Usul/Interfaces/IPlugin.h implementation
//
///////////////////////////////////////////////////////////////////////////////

std::string VolumeToTrianglesComponent::getPluginName() const 
{
  return "VolumeToTriangles";
}

	
///////////////////////////////////////////////////////////////////////////////
//
//  Smooth the model.
//
///////////////////////////////////////////////////////////////////////////////

void VolumeToTrianglesComponent::execute ( Usul::Interfaces::IUnknown *caller )
{
  Usul::Interfaces::IDocument::QueryPtr document ( Usul::App::Controller::instance().activeDocument() );
  Usul::Interfaces::IImageList::QueryPtr imageList ( document );

  if ( imageList.valid() )
  {
    //osg::ref_ptr< osg::Image > image ( OsgTools::Images::image3d( imageList->getImageList() ) );

    vtkSmartPointer < vtkImageData > vtkImage ( vtkImageData::New() );

    //VTKTools::Convert::ImageData::osgImageToImageData( *image, *vtkImage );
    VTKTools::Convert::ImageData::osgImageToImageData( imageList->getImageList(), *vtkImage );

    vtkSmartPointer < vtkDiscreteMarchingCubes > surface ( vtkDiscreteMarchingCubes::New() );
    surface->SetInput( vtkImage.GetPointer() );
    surface->SetValue ( 0, 255.0 );
	  surface->ComputeScalarsOff();
	  surface->ComputeNormalsOn();
	  surface->ComputeGradientsOff();

    vtkSmartPointer < vtkTriangleFilter > triangles ( vtkTriangleFilter::New() );
    triangles->SetInput ( surface->GetOutput() );

    OsgTools::Triangles::TriangleSet::ValidRefPtr triangleSet ( new OsgTools::Triangles::TriangleSet );

    VTKTools::Convert::PolyData::polyDataToTriangleSet ( triangles->GetOutput(), triangleSet.get() );

    typedef Usul::Components::Manager PluginManager;
    typedef Usul::Interfaces::ICreateTriangleDocument Creator;

    Creator::QueryPtr plugin ( PluginManager::instance().getInterface( Creator::IID ) );
    
    if ( plugin.valid() )
    {
      plugin->createTriangleDocument( triangleSet.get(), caller );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the menu text.
//
///////////////////////////////////////////////////////////////////////////////

std::string VolumeToTrianglesComponent::menuText() const
{
  return "Volume To Triangles";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the hot-key text.
//
///////////////////////////////////////////////////////////////////////////////

std::string VolumeToTrianglesComponent::hotKeyText() const
{
  return "";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the status-bar text.
//
///////////////////////////////////////////////////////////////////////////////

std::string VolumeToTrianglesComponent::statusBarText() const
{
  return "Create a triangle document from the volume.";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get Group for the menu.  "" means no group
//
///////////////////////////////////////////////////////////////////////////////

std::string VolumeToTrianglesComponent::menuGroup() const
{
  return "Volumes";
}
