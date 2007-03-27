
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

#include "Usul/Pointers/Pointers.h"
#include "Threads/OpenThreads/Mutex.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Strings/ManagedToNative.h"

#include "OsgTools/Render/Viewer.h"

#include "Minerva/Core/Layers/Layer.h"

#include <string>

#include "Usul/Interfaces/GUI/IProgressBar.h"
#include "Usul/Interfaces/IPlayMovie.h"
#include "Usul/Components/Manager.h"

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
  Usul::Pointers::unreference ( _document );
  _document = 0x0;
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
      ::Minerva::Core::Layers::Layer::RefPtr base ( reinterpret_cast < ::Minerva::Core::Layers::Layer * > ( layerPtr->layerPtr().ToPointer() ) );
      _document->removeLayer( base.get() );
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
    DT::Minerva::Interfaces::ILayer ^layerPtr = dynamic_cast < DT::Minerva::Interfaces::ILayer ^ > ( layer );

    if( nullptr != layerPtr )
    {
      ::Minerva::Core::Layers::Layer::RefPtr base ( reinterpret_cast < ::Minerva::Core::Layers::Layer * > ( layerPtr->layerPtr().ToPointer() ) );
      _document->hideLayer( base.get() );
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
    DT::Minerva::Interfaces::ILayer ^layerPtr = dynamic_cast < DT::Minerva::Interfaces::ILayer ^ > ( layer );

    if( nullptr != layerPtr )
    {
      ::Minerva::Core::Layers::Layer::RefPtr base ( reinterpret_cast < ::Minerva::Core::Layers::Layer * > ( layerPtr->layerPtr().ToPointer() ) );
      _document->showLayer( base.get() );
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
    DT::Minerva::Interfaces::ILayer ^layerPtr = dynamic_cast < DT::Minerva::Interfaces::ILayer ^ > ( layer );

    if( nullptr != layerPtr )
    {
      ::Minerva::Core::Layers::Layer::RefPtr base ( reinterpret_cast < ::Minerva::Core::Layers::Layer * > ( layerPtr->layerPtr().ToPointer() ) );
      Progress p ( progress );
      _document->modifyLayer( base, p.unmanagedProgress() );
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


void DllGlue::timestepType ( CadKit::Interfaces::AnimateTimestep type )
{
 _document->timestepType( static_cast < OsgTools::Animate::Settings::TimestepType > ( type ) );
}


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
    _document->startAnimation( speed, accumulate, timeWindow, numDays );
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
    _document->stopAnimation( );
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

void DllGlue::viewer( CadKit::Viewer::Glue::Viewer ^v )
{
  try
  {
    OsgTools::Render::Viewer* viewer = reinterpret_cast < OsgTools::Render::Viewer* > ( v->viewer().ToPointer() );

    if( viewer )
    {
      _document->viewer( viewer->queryInterface( Usul::Interfaces::IUnknown::IID ) );
      viewer->document( _document );

      viewer->computeNearFar( false );

      viewer->scene( _document->buildScene ( Usul::Interfaces::IBuildScene::Options() ) );
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
    _document->resize( w, h );
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

void DllGlue::addLayer( CadKit::Interfaces::ILayer ^layer, CadKit::Threads::Jobs::Progress ^progress )
{
  try
  {
    DT::Minerva::Interfaces::ILayer ^layerPtr = dynamic_cast < DT::Minerva::Interfaces::ILayer ^ > ( layer );

    if( nullptr != layerPtr )
    {
      ::Minerva::Core::Layers::Layer::RefPtr base ( reinterpret_cast < ::Minerva::Core::Layers::Layer * > ( layerPtr->layerPtr().ToPointer() ) );

      // Managed/Unmanged progress interop class.
      Progress p ( progress );

      _document->addLayer( base.get(), p.unmanagedProgress() );
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


///////////////////////////////////////////////////////////////////////////////
//
//  Add an image.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::addLayer( CadKit::Interfaces::IOssimLayer ^ layer )
{
  if( nullptr != layer )
  {
    CadKit::Interfaces::INativePtr ^ nativePtr = dynamic_cast < CadKit::Interfaces::INativePtr^ > ( layer );

    if( nullptr != nativePtr )
    {
      // Add layer to group.
      osg::ref_ptr< ossimPlanetTextureLayer > texture ( reinterpret_cast < ossimPlanetTextureLayer* > ( nativePtr->nativeIntPtr().ToPointer() ) );

      if( texture.valid() )
        _document->addLayer( texture.get() );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove an image.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::removeLayer( CadKit::Interfaces::IOssimLayer ^ layer )
{
  if( nullptr != layer )
  {
    CadKit::Interfaces::INativePtr ^ nativePtr = dynamic_cast < CadKit::Interfaces::INativePtr^ > ( layer );

    if( nullptr != nativePtr )
    {
      osg::ref_ptr< ossimPlanetTextureLayer > texture ( reinterpret_cast < ossimPlanetTextureLayer* > ( nativePtr->nativeIntPtr().ToPointer() ) );

      if( texture.valid() )
        _document->removeLayer( texture.get() );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is elevation enabled.
//
///////////////////////////////////////////////////////////////////////////////

bool DllGlue::elevationEnabled()
{
  return _document->elevationEnabled();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the elevation enabled flag.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::elevationEnabled( bool val )
{
  _document->elevationEnabled( val );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the HUD enabled.
//
///////////////////////////////////////////////////////////////////////////////

bool DllGlue::hudEnabled()
{
  return _document->hudEnabled();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the hud enabled flag.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::hudEnabled( bool val )
{
  _document->hudEnabled( val );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is ephemeris enabled?
//
///////////////////////////////////////////////////////////////////////////////

bool DllGlue::ephemerisFlag()
{
  return _document->ephemerisFlag();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the ephemeris flag.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::ephemerisFlag( bool val )
{
  _document->ephemerisFlag( val );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the elevation exageration flag.
//
///////////////////////////////////////////////////////////////////////////////

float DllGlue::elevationExag()
{
  return _document->elevationExag();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the elevation exageration flag.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::elevationExag( float exag )
{
  _document->elevationExag( exag );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the elevation patch size.
//
///////////////////////////////////////////////////////////////////////////////

int DllGlue::elevationPatchSize()
{
  return _document->elevationPatchSize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the elevation patch size.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::elevationPatchSize( float patchSize )
{
  _document->elevationPatchSize( patchSize );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the max level of detail.
//
///////////////////////////////////////////////////////////////////////////////

int DllGlue::levelDetail()
{
  return _document->levelDetail();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the max level of detail.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::levelDetail( float levelDetail )
{
  _document->levelDetail( levelDetail );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the elevation cache directory.
//
///////////////////////////////////////////////////////////////////////////////

System::String^ DllGlue::elevationCacheDir()
{
  return gcnew System::String( _document->elevationCacheDir().c_str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the elevation cache directory.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::elevationCacheDir( System::String^ directory )
{
  _document->elevationCacheDir( Usul::Strings::convert( directory ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the lat long grid flag.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::latLongGrid ( bool b )
{
  _document->latLongGrid ( b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the lat long grid flag.
//
///////////////////////////////////////////////////////////////////////////////

bool DllGlue::latLongGrid()
{
  return _document->latLongGrid();
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
  return _document->showLegend();
}


///////////////////////////////////////////////////////////////////////////////
//
//  View the extents of given layer.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::viewLayerExtents( CadKit::Interfaces::ILayer ^layer )
{
  if( nullptr != layer )
  {
    CadKit::Interfaces::INativePtr ^ nativePtr = dynamic_cast < CadKit::Interfaces::INativePtr^ > ( layer );

    if( nullptr != nativePtr )
    {
      osg::ref_ptr< ossimPlanetTextureLayer > texture ( reinterpret_cast < ossimPlanetTextureLayer* > ( nativePtr->nativeIntPtr().ToPointer() ) );

      if( texture.valid() )
      {
        _document->viewLayerExtents( texture.get() );
      }
    }
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
      osg::ref_ptr< ossimPlanetTextureLayer > texture ( reinterpret_cast < ossimPlanetTextureLayer* > ( nativePtr->nativeIntPtr().ToPointer() ) );

      if( texture.valid() )
      { 
        _document->setLayerOperation( Usul::Strings::convert( optype ), val, texture.get() );
      }
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
    DT::Minerva::Interfaces::ILayer ^layerPtr = dynamic_cast < DT::Minerva::Interfaces::ILayer ^ > ( layer );

    if( nullptr != layerPtr )
    {
      ::Minerva::Core::Layers::Layer::RefPtr base ( reinterpret_cast < ::Minerva::Core::Layers::Layer * > ( layerPtr->layerPtr().ToPointer() ) );
      _document->addToFavorites( base.get() );
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
  ::Minerva::Core::Layers::Layer::RefPtr layer ( _document->createFavorite ( Usul::Strings::convert( name ) ) );

  return DT::Minerva::Glue::Factory::create( System::IntPtr ( layer.get() ) );
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

void DllGlue::setMovieMode( bool b,  CadKit::Viewer::Glue::Viewer^ v )
{
  OsgTools::Render::Viewer* viewer = reinterpret_cast < OsgTools::Render::Viewer* > ( v->viewer().ToPointer() );

  if( viewer )
  {
    // PlayMovie plugin implements both IPlayMovie and ITool. 
    Usul::Interfaces::ITool::QueryPtr component = Usul::Components::Manager::instance().getInterface( Usul::Interfaces::IPlayMovie::IID );
    if( component )
    {
      viewer->setTool( component );
    }
  }  
}