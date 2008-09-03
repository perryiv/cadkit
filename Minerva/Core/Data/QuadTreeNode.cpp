
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

#include "Usul/Threads/Safe.h"

using namespace Minerva::Core::Data;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

QuadTreeKey::QuadTreeKey ( Vertex* vertex, unsigned int index ) : BaseClass(),
  _vertex ( vertex ),
  _index ( index )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

QuadTreeKey::~QuadTreeKey()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

QuadTreeNode::QuadTreeNode( const Extents& extents ) : BaseClass(),
  _extents ( extents ),
  _children(),
  _keys()
{
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


///////////////////////////////////////////////////////////////////////////////
//
//  Split.
//
///////////////////////////////////////////////////////////////////////////////

void QuadTreeNode::_split()
{
  Guard guard ( this->mutex() );

  // Make enough room.
  _children.resize ( 4 );

  // Get the extents.
  Extents extents ( this->extents() );
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
  
  // Insert our keys into the children.
  for ( Keys::const_iterator iter = _keys.begin(); iter != _keys.end(); ++iter )
  {
    KeyPtr key ( *iter );
    _children[0]->insert ( key );
    _children[1]->insert ( key );
    _children[2]->insert ( key );
    _children[3]->insert ( key );
  }
  
  _keys.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Does this nood need to be split?
//
///////////////////////////////////////////////////////////////////////////////

bool QuadTreeNode::_needsSplit() const
{
  Guard guard ( this->mutex() );
  return _keys.size() > 100;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is this node a leaf?
//
///////////////////////////////////////////////////////////////////////////////

bool QuadTreeNode::isLeaf() const
{
  Guard guard ( this->mutex() );
  return 0 == _children.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Insert a key.
//
///////////////////////////////////////////////////////////////////////////////

void QuadTreeNode::insert ( KeyPtr key )
{
  if ( key.valid() )
  {
    const Usul::Math::Vec3d* const vertex ( key->vertex() );
    
    Guard guard ( this->mutex() );
    if ( _extents.contains ( Extents::Vertex ( (*vertex)[0], (*vertex)[1] ) ) )
    {
      _keys.push_back ( key );
    }
  }
  
  if ( this->_needsSplit() )
    this->_split();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get all the keys that are contained by the extents.
//
///////////////////////////////////////////////////////////////////////////////

QuadTreeNode::Keys QuadTreeNode::query ( const Extents& e ) const
{
  Keys keys;
  this->query ( e, keys );
  return keys;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get all the keys that are contained by the extents.
//
///////////////////////////////////////////////////////////////////////////////

void QuadTreeNode::query ( const Extents& e, Keys& keys ) const
{
  // Only proceed if the requested extents intersects with our extents.
  if ( true == this->extents().intersects( e ) )
  {
    if ( true == this->isLeaf() )
    {
      Guard guard ( this->mutex() );
      for ( Keys::const_iterator iter = _keys.begin(); iter != _keys.end(); ++iter )
      {
        KeyPtr key ( *iter );
        const Usul::Math::Vec3d* const vertex ( key->vertex() );
        if ( e.contains ( Extents::Vertex ( (*vertex)[0], (*vertex)[1] ) ) )
          keys.push_back ( key );
      }
    }
    else
    {
      Children children ( Usul::Threads::Safe::get ( this->mutex(), _children ) );
      children[0]->query ( e, keys );
      children[1]->query ( e, keys );
      children[2]->query ( e, keys );
      children[3]->query ( e, keys );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear.
//
///////////////////////////////////////////////////////////////////////////////

void QuadTreeNode::clear()
{
  Guard guard ( this->mutex() );
  _keys.clear();
  _children.clear();
}
