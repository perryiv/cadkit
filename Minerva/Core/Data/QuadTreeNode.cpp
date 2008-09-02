
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class for node of a quad tree.
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Data/QuadTreeNode.h"

using namespace Minerva::Core::Data;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

QuadTreeNode::QuadTreeNode( const Extents& extents ) : BaseClass(),
  _extents ( extents ),
  _children ( 4 )
{
  const Extents::Vertex &mn ( extents.minimum() );
  const Extents::Vertex &mx ( extents.maximum() );
  const Extents::Vertex md ( ( mx + mn ) * 0.5 );
  
  // Extents for children nodes.
  const Extents ll ( Extents::Vertex ( mn[0], mn[1] ), Extents::Vertex ( md[0], md[1] ) );
  const Extents lr ( Extents::Vertex ( md[0], mn[1] ), Extents::Vertex ( mx[0], md[1] ) );
  const Extents ul ( Extents::Vertex ( mn[0], md[1] ), Extents::Vertex ( md[0], mx[1] ) );
  const Extents ur ( Extents::Vertex ( md[0], md[1] ), Extents::Vertex ( mx[0], mx[1] ) );
  
  // Make the children.
  _children[LOWER_LEFT]  = new QuadTreeNode ( ll );
  _children[LOWER_RIGHT] = new QuadTreeNode ( lr );
  _children[UPPER_LEFT]  = new QuadTreeNode ( ul );
  _children[UPPER_RIGHT] = new QuadTreeNode ( ur );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

QuadTreeNode::~QuadTreeNode()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the extents.
//
///////////////////////////////////////////////////////////////////////////////

QuadTreeNode::Extents QuadTreeNode::extents() const
{
  Guard guard ( this->mutex() );
  return _extents;
}
