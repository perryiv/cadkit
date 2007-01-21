
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater at Arizona State University
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "DllGlue.h"
#include "Progress.h"

#include "Usul/Pointers/Pointers.h"
#include "Threads/OpenThreads/Mutex.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Strings/ManagedToNative.h"

#include "Minerva/Layers/Layer.h"
#include "Minerva/postGIS/Geometry.h"

#include "osgDB/ReadFile"

#include "ossimPlanet/ossimPlanetDatabasePager.h"
#include "ossimPlanet/ossimPlanetManipulator.h"

#include <string>

#include "Usul/Interfaces/GUI/IProgressBar.h"

using namespace DT::Minerva::Plugins::Document::Glue;

namespace Detail
{
  std::string toString( System::String^ source )
  {
    System::IntPtr ptr = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( source );
    char* s = (char*)(void*) ptr;
    
    std::string string ( s );
    
    System::Runtime::InteropServices::Marshal::FreeHGlobal( ptr );

    return string;
  }

  System::String^ toMString( const std::string& source )
  {
    return gcnew System::String( source.c_str() );
  }


  System::String^ intToString ( int i )
  {
    std::ostringstream os;
    os << i;
    return toMString ( os.str() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DllGlue::DllGlue() : 
_viewer( 0x0 ),
_sceneManager( 0x0 )
{
  Usul::Threads::SetMutexFactory factory ( &Threads::OT::newOpenThreadsMutex );
  _sceneManager = new ::Minerva::Scene::SceneManager;
  Usul::Pointers::reference ( _sceneManager );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DllGlue::~DllGlue()
{
  Usul::Pointers::unreference ( _sceneManager );
  _sceneManager = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Finalizer.
//
///////////////////////////////////////////////////////////////////////////////

DllGlue::!DllGlue()
{
  Usul::Pointers::unreference ( _sceneManager );
  _sceneManager = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the layer.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::removeLayer( CadKit::Interfaces::ILayer ^layer )
{
  try
  {
    DT::Minerva::Interfaces::ILayer ^layerPtr = dynamic_cast < DT::Minerva::Interfaces::ILayer ^ > ( layer );

    if( nullptr != layerPtr )
    {
      ::Minerva::Layers::Layer::RefPtr base ( reinterpret_cast < ::Minerva::Layers::Layer * > ( layerPtr->layerPtr().ToPointer() ) );
      _sceneManager->removeLayer( base->layerID() );
      _sceneManager->dirty ( true );
      _sceneManager->buildScene();
    }
  }
  catch ( System::Exception ^e )
  {
    System::Console::WriteLine( "Error 2579788220:" + e->Message );
  }
  catch ( const std::exception &e )
  {
    System::Console::WriteLine( "Error 2608069470:" + gcnew System::String ( e.what() ) );
  }
  catch ( ... )
  {
    System::Console::WriteLine( "Error 2637756970: Unknown exception caught." );
  }
}


void DllGlue::dirtyScene()
{
  _sceneManager->dirty( true );
  _sceneManager->buildScene();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Modify the layer.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::modifyLayer( CadKit::Interfaces::ILayer ^layer, CadKit::Threads::Jobs::Progress ^progress )
{
  try
  {
    DT::Minerva::Interfaces::ILayer ^layerPtr = dynamic_cast < DT::Minerva::Interfaces::ILayer ^ > ( layer );

    if( nullptr != layerPtr )
    {
      ::Minerva::Layers::Layer::RefPtr base ( reinterpret_cast < ::Minerva::Layers::Layer * > ( layerPtr->layerPtr().ToPointer() ) );
      Progress p ( progress );
      base->modify( p.unmanagedProgress() );
      _sceneManager->dirty( true );
      _sceneManager->buildScene();
    }
  }
  catch ( System::Exception ^e )
  {
    System::Console::WriteLine( "Error 2048069470:" + e->Message );
  }
  catch ( const std::exception &e )
  {
    System::Console::WriteLine( "Error 2083538220:" + gcnew System::String ( e.what() ) );
  }
  catch ( ... )
  {
    System::Console::WriteLine( "Error 2122288220: Unknown exception caught." );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the animation.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::startAnimation(float speed, bool accumulate, bool dateTimeStep, bool timeWindow, int numDays)
{
  try
  {
    _sceneManager->animate(true, accumulate, dateTimeStep, speed, timeWindow, numDays);
    _sceneManager->buildScene();
  }
  catch ( System::Exception ^e )
  {
    System::Console::WriteLine( "Error 1740100720:" + e->Message );
  }
  catch ( const std::exception &e )
  {
    System::Console::WriteLine( "Error 1762600720:" + gcnew System::String ( e.what() ) );
  }
  catch ( ... )
  {
    System::Console::WriteLine( "Error 1818850720: Unknown exception caught." );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Stop the animation.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::stopAnimation()
{
  try
  {
    _sceneManager->animate(false, false, false, 0.0, false, 0);
    _sceneManager->buildScene();
  }
  catch ( System::Exception ^e )
  {
    System::Console::WriteLine( "Error 1595256970:" + e->Message );
  }
  catch ( const std::exception &e )
  {
    System::Console::WriteLine( "Error 1620413220:" + gcnew System::String ( e.what() ) );
  }
  catch ( ... )
  {
    System::Console::WriteLine( "Error 1646194470: Unknown exception caught." );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the viewer's scene.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::viewer( CadKit::Viewer::Glue::Viewer ^viewer )
{
  try
  {
    _viewer = reinterpret_cast < OsgTools::Render::Viewer* > ( viewer->viewer().ToPointer() );

    if( _viewer )
    {
      _viewer->navManip( new ossimPlanetManipulator );
      _viewer->databasePager( new ossimPlanetDatabasePager );

      _viewer->computeNearFar( false );

      _viewer->scene( _sceneManager->root() );
      _viewer->sceneUpdate( _sceneManager );
    }
  }
  catch ( System::Exception ^e )
  {
    System::Console::WriteLine( "Error 1347444470:" + e->Message );
  }
  catch ( const std::exception &e )
  {
    System::Console::WriteLine( "Error 1372131970:" + gcnew System::String ( e.what() ) );
  }
  catch ( ... )
  {
    System::Console::WriteLine( "Error 1396038220: Unknown exception caught." );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Resize the scene manager.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::resize( int w, int h )
{
  try
  {
    _sceneManager->resize( 0, 0, w, h );
  }
  catch ( System::Exception ^e )
  {
    System::Console::WriteLine( "Error 3665068016:" + e->Message );
  }
  catch ( const std::exception &e )
  {
    System::Console::WriteLine( "Error 4149780950:" + gcnew System::String ( e.what() ) );
  }
  catch ( ... )
  {
    System::Console::WriteLine( "Error 4175093450: Unknown exception caught." );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show the layer.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::showLayer( CadKit::Interfaces::ILayer ^layer, CadKit::Threads::Jobs::Progress ^progress )
{
  try
  {
    DT::Minerva::Interfaces::ILayer ^layerPtr = dynamic_cast < DT::Minerva::Interfaces::ILayer ^ > ( layer );

    if( nullptr != layerPtr )
    {
      ::Minerva::Layers::Layer::RefPtr base ( reinterpret_cast < ::Minerva::Layers::Layer * > ( layerPtr->layerPtr().ToPointer() ) );

      // Managed/Unmanged progress interop class.
      Progress p ( progress );
      base->buildDataObjects( p.unmanagedProgress() );
      _sceneManager->addLayer( base.get() );
    }
  }
  catch ( System::Exception ^e )
  {
    System::Console::WriteLine( "Error 674969904:" + e->Message );
  }
  catch ( const std::exception &e )
  {
    System::Console::WriteLine( "Error 1029090542:" + gcnew System::String ( e.what() ) );
  }
  catch ( ... )
  {
    System::Console::WriteLine( "Error 1050340542: Unknown exception caught." );
  }
}


void DllGlue::addKeyWordList( System::String^ kwl )
{
  try
  {
    _sceneManager->addKeyWordList( Usul::Strings::convert ( kwl ).c_str() );
  }
  catch ( System::Exception ^e )
  {
    System::Console::WriteLine( "Error 1935115428:" + e->Message );
  }
  catch ( const std::exception &e )
  {
    System::Console::WriteLine( "Error 1962615428:" + gcnew System::String ( e.what() ) );
  }
  catch ( ... )
  {
    System::Console::WriteLine( "Error 1994334178: Unknown exception caught." );
  }
}


void DllGlue::addLayer( CadKit::OSSIMPlanet::Glue::ImageLayer ^ layer )
{
  if( nullptr != layer )
  {
    // Add layer to group.
    osg::ref_ptr< ossimPlanetTextureLayer > texture ( reinterpret_cast < ossimPlanetTextureLayer* > ( layer->intPtr().ToPointer() ) );

    if( texture.valid() )
      _sceneManager->addImage( texture.get() );
  }
}

void DllGlue::removeLayer( CadKit::OSSIMPlanet::Glue::ImageLayer ^ layer )
{
  if( nullptr != layer )
  {
    osg::ref_ptr< ossimPlanetTextureLayer > texture ( reinterpret_cast < ossimPlanetTextureLayer* > ( layer->intPtr().ToPointer() ) );

    if( texture.valid() )
      _sceneManager->removeImage( texture.get() );
  }
}

