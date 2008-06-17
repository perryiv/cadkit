#include "OctTree.h"

//USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( OctTree, OctTree::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

OctTree::OctTree():
_tree( new OctTreeNode ),
_tolerance( 1000 )
{
  _tree->tolerance( _tolerance );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

OctTree::~OctTree()
{
	
}

/////////////////
//  PUBLIC
/////////////////

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void OctTree::insert( Point p )
{
  Guard guard ( this->mutex() );
  _tree->add( p );
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void OctTree::bounds( osg::BoundingBox bb )
{
  Guard guard ( this->mutex() );
  _tree->boundingBox( bb );
  _tree->distance( _tree->getBoundingRadius() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene tree from all octree nodes
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* OctTree::buildScene( Unknown *caller, Unknown *progress )
{
  Guard guard ( this->mutex() );
  osg::ref_ptr< osg::Group > group ( new osg::Group );
  
  group->addChild( _tree->buildScene( caller, progress ) );

  return group.release();
}



///////////////////////////////////////////////////////////////////////////////
//
//  Set the tolerance level
//
///////////////////////////////////////////////////////////////////////////////

void OctTree::tolerance( unsigned int t )
{ 
  Guard guard ( this->mutex() );
  _tolerance = t; 
  _tree->tolerance( t );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the tolerance level
//
///////////////////////////////////////////////////////////////////////////////

unsigned int OctTree::tolerance()
{ 
  Guard guard ( this->mutex() );
  return _tolerance; 
}

/////////////////
//  PROTECTED
/////////////////

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

/////////////////
//  PRIVATE
/////////////////

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////


