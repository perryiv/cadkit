
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Scene/SceneManager.h"

#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Trace/Trace.h"

#include "OsgTools/State/StateSet.h"

#include "osg/Geode"
#include "osg/Material"
#include "osg/Version"

#include <algorithm>

using namespace Minerva::Core::Scene;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SceneManager::SceneManager() :
BaseClass(),
_mutex(),
_root ( new osg::Group ),
_layers(),
_dirty ( false )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SceneManager::~SceneManager()
{
  this->clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

void SceneManager::buildScene( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;

  Guard guard ( _mutex );

  if ( this->dirty() )
  {
    // Clear what we have
    _root->removeChild( 0, _root->getNumChildren() );
    
    // Loop throught the layers
    for ( Layers::iterator iter = _layers.begin(); iter != _layers.end(); ++iter )
    {
      if( iter->second->showLayer() )
      {
        Usul::Interfaces::IBuildScene::QueryPtr build ( iter->second.get() );
        if( build.valid() )
        {
          // Build the scene with default options.
          _root->addChild( build->buildScene( Usul::Interfaces::IBuildScene::Options(), caller ) );
        }
      }
    }

    _root->dirtyBound();
  }

  this->dirty( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the scene dirty?
//
///////////////////////////////////////////////////////////////////////////////

bool SceneManager::dirty() const
{
  Guard guard ( _mutex );
  return _dirty;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the dirty flag
//
///////////////////////////////////////////////////////////////////////////////

void SceneManager::dirty( bool b )
{
  Guard guard ( _mutex );
  _dirty = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear internal data.
//
///////////////////////////////////////////////////////////////////////////////

void SceneManager::clear()
{
  Guard guard ( _mutex );

  _layers.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a layer.
//
///////////////////////////////////////////////////////////////////////////////

void SceneManager::addLayer ( Layer *layer )
{
  USUL_TRACE_SCOPE;

  if( 0x0 != layer )
  {
    Guard guard ( _mutex );
    _layers[ layer->guid() ] = layer;
  }

  this->dirty( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a layer.
//
///////////////////////////////////////////////////////////////////////////////

void SceneManager::removeLayer ( const std::string& guid )
{
  USUL_TRACE_SCOPE;

  Layers::iterator iter = _layers.find ( guid );

  if ( iter != _layers.end() )
  {
    {
      Guard guard ( _mutex );
      _layers.erase ( iter );
    }
    this->dirty ( true );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is this layer being rendered?
//
///////////////////////////////////////////////////////////////////////////////

bool SceneManager::hasLayer ( const std::string& guid ) const
{
  USUL_TRACE_SCOPE;

  Guard guard ( _mutex );

  Layers::const_iterator iter = _layers.find( guid );
  if( iter != _layers.end() )
    return true;

  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the data objects for this layer.
//
///////////////////////////////////////////////////////////////////////////////

SceneManager::Layer *  SceneManager::getLayer ( const std::string& guid )
{
  USUL_TRACE_SCOPE;

  Guard guard ( _mutex );

  Layers::const_iterator iter = _layers.find( guid );
  if( iter != _layers.end() )
  {
    return _layers[guid].get();
  }

  throw std::runtime_error( "Error 284022903: could not find layer id." );
}
