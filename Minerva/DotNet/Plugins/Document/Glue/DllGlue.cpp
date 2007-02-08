
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

  _planet = new Magrathea::Planet();
  _planet->ref();
  _planet->init();
  this->elevationExag( 1.0f );
  _planet->root()->addChild( _sceneManager->root() );
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

  Usul::Pointers::unreference( _planet );
  _planet = 0x0;
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

  Usul::Pointers::unreference( _planet );
  _planet = 0x0;
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


///////////////////////////////////////////////////////////////////////////////
//
//  Dirty the scene.
//
///////////////////////////////////////////////////////////////////////////////

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
      _planet->viewer( _viewer->queryInterface( Usul::Interfaces::IUnknown::IID ) );
      _viewer->navManip( _planet->manipulator() );
      _viewer->databasePager( _planet->databasePager() );

      _viewer->computeNearFar( false );

      _viewer->scene( _planet->root() );
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


///////////////////////////////////////////////////////////////////////////////
//
//  Add a keyword list.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::addKeyWordList( System::String^ kwl )
{
  try
  {
    _planet->readKWL( Usul::Strings::convert ( kwl ).c_str() );
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


///////////////////////////////////////////////////////////////////////////////
//
//  Add an image.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::addLayer( CadKit::OSSIMPlanet::Glue::ImageLayer ^ layer )
{
  if( nullptr != layer )
  {
    // Add layer to group.
    osg::ref_ptr< ossimPlanetTextureLayer > texture ( reinterpret_cast < ossimPlanetTextureLayer* > ( layer->nativeIntPtr().ToPointer() ) );

    if( texture.valid() )
      _planet->addLayer( texture.get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove an image.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::removeLayer( CadKit::OSSIMPlanet::Glue::ImageLayer ^ layer )
{
  if( nullptr != layer )
  {
    osg::ref_ptr< ossimPlanetTextureLayer > texture ( reinterpret_cast < ossimPlanetTextureLayer* > ( layer->nativeIntPtr().ToPointer() ) );

    if( texture.valid() )
      _planet->removeLayer( texture.get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is elevation enabled.
//
///////////////////////////////////////////////////////////////////////////////

bool DllGlue::elevationEnabled()
{
  return _planet->elevationEnabled();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the elevation enabled flag.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::elevationEnabled( bool val )
{
  _planet->elevationEnabled( val );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the HUD enabled.
//
///////////////////////////////////////////////////////////////////////////////

bool DllGlue::hudEnabled()
{
  return _planet->hudEnabled();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the hud enabled flag.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::hudEnabled( bool val )
{
  _planet->hudEnabled( val );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is ephemeris enabled?
//
///////////////////////////////////////////////////////////////////////////////

bool DllGlue::ephemerisFlag()
{
  return _planet->ephemerisFlag();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the ephemeris flag.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::ephemerisFlag( bool val )
{
  _planet->ephemerisFlag( val );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the elevation exageration flag.
//
///////////////////////////////////////////////////////////////////////////////

float DllGlue::elevationExag()
{
  return _planet->elevationExag();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the elevation exageration flag.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::elevationExag( float exag )
{
  _planet->elevationExag( exag );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the elevation patch size.
//
///////////////////////////////////////////////////////////////////////////////

int DllGlue::elevationPatchSize()
{
  return _planet->elevationPatchSize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the elevation patch size.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::elevationPatchSize( float patchSize )
{
  _planet->elevationPatchSize( patchSize );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the max level of detail.
//
///////////////////////////////////////////////////////////////////////////////

int DllGlue::levelDetail()
{
  return _planet->levelDetail();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the max level of detail.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::levelDetail( float levelDetail )
{
  _planet->levelDetail( levelDetail );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the elevation cache directory.
//
///////////////////////////////////////////////////////////////////////////////

System::String^ DllGlue::elevationCacheDir()
{
  return gcnew System::String( _planet->elevationCacheDir().c_str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the elevation cache directory.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::elevationCacheDir( System::String^ directory )
{
  _planet->elevationCacheDir( Usul::Strings::convert( directory ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the lat long grid flag.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::latLongGrid ( bool b )
{
  _planet->showLatLongGrid ( b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the lat long grid flag.
//
///////////////////////////////////////////////////////////////////////////////

bool DllGlue::latLongGrid()
{
  return _planet->showLatLongGrid();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the show legend flag.
//
///////////////////////////////////////////////////////////////////////////////

void DllGlue::showLegend( bool b )
{
  _sceneManager->showLegend( b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the show legend flag.
//
///////////////////////////////////////////////////////////////////////////////

bool DllGlue::showLegend()
{
  return _sceneManager->showLegend();
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
        double lat ( 0.0 ), lon ( 0.0 ), height ( 0.0 );

        texture->getCenterLatLonLength( lat, lon, height );
        _planet->gotoLocation ( lat, lon, height );

      }
    }
  }
}

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
        if( !_planet->hasLayerOperation( texture.get() ) )
        {
          _planet->addLayerOperation( texture.get() );
          _planet->removeLayer( texture.get() );
        }             

        if( optype != nullptr )
        {
          if( optype->Equals( "Opacity" ) )
          {            
            float oVal = static_cast< float >( ( static_cast< float >( val ) ) / ( 100.0f ) );
            _planet->opacity( oVal );   
          }
          else if( optype->Equals( "Top" ) )
          {
            _planet->top();
          }
          else if( optype->Equals( "Reference" ) )
          {
            _planet->reference();
          }
          else if( optype->Equals( "AbsoluteDifference" ) )
          {
            _planet->absoluteDifference();
          }
          else if( optype->Equals( "FalseColorReplacement" ) )
          {
            _planet->falseColorReplacement();
          }
          else if( optype->Equals( "HorizontalSwipe" ) )
          {
            float hVal = static_cast< float >( ( static_cast< float >( val ) ) / ( 100.0f ) );
            _planet->horizontalSwipe( hVal );
          }
          else if( optype->Equals( "VerticalSwipe" ) )
          {
            float vVal = static_cast< float >( ( static_cast< float >( val ) ) / ( 100.0f ) );
            _planet->verticalSwipe( vVal );
          }
        }
      }
    }
  }
}