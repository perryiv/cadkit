
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "DllGlue.h"
#include "Progress.h"

#ifdef __CLR_VER
#undef __CLR_VER
#endif

#include "Usul/Pointers/Pointers.h"
#include "Threads/OpenThreads/Mutex.h"
#include "Usul/Threads/Mutex.h"

#include "Usul/Interfaces/GUI/IProgressBar.h"
#include "Usul/Interfaces/IPlayMovie.h"
#include "Usul/Interfaces/IOssimPlanetLayer.h"
#include "Usul/Components/Manager.h"
#include "Usul/Documents/Manager.h"

#include "OsgTools/Render/Viewer.h"

#include "Minerva/Core/Layers/Layer.h"

#include <string>

#ifndef __CLR_VER
#define __CLR_VER
#endif

#include "Usul/Strings/Managed.h"

using namespace DT::Minerva::Plugins::Document::Glue;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DllGlue::DllGlue() : 
_document( 0x0 )
{
  Usul::Threads::SetMutexFactory factory ( &Threads::OT::newOpenThreadsMutex );

  _document = new ::Minerva::Document::MinervaDocument;
  Usul::Pointers::reference ( _document );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DllGlue::~DllGlue()
{
  _document->clear();
  Usul::Pointers::unreference ( _document );
  _document = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Finalizer.
//
///////////////////////////////////////////////////////////////////////////////

DllGlue::!DllGlue()
{
  try
  {
    _document->clear();
    Usul::Pointers::unreference ( _document );
    _document = 0x0;
  }
  catch ( System::Exception ^e )
  {
    System::Console::WriteLine( "Error 3775065703:" + e->Message );
  }
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
    CadKit::Interfaces::INativePtr ^layerPtr = safe_cast < CadKit::Interfaces::INativePtr ^ > ( layer );

    if( nullptr != layerPtr )
    {
      Usul::Interfaces::ILayer::QueryPtr base ( reinterpret_cast < Usul::Interfaces::ILayer * > ( layerPtr->nativeIntPtr().ToPointer() ) );
      _document->removeLayerCommand( base.get() );
    }
  }
  catch ( System::Exception ^e )
  {
    System::Console::WriteLine( "Error 2579788220:" + e->Message );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Hide the layer.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::hideLayer( CadKit::Interfaces::ILayer ^layer )
{
  try
  {
    CadKit::Interfaces::INativePtr ^layerPtr = safe_cast < CadKit::Interfaces::INativePtr ^ > ( layer );

    if( nullptr != layerPtr )
    {
      Usul::Interfaces::ILayer::QueryPtr base ( reinterpret_cast < Usul::Interfaces::ILayer * > ( layerPtr->nativeIntPtr().ToPointer() ) );
      _document->hideLayerCommand ( base.get() );
    }
  }
  catch ( System::Exception ^e )
  {
    System::Console::WriteLine( "Error 3092116830:" + e->Message );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show the layer.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::showLayer( CadKit::Interfaces::ILayer ^layer )
{
  try
  {
    CadKit::Interfaces::INativePtr ^layerPtr = safe_cast < CadKit::Interfaces::INativePtr ^ > ( layer );

    if( nullptr != layerPtr )
    {
      void *ptr ( layerPtr->nativeIntPtr().ToPointer() );
      Usul::Interfaces::IUnknown *unknown ( reinterpret_cast < Usul::Interfaces::IUnknown * > ( ptr ) );
      if( 0x0 != unknown && 0x0 != unknown->queryInterface( Usul::Interfaces::ILayer::IID ) )
        _document->showLayerCommand ( static_cast < Usul::Interfaces::ILayer* > ( unknown ) );
    }
  }
  catch ( System::Exception ^e )
  {
    System::Console::WriteLine( "Error 3046648080:" + e->Message );
  }
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
    CadKit::Interfaces::INativePtr ^layerPtr = safe_cast < CadKit::Interfaces::INativePtr ^ > ( layer );

    if( nullptr != layerPtr )
    {
      Usul::Interfaces::ILayer::QueryPtr base ( reinterpret_cast < Usul::Interfaces::ILayer * > ( layerPtr->nativeIntPtr().ToPointer() ) );
      Usul::Interfaces::IUnknown::QueryPtr queryPtr ( static_cast < Usul::Interfaces::IUnknown* > ( 0x0 ) );

      if( nullptr != progress )
      {
        // Managed/Unmanged progress interop class.
        Progress p ( progress );
        queryPtr = p.unmanagedProgress();
      }

      _document->modifyLayerCommand( base.get() );
    }
  }
  catch ( System::Exception ^e )
  {
    System::Console::WriteLine( "Error 2048069470:" + e->Message );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show the layer.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::addLayer( CadKit::Interfaces::ILayer ^layer, CadKit::Threads::Jobs::Progress ^progress )
{
  try
  {
    CadKit::Interfaces::INativePtr ^layerPtr = safe_cast < CadKit::Interfaces::INativePtr ^ > ( layer );

    if( nullptr != layerPtr )
    {
      Usul::Interfaces::ILayer::QueryPtr base ( reinterpret_cast < Usul::Interfaces::ILayer * > ( layerPtr->nativeIntPtr().ToPointer() ) );
      
      // Managed/Unmanged progress interop class.
      Progress p ( progress );

      _document->addLayerCommand ( base.get() );
    }
  }
  catch ( System::Exception ^e )
  {
    System::Console::WriteLine( "Error 674969904:" + e->Message );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the timestep type.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::timestepType ( CadKit::Interfaces::AnimateTimestep type )
{
  _document->timestepType( static_cast < ::Minerva::Interfaces::IAnimationControl::TimestepType > ( type ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the timestep type.
//
///////////////////////////////////////////////////////////////////////////////

CadKit::Interfaces::AnimateTimestep DllGlue::timestepType ()
{
  return static_cast < CadKit::Interfaces::AnimateTimestep > ( _document->timestepType() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the animation.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::startAnimation(float speed, bool accumulate, bool timeWindow, int numDays)
{
  try
  {
    _document->startAnimationCommand();
  }
  catch ( System::Exception ^e )
  {
    System::Console::WriteLine( "Error 1740100720:" + e->Message );
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
    _document->stopAnimationCommand( );
  }
  catch ( System::Exception ^e )
  {
    System::Console::WriteLine( "Error 1595256970:" + e->Message );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Stop the animation.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::pauseAnimation()
{
  try
  {
    _document->pauseAnimationCommand( );
  }
  catch ( System::Exception ^e )
  {
    System::Console::WriteLine( "Error 1595256970:" + e->Message );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the animation speed.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::animationSpeed ( double d )
{
  _document->animationSpeedCommand ( d );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the animation speed.
//
///////////////////////////////////////////////////////////////////////////////

double DllGlue::animationSpeed ()
{
  return 0.0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the viewer's scene.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::viewer( CadKit::Viewer::Glue::Viewer ^v )
{
  try
  {
    OsgTools::Render::Viewer* viewer = reinterpret_cast < OsgTools::Render::Viewer* > ( v->viewer().ToPointer() );

    if( viewer )
    {
      //_document->viewer( viewer->queryInterface( Usul::Interfaces::IUnknown::IID ) );
      viewer->document( _document );

      viewer->computeNearFar( false );

      viewer->scene( _document->buildScene ( Usul::Interfaces::IBuildScene::Options() ) );
    }
  }
  catch ( System::Exception ^e )
  {
    System::Console::WriteLine( "Error 1347444470:" + e->Message );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the show legend flag.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::showLegend( bool b )
{
  _document->showLegend( b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the show legend flag.
//
///////////////////////////////////////////////////////////////////////////////

bool DllGlue::showLegend()
{
  return _document->isShowLegend();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set percent screen width for legend.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::percentScreenWidth ( float percent )
{
  _document->percentScreenWidth ( percent );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set percent screen width for legend.
//
///////////////////////////////////////////////////////////////////////////////

float DllGlue::percentScreenWidth()
{
  return _document->percentScreenWidth();
}


///////////////////////////////////////////////////////////////////////////////
//
//  View the extents of given layer.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::viewLayerExtents( CadKit::Interfaces::ILayer ^layer )
{
  try
  {
    if( nullptr != layer )
    {
      CadKit::Interfaces::INativePtr ^ nativePtr = dynamic_cast < CadKit::Interfaces::INativePtr^ > ( layer );

      if( nullptr != nativePtr )
      {
        Usul::Interfaces::IUnknown::QueryPtr unknown ( reinterpret_cast < Usul::Interfaces::IUnknown* > ( nativePtr->nativeIntPtr().ToPointer() ) );
        _document->viewLayerExtents( unknown.get() );
      }
    }
  }
  catch ( System::Exception ^e )
  {
    System::Console::WriteLine( "Error 1986414875:" + e->Message );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the layer operation.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::setLayerOperation( System::String ^optype, int val, CadKit::Interfaces::ILayer ^layer )
{
  if( nullptr != layer ) 
  {   
    CadKit::Interfaces::INativePtr^ nativePtr = dynamic_cast< CadKit::Interfaces::INativePtr^ >( layer );   
    if( nullptr != nativePtr )
    {
      Usul::Interfaces::IUnknown::QueryPtr unknown ( reinterpret_cast < Usul::Interfaces::IUnknown* > ( nativePtr->nativeIntPtr().ToPointer() ) );
      _document->setLayerOperation( Usul::Strings::convert( optype ), val, unknown.get() );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the layer to the list of favorites.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::addToFavorites( CadKit::Interfaces::ILayer^ layer )
{
  if( nullptr != layer )
  {
    CadKit::Interfaces::INativePtr ^layerPtr = safe_cast < CadKit::Interfaces::INativePtr ^ > ( layer );

    if( nullptr != layerPtr )
    {
      Usul::Interfaces::IUnknown::QueryPtr unknwon ( reinterpret_cast < Usul::Interfaces::IUnknown* > ( layerPtr->nativeIntPtr().ToPointer() ) );
      if( unknwon.valid() )
        _document->addToFavorites( unknwon.get() );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a favorite based on the name.
//
///////////////////////////////////////////////////////////////////////////////

CadKit::Interfaces::ILayer^ DllGlue::createFavorite( System::String^ name )
{
  Usul::Interfaces::ILayer::QueryPtr layer ( _document->createFavorite ( Usul::Strings::convert( name ) ) );
  return DT::Minerva::Glue::Factory::createIUnknown( System::IntPtr ( layer.get() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get a list of favorites.
//
///////////////////////////////////////////////////////////////////////////////

array<System::String^ > ^ DllGlue::Favorites::get()
{
  ::Minerva::Document::MinervaDocument::Names names ( _document->favorites() );

  array<System::String^>^ managedNames = gcnew array<System::String^> ( names.size() );

  for( unsigned int i = 0; i < names.size(); ++i )
    managedNames->SetValue( gcnew System::String( names[i].c_str() ), static_cast < int > ( i ) );

  return managedNames;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Open the filename.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::open( System::String^ filename )
{
  _document->open( Usul::Strings::convert( filename ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Save document.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::save()
{
  _document->save();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Save document as given filename.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::saveAs( System::String^ filename )
{
  _document->saveAs ( Usul::Strings::convert( filename ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Export document as given filename.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::exportFile( System::String^ filename )
{
  _document->write ( Usul::Strings::convert( filename ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the movie mode true / false. 
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::setMovieMode( bool b,  CadKit::Viewer::Glue::Viewer^ v )
{

  OsgTools::Render::Viewer* viewer = reinterpret_cast < OsgTools::Render::Viewer* > ( v->viewer().ToPointer() );

  if( viewer )
  {
    // PlayMovie plugin implements both IPlayMovie and ITool. 
    Usul::Interfaces::ITool::QueryPtr component = Usul::Components::Manager::instance().getInterface( Usul::Interfaces::IPlayMovie::IID );
    if( component && b )
    {
      viewer->setTool( component );
    }
    else
    {
      viewer->doneTool();
    }
  }  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Play current movie. 
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::play( CadKit::Viewer::Glue::Viewer^ v )
{
  OsgTools::Render::Viewer* viewer = reinterpret_cast < OsgTools::Render::Viewer* > ( v->viewer().ToPointer() );

  if( viewer )
  {
    // PlayMovie plugin implements both IPlayMovie and ITool. 
    Usul::Interfaces::IPlayMovie::QueryPtr component = Usul::Components::Manager::instance().getInterface( Usul::Interfaces::IPlayMovie::IID );
    if( component )
    {
      component->play();
    }
  }  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pause current movie. 
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::pause( CadKit::Viewer::Glue::Viewer^ v )
{
  OsgTools::Render::Viewer* viewer = reinterpret_cast < OsgTools::Render::Viewer* > ( v->viewer().ToPointer() );

  if( viewer )
  {
    // PlayMovie plugin implements both IPlayMovie and ITool. 
    Usul::Interfaces::IPlayMovie::QueryPtr component = Usul::Components::Manager::instance().getInterface( Usul::Interfaces::IPlayMovie::IID );
    if( component )
    {
      component->pause();
    }
  }  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Restart current movie. 
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::restart( CadKit::Viewer::Glue::Viewer^ v )
{
  OsgTools::Render::Viewer* viewer = reinterpret_cast < OsgTools::Render::Viewer* > ( v->viewer().ToPointer() );

  if( viewer )
  {
    // PlayMovie plugin implements both IPlayMovie and ITool. 
    Usul::Interfaces::IPlayMovie::QueryPtr component = Usul::Components::Manager::instance().getInterface( Usul::Interfaces::IPlayMovie::IID );
    if( component )
    {
      component->restart();
    }
  }  
}


///////////////////////////////////////////////////////////////////////////////
//
// Movie currently playing? 
//
///////////////////////////////////////////////////////////////////////////////

bool DllGlue::isPlaying()
{
  Usul::Interfaces::IPlayMovie::QueryPtr component = Usul::Components::Manager::instance().getInterface( Usul::Interfaces::IPlayMovie::IID );
  if( component )
  {
    return component->isPlaying();
  }

  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
// Movie currently paused? 
//
///////////////////////////////////////////////////////////////////////////////

bool DllGlue::isPaused()
{
  Usul::Interfaces::IPlayMovie::QueryPtr component = Usul::Components::Manager::instance().getInterface( Usul::Interfaces::IPlayMovie::IID );
  if( component )
  {
    return component->isPaused();
  }

  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the layers.
//
///////////////////////////////////////////////////////////////////////////////

array<CadKit::Interfaces::ILayer^ > ^ DllGlue::Layers::get()
{
  typedef ::Minerva::Document::MinervaDocument::Layers  NativeLayers;
  NativeLayers& layers ( _document->layers() );

  const unsigned int size ( layers.size() );

  array<CadKit::Interfaces::ILayer^ > ^ managedLayers = gcnew array<CadKit::Interfaces::ILayer^ > ( size );

  for ( unsigned int i = 0; i < size; ++i  )
  {
    Usul::Interfaces::ILayer::QueryPtr layer ( layers.at( i ) );
    Usul::Interfaces::IVectorLayer::QueryPtr vector ( layer );
    if( vector.valid() )
    {
      managedLayers->SetValue( DT::Minerva::Glue::Factory::createIUnknown( System::IntPtr ( layer.get() ) ), static_cast < int > ( i ) );
    }
    else
    {
      Usul::Interfaces::IOssimPlanetLayer::QueryPtr ossim ( layer );
      if( ossim.valid() )
        managedLayers->SetValue( CadKit::OSSIMPlanet::Glue::Factory::create( System::IntPtr ( ossim.get() ) ), static_cast < int > ( i ) );
    }
  }

  return managedLayers;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the document as active?
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::setActive ( bool b )
{
  Usul::Documents::Manager::instance().activeDocument ( b ? _document : 0x0 );
}
