
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base visitor class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Visitor.h"
#include "Minerva/Core/TileEngine/System.h"
#include "Minerva/Core/Data/Container.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Cast/Cast.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Trace/Trace.h"

using namespace Minerva::Core;

MINERVA_IMPLEMENT_VISITOR_CLASS ( Visitor );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Visitor::Visitor() : BaseClass()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Visitor::~Visitor()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Visitor::_destroy ), "2197133630" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy
//
///////////////////////////////////////////////////////////////////////////////

void Visitor::_destroy()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the feature.
//
///////////////////////////////////////////////////////////////////////////////

void Visitor::visit ( Minerva::Core::Data::Feature &feature )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the data object.
//
///////////////////////////////////////////////////////////////////////////////

void Visitor::visit ( Minerva::Core::Data::DataObject &dataObject )
{
  this->visit ( USUL_UNSAFE_CAST ( Feature&, dataObject ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the vector layer.
//
///////////////////////////////////////////////////////////////////////////////

void Visitor::visit ( Minerva::Core::Data::Container& vector )
{
  vector.traverse ( *this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the node.
//
///////////////////////////////////////////////////////////////////////////////

void Visitor::visit ( Minerva::Core::TileEngine::Node & )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the node.
//
///////////////////////////////////////////////////////////////////////////////

void Visitor::visit ( Minerva::Core::TileEngine::Body & body )
{
  USUL_TRACE_SCOPE;
  
  // Get body compoents.
  Minerva::Core::Data::Container::RefPtr vector ( body.vectorData() );
  Minerva::Core::Layers::RasterLayer::RefPtr raster ( body.rasterData() );
  Minerva::Core::Layers::RasterLayer::RefPtr elevation ( body.elevationData() );
  
  if ( vector.valid() )
    vector->traverse ( *this );
  
  if ( raster.valid() )
    this->visit ( *raster );
  
  if ( elevation.valid() )
    this->visit ( *elevation );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the node.
//
///////////////////////////////////////////////////////////////////////////////

void Visitor::visit ( Minerva::Core::TileEngine::Group &group )
{
  USUL_TRACE_SCOPE;
  Guard guard1 ( this->mutex() );
  Guard guard2 ( group.mutex() );

  if ( false == group.nodes().empty() )
  {
    // Typedefs.
    typedef Minerva::Core::TileEngine::Node  Node;
    typedef Minerva::Core::TileEngine::Group Group;
    
    // Get all the nodes of the group.
    Group::Nodes &nodes ( group.nodes() );
    for ( Group::Nodes::iterator i = nodes.begin(); i != nodes.end(); ++i )
    {
      Node::RefPtr &node ( *i );
      Guard guard3 ( node->mutex() );
      if ( true == node.valid() )
      {
        node->accept ( *this );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the node.
//
///////////////////////////////////////////////////////////////////////////////

void Visitor::visit ( Minerva::Core::TileEngine::System &system )
{
  USUL_TRACE_SCOPE;
  Guard guard1 ( this->mutex() );
  Guard guard2 ( system.mutex() );

  if ( 0x0 != system.body() )
  {
    system.body()->accept ( *this );
  }

  if ( 0x0 != system.satellites() && false == system.satellites()->empty() )
  {
    system.satellites()->accept ( *this );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the raster layer.
//
///////////////////////////////////////////////////////////////////////////////

void Visitor::visit ( Minerva::Core::Layers::RasterLayer &rasterLayer )
{
  this->visit ( USUL_UNSAFE_CAST ( Feature&, rasterLayer ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the raster group.
//
///////////////////////////////////////////////////////////////////////////////

void Visitor::visit ( Minerva::Core::Layers::RasterGroup &rasterGroup )
{
  rasterGroup.traverse ( *this );
}
