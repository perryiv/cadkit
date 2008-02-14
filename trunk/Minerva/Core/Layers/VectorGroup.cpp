
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
#include "Usul/Trace/Trace.h"

#include "osg/Group"

using namespace Minerva::Core::Layers;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS( VectorGroup, VectorGroup::BaseClass );
USUL_FACTORY_REGISTER_CREATOR ( VectorGroup );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

VectorGroup::VectorGroup() :
  BaseClass(),
  _layers()
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
//  Query Interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* VectorGroup::queryInterface ( unsigned long iid )
{
  USUL_TRACE_SCOPE;
  
  switch ( iid )
  {
  case Usul::Interfaces::ILayerList::IID:
    return static_cast<Usul::Interfaces::ILayerList*> ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
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
  osg::ref_ptr < osg::Group > group ( new osg::Group );
  
  // Call the base class
  group->addChild ( BaseClass::buildScene ( options, caller ) );
  
  for ( Layers::iterator iter = _layers.begin(); iter != _layers.end(); ++iter )
  {
    Vector::RefPtr layer ( *iter );
    if ( layer.valid() && layer->showLayer() )
      group->addChild ( layer->buildScene ( options, caller ) );
  }
  return group.release();
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
