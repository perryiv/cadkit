
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Layers/VectorGroup.h"
#include "Minerva/Core/Visitor.h"

#include "Usul/Factory/RegisterCreator.h"
#include "Usul/Functions/Execute.h"
#include "Usul/Trace/Trace.h"

#include "OsgTools/Group.h"

#include "osg/Group"

using namespace Minerva::Core::Layers;

USUL_FACTORY_REGISTER_CREATOR ( VectorGroup );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

VectorGroup::VectorGroup() :
  BaseClass(),
  _layers(),
  _root ( new osg::Group )
{
  USUL_TRACE_SCOPE;
  this->_addMember ( "layers", _layers );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

VectorGroup::~VectorGroup()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Accept the visitor.
//
///////////////////////////////////////////////////////////////////////////////

void VectorGroup::accept ( Minerva::Core::Visitor& visitor )
{
  USUL_TRACE_SCOPE;
  visitor.visit ( *this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Traverse all layers.
//
///////////////////////////////////////////////////////////////////////////////

void VectorGroup::traverse ( Minerva::Core::Visitor& visitor )
{
  USUL_TRACE_SCOPE;
  
  // Call the base class
  BaseClass::traverse ( visitor );
  
  // Visit the layers.
  Guard guard ( this );
  for ( Layers::iterator iter = _layers.begin(); iter != _layers.end(); ++iter )
    (*iter)->accept ( visitor );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node * VectorGroup::buildScene ( const Options &options, Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _root.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

void VectorGroup::_buildScene ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( true == _root.valid() || true == BaseClass::dirtyScene() )
  {
    // Remove what we have.
    OsgTools::Group::removeAllChildren ( _root.get() );
    
    // Call the base class
    _root->addChild ( BaseClass::buildScene ( Usul::Interfaces::IBuildScene::Options(), caller ) );

    for ( Layers::iterator iter = _layers.begin(); iter != _layers.end(); ++iter )
    {
      Vector::RefPtr layer ( *iter );
      if ( layer.valid() && layer->showLayer() )
        _root->addChild ( layer->buildScene ( Usul::Interfaces::IBuildScene::Options(), caller ) );
    }

    // Our scene is no longer dirty.
    this->dirtyScene ( false );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a layer.
//
///////////////////////////////////////////////////////////////////////////////

void VectorGroup::addLayer ( Vector* layer )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  
  if ( 0x0 != layer )
  {
    _layers.push_back ( layer );
  }
  
  BaseClass::dirtyScene( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a layer.
//
///////////////////////////////////////////////////////////////////////////////

void VectorGroup::removeLayer ( Vector* layer )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  Layers::iterator doomed ( std::find( _layers.begin(), _layers.end(), Usul::Interfaces::ILayer::QueryPtr ( layer ) ) );
  if( doomed != _layers.end() )
    _layers.erase( doomed );
  
  BaseClass::dirtyScene( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear layers.
//
///////////////////////////////////////////////////////////////////////////////

void VectorGroup::clearLayers()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _layers.clear();
  BaseClass::dirtyScene( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of layers (ILayer).
//
///////////////////////////////////////////////////////////////////////////////

unsigned int VectorGroup::numberLayers() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _layers.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the layer at position i (ILayer).
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::ILayer* VectorGroup::layer ( unsigned int i )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _layers.at ( i ).get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get a copy of the layers.
//
///////////////////////////////////////////////////////////////////////////////

void VectorGroup::layers ( Layers& container ) const
{
  USUL_TRACE_SCOPE;
  
  // Clear what may be in the container.
  container.clear();
  
  // Copy.
  Guard guard ( this );
  container.assign ( _layers.begin(), _layers.end() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update.
//
///////////////////////////////////////////////////////////////////////////////

void VectorGroup::updateNotify ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  
  // Capture the dirty scene state before calling the base class' update.
  const bool dirtyScene ( BaseClass::dirtyScene() );
  
  // Call the base class first.
  BaseClass::updateNotify ( caller );
  
  // Restore state.
  BaseClass::dirtyScene ( dirtyScene );
  
  if ( dirtyScene )
    this->_buildScene( caller );
  
  // Get all the layers.
  Layers layers;
  this->layers ( layers );
  
  // Ask each one to update.
  std::for_each ( layers.begin(), layers.end(), std::bind2nd ( std::mem_fun ( &Vector::updateNotify ), caller ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set dirty scene flag.
//
///////////////////////////////////////////////////////////////////////////////

void VectorGroup::dirtyScene( bool b, Usul::Interfaces::IUnknown* caller )
{
  USUL_TRACE_SCOPE;
  
  // Call the base class first.
  BaseClass::dirtyScene ( b, caller );
  
  // Get all the layers.
  Layers layers;
  this->layers ( layers );

  // Ask each one to dirty scene.
  for ( Layers::iterator iter = layers.begin(); iter != layers.end(); ++iter )
  {
    Vector::RefPtr layer ( *iter );
    if ( layer.valid() )
      layer->dirtyScene ( b, caller );
  }
  
  // This won't compile using gcc. I'm not sure why not...
  //Usul::Functions::executeMemberFunctions ( layers, &Vector::dirtyScene, b, caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of children (ITreeNode).
//
///////////////////////////////////////////////////////////////////////////////

unsigned int VectorGroup::getNumChildNodes() const
{
  USUL_TRACE_SCOPE;
  return this->numberLayers();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the child node (ITreeNode).
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::ITreeNode * VectorGroup::getChildNode ( unsigned int which )
{
  USUL_TRACE_SCOPE;
  return Usul::Interfaces::ITreeNode::QueryPtr ( this->layer ( which ) );
}
