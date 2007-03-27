
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

// Disable deprecated warning in Visual Studio 8 
#if defined ( _MSC_VER ) && _MSC_VER == 1400
#pragma warning ( disable : 4996 )
#endif

#if defined _MSC_VER
#define NOMINMAX
#endif

#include "FoxTools/App/Application.h"
#include "FoxTools/Functions/GLVisual.h"

#include "Helios/Fox/Views/Canvas.h"

#include "OsgTools/Box.h"

#include "OsgTools/State/StateSet.h"

#include "osg/State"
#include "osg/StateSet"

#include "osgDB/WriteFile"

#include "fx.h"

#include "Threads/OpenThreads/Mutex.h"

#include "Usul/Threads/Mutex.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/CommandLine/Parser.h"
#include "Usul/Documents/Manager.h"
#include "Usul/App/Controller.h"
#include "Usul/Components/Manager.h"

#include "Usul/Interfaces/ITextMatrix.h"
#include "Usul/Interfaces/IScreenCapture.h"

#include <iomanip>

///////////////////////////////////////////////////////////////////////////////
//
//  Set for multi-threaded. This is global because the sooner we set this, 
//  the better. Setting in main() may be too late.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Threads::SetMutexFactory factory ( &Threads::OT::newOpenThreadsMutex );

int main ( int argc, char* argv[] )
{
  const unsigned int width ( 800 );
  const unsigned int height ( 800 );
  const std::string iveDir ( "F:/Task_7/Data/VF54_Set-1/SurfaceMesh_IVE/" );
  const std::string ivePrefix ( "TEST_VF54_Set-1_T" );
  const std::string imageDir ( "F:/Task_7/Data/VF54_Set-1/SurfaceMesh_IVE/images/" );
  const std::string name ( "VF54" );
  const unsigned int start ( 23 );
  const unsigned int finish ( 24 );

  Usul::CommandLine::Arguments::instance().set( argc, argv );

  // Declare the application.
  FoxTools::App::Application application ( "Movie test", "CadKit" );

  // Initialize the application.
  application.init ( argc, argv );

  // Declare the main window.
  FX::FXMainWindow *window ( new FX::FXMainWindow ( &application, application.getAppName(), 0x0, 0x0 ) );

#ifdef _DEBUG
  Usul::Components::Manager::instance().addPluginExtension( "plugd" );
#else
  Usul::Components::Manager::instance().addPluginExtension( "plug" );
#endif
  Usul::App::Controller::instance().loadPlugins();

  //ParadisDocument::ValidRefPtr doc ( new ParadisDocument );
  Helios::Views::Canvas *canvas = new Helios::Views::Canvas ( 0x0, window, FoxTools::Functions::visual() );

  window->resize( width, height );

  // Create the application.
  application.create();

  //window->show();

  Usul::Interfaces::ITrackball::ValidQueryPtr trackball ( canvas->viewer() );

  trackball->setCenter ( osg::Vec3 ( 116.139, 135.414, 103.869 ) );
  trackball->setDistance ( 539.523 );
  trackball->setRotation ( osg::Quat ( -0.18283, 0.444468, 0.802413, -0.353772 ) );


  //canvas->addChoreCameraFit();
  canvas->viewer()->backgroundColor ( osg::Vec4 ( 53.0, 52.0, 102.0, 1.0 ) );
  canvas->viewer()->resize( width, height );
  canvas->viewer()->axes( false );

  Usul::Interfaces::ITextMatrix::ValidQueryPtr text ( canvas->viewer() );

  text->createMatrix( 3, 5, 1, 1, 30, 320 );

  for ( unsigned int i = start; i < finish; ++i )
  {
    std::ostringstream hud;
    hud << name << " " << "Timestep: " << i;

    text->setText( 3, 5, 0, 0, hud.str() );

    std::ostringstream fn;
    fn << iveDir << ivePrefix << std::setw ( 3 ) << i << ".ive";
    std::string filename ( fn.str() );
    std::replace ( filename.begin(), filename.end(), ' ', '0' );

    // Useful typedefs
    typedef Usul::Documents::Document Document;

    // Try to create the document from one of our readers.
    Document::RefPtr document ( Usul::Documents::Manager::instance().create ( filename ) );

    // Set the new document.
    canvas->viewer()->document( document.get() );

    document->read ( filename );

    Usul::Interfaces::IBuildScene::ValidQueryPtr build ( document );
    osg::Group *group ( static_cast < osg::Group * > ( build->buildScene ( Usul::Documents::Document::Options() ) ) );
    
    canvas->viewer()->boundingBox( false );

    osg::BoundingBox bb;
    bb.expandBy ( osg::Vec3 ( 0.0, 0.0, 0.0 ) );
    bb.expandBy ( osg::Vec3 ( 255.0, 255.0, 255.0 ) );

    OsgTools::ColorBox box ( bb );
    box.color_policy().color ( osg::Vec4 ( 0, 0, 1, 1 ) );

    osg::ref_ptr<osg::MatrixTransform> mt ( new osg::MatrixTransform );
    mt->setMatrix ( osg::Matrix::translate ( bb.center() ) );
    mt->addChild ( box() );

    // Wire-frame.
    OsgTools::State::StateSet::setPolygonsLines ( mt.get(), true );
    OsgTools::State::StateSet::setLighting ( mt.get(), false );

    group->addChild( mt.get() );

    canvas->viewer()->scene( group );

    canvas->viewer()->numRenderPasses( 16 );

    canvas->viewer()->render();

    std::ostringstream out;
    out << imageDir << "image" << std::setw ( 3 ) << i << ".jpg";
    std::string outname ( out.str() );
    std::replace ( outname.begin(), outname.end(), ' ', '0' );

    Usul::Interfaces::IScreenCapture::ValidQueryPtr capture ( canvas->viewer() );

    osg::ref_ptr< osg::Image > image ( capture->screenCapture( height, width ) );

    osgDB::writeImageFile ( *image, outname );

    //canvas->viewer()->writeImageFile( outname );
    std::cout << "Done " << i << std::endl;

    document->clear();
  }
  
  return 0;
}

