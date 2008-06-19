#include "OctTreeNode.h"

#include "Usul/Math/MinMax.h"
#include "Usul/Math/Vector3.h"

#include "OsgTools/Lod.h"
#include "OsgTools/State/StateSet.h"

#include "osg/Vec3d"
#include "osg/Plane"
#include "osg/Group"
#include "osg/Geometry"
#include "osg/Geode"
#include "osg/LOD"

#include <limits>
#include <iostream>

#include <Math.h>

//USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( OctTreeNode, OctTreeNode::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

OctTreeNode::OctTreeNode():
_bb(),
_children( 0 ),
_points( new osg::Vec3Array ),
_type( POINT_HOLDER ),
_tolerance( 1000 ),
_useLOD( true ),
_list( new std::list< osg::Vec3f > ),
_numLODs( 10 )
{

}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

OctTreeNode::~OctTreeNode()
{
	
}

/////////////////
//  PUBLIC
/////////////////


///////////////////////////////////////////////////////////////////////////////
//
//  Set value to use level of detail or not
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::useLOD( bool value )
{
  Guard guard ( this->mutex() );
  _useLOD = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the bounds of the bounding box
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::boundingBox( const osg::BoundingBox &bb )
{
  Guard guard ( this->mutex() );
  this->_bb = bb;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the type of the cell
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::type( unsigned int type )
{
  Guard guard ( this->mutex() );
  this->_type = type;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a cell to this tree node
//
///////////////////////////////////////////////////////////////////////////////

bool OctTreeNode::add( Point p )
{
  Guard guard ( this->mutex() );
  //check my bb to determine if the point fits inside me
  if( true == this->_contains( p ) )
  {

    if( _type == POINT_HOLDER && _list->size() < _tolerance )
    {
      //insert node into _cells 
      //_points->push_back( p );
      _list->push_back( p );
    }
    else
    {
      //find the bounds of the child that contains the cell
      this->_insertOrCreateChildren( p );
    }

  }
  else
    return false;

  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::tolerance( unsigned int level )
{
  Guard guard ( this->mutex() );
  _tolerance = level;
  if( _children.size() != 0 )
  {
    for( unsigned int i = 0; i < _children.size(); ++i )
    {
      _children.at( i )->tolerance( level );
    }
  }
}



///////////////////////////////////////////////////////////////////////////////
//
//  Get the Bounding Box for this node
//
///////////////////////////////////////////////////////////////////////////////

osg::BoundingBox OctTreeNode::boundingBox()
{ 
  Guard guard ( this->mutex() );
  return _bb; 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the children of this node
//
///////////////////////////////////////////////////////////////////////////////

OctTreeNode::Children OctTreeNode::children()
{ 
  Guard guard ( this->mutex() );
  return _children; 
}

  
///////////////////////////////////////////////////////////////////////////////
//
//  Get the type of this node
//
///////////////////////////////////////////////////////////////////////////////

unsigned int OctTreeNode::type()
{ 
  Guard guard ( this->mutex() );
  return _type; 
}

  
///////////////////////////////////////////////////////////////////////////////
//
//  Get the tolerance level of this node
//
///////////////////////////////////////////////////////////////////////////////

unsigned int OctTreeNode::tolerance()
{ 
  Guard guard ( this->mutex() );
  return _tolerance; 
}


osg::Node* OctTreeNode::buildScene( Unknown *caller, Unknown *progress )
{
  Guard guard ( this->mutex() );
  typedef osg::ref_ptr< osg::Geometry > GeometryPtr;
  typedef osg::ref_ptr< osg::Geode > GeodePtr;
  osg::ref_ptr< osg::LOD > lod ( new osg::LOD );
  osg::ref_ptr< osg::Group > group ( new osg::Group );

  if( POINT_HOLDER == this->type() )
  {
    // LOds
    if( _points->size() > 0 )
    {
      for( unsigned int lodLevel = 0; lodLevel < _numLODs; ++lodLevel )
      {
        //Points vertices ( new osg::Vec3Array );
        ElementsPtr indices ( new osg::DrawElementsUShort ( osg::PrimitiveSet::POINTS ) );
        unsigned int lodMultiplier = ( lodLevel * 2 ) + 1;
        indices->reserve( static_cast< unsigned int > ( ( _points->size() / lodMultiplier ) ) + 1  );
        for( unsigned short i = 0; i < _points->size(); i += lodMultiplier )
        {
          indices->push_back( i );
        }
        GeometryPtr geometry ( new osg::Geometry ); 
        geometry->setVertexArray( _points.get() );
        geometry->addPrimitiveSet( indices.get() );
      
        GeodePtr geode ( new osg::Geode );
        geode->addDrawable( geometry.get() );
        
        // High level LODs
        float minLevel ( static_cast< float > ( lodLevel ) / static_cast< float > ( _numLODs ) );
        float maxLevel ( ( static_cast< float > ( lodLevel ) + 1.0f ) / static_cast< float > ( _numLODs ) );
        float minDistance = Usul::Math::maximum( ( minLevel * _distance ), this->getBoundingRadius() * minLevel );
        float maxDistance = Usul::Math::maximum( ( maxLevel * _distance ), this->getBoundingRadius() * maxLevel );
        if( lodLevel == _numLODs - 1 )
        {
          maxDistance = Usul::Math::maximum( ( maxLevel * _distance ), std::numeric_limits< float >::max() );
        }

        lod->addChild( geode.get(), minDistance, maxDistance );
      }

      // add the lods to the main group
      group->addChild( lod.get() );
    }
  }
  else if( NODE_HOLDER == this->type() )
  { 
    for( unsigned int i = 0; i < _children.size(); ++i )
    {
      group->addChild( _children.at( i )->buildScene() );
    } 
  }

  return group.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Check to see if the point is contained within this bounding box
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::distance( float d )
{
  Guard guard ( this->mutex() );
  _distance = d;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the diagnol distance of the bounding box of this node
//
///////////////////////////////////////////////////////////////////////////////

float OctTreeNode::getBoundingRadius()
{
  Guard guard ( this->mutex() );
  Usul::Math::Vec3f min ( _bb.xMin(), _bb.yMin(), _bb.zMin() );
  Usul::Math::Vec3f max ( _bb.xMax(), _bb.yMax(), _bb.zMax() );
  float d = max.distanceSquared( min );
  d = sqrt( d );
  return d;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the vector of points from the linked list
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::buildVectors()
{
  Guard guard ( this->mutex() );
  if( true == _children.empty() )
  {
    _points->reserve( _list->size() );
    for( LinkedList::iterator iter = _list->begin(); iter != _list->end(); ++iter )
    {
      _points->push_back( (*iter) );
    }
    _list->clear();
    delete _list;
    _list = 0x0;
  }
  else
  {
    for( unsigned int i = 0; i < _children.size(); ++i )
    {
      _children.at( i )->buildVectors();
    }
  }
}
/////////////////
//  PROTECTED
/////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Check to see if the point is contained within this bounding box
//
///////////////////////////////////////////////////////////////////////////////

bool OctTreeNode::_contains( Point p )
{
  Guard guard ( this->mutex() );
  if( _bb.contains( p ) )
    return true;
  else
    return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Insert a point into a child node
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::_insertOrCreateChildren( Point p )
{
  Guard guard ( this->mutex() );
  if( _children.size() < 8 )
  {
    this->_createChildren();
    this->_reorder();
    _list->clear();
    delete _list;
    _list = 0x0;
  }

  this->_addCellToChild( p );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reorder the points so that they are distributed to all children
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::_reorder()
{
  Guard guard ( this->mutex() );
#if 0
  for( unsigned int i = 0; i < _points->size(); ++i )
  {
    this->_addCellToChild( _points->at( i ) );
  }
#else
  for( LinkedList::iterator iter = _list->begin(); iter != _list->end(); ++iter )
  {
    this->_addCellToChild( (*iter) );
  }
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create child cells and set the bounds of each one
//
///////////////////////////////////////////////////////////////////////////////

void OctTreeNode::_createChildren()
{
  Guard guard ( this->mutex() );
  this->type( NODE_HOLDER );
  _children.resize( 8 );
  
  // split my bounding boxes
  BoundingBoxVec b = this->_split();

  for( unsigned int i = 0; i < 8; i ++ )
  {
    _children.at( i ) = new OctTreeNode;
    _children.at( i )->distance( _distance );
    _children.at( i )->boundingBox( b.at( i ) );
    _children.at( i )->type( POINT_HOLDER );
    _children.at( i )->tolerance( _tolerance );

  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Split my bounding box into 8 to distribute to my children.  
//
///////////////////////////////////////////////////////////////////////////////

OctTreeNode::BoundingBoxVec OctTreeNode::_split()
{
  Guard guard ( this->mutex() );
  BoundingBoxVec b ( 0 );
  osg::Vec3d min = _bb.corner( 0 );
  osg::Vec3d max = _bb.corner( 7 );

  // box 0
  osg::BoundingBox b0( min, osg::Vec3d( min.x() + ( ( max.x() - min.x() ) / 2 ), 
                                        min.y() + ( ( max.y() - min.y() ) / 2 ),
                                        min.z() + ( ( max.z() - min.z() ) / 2 ) ) );
  // box 7
  osg::BoundingBox b7( b0.corner( 7 ), max );

  // box 4
  osg::BoundingBox b4( b0.corner( 4 ), b7.corner( 4 ) );

  // box 3
  osg::BoundingBox b3( b0.corner( 3 ), b7.corner( 3 ) ); 

  b.push_back( b0 );
  b.push_back( osg::BoundingBox( b0.corner( 1 ),  b7.corner( 1 ) ) );
  b.push_back( osg::BoundingBox( b0.corner( 2 ),  b7.corner( 2 ) ) );
  b.push_back( b3 );
  b.push_back( b4 );
  b.push_back( osg::BoundingBox( b4.corner( 1 ),  b7.corner( 5 ) ) );
  b.push_back( osg::BoundingBox( b4.corner( 2 ),  b7.corner( 6 ) ) );
  b.push_back( b7 );
  return b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a cell to 1 more more child cells
//
///////////////////////////////////////////////////////////////////////////////

bool OctTreeNode::_addCellToChild( Point p )
{
  Guard guard ( this->mutex() );
  for( unsigned int c = 0; c < _children.size(); ++c )
  {
    
    if( true == _children.at( c )->boundingBox().contains( p ) )
    {
      _children.at( c )->add( p );
    }
  }
  return true;
}




/////////////////
//  PRIVATE
/////////////////

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////


