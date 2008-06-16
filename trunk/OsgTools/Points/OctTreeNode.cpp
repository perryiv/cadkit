#include "OctTreeNode.h"

#include "Usul/Math/MinMax.h"
#include "Usul/Math/Vector3.h"

#include "OsgTools/Lod.h"

#include "osg/Vec3d"
#include "osg/Plane"
#include "osg/Group"
#include "osg/Geometry"
#include "osg/Geode"
#include "osg/LOD"



#include <limits>

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
_tolerance( 100 ),
_useLOD( true )
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

    if( _type == POINT_HOLDER && _points->size() < _tolerance )
    {
      //insert node into _cells 
      _points->push_back( p );
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
    if( _points->size() > 0 )
    {
      float highLODMax = Usul::Math::maximum( ( 0.33f * _distance ), this->getBoundingRadius() );
      float medLODMax = Usul::Math::maximum( ( 0.66f * _distance ), highLODMax * 2 );

      // Highest detail LOD
      {
        GeometryPtr geometry ( new osg::Geometry ); 
        geometry->setVertexArray( _points.get() );
        geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::POINTS, 0, _points->size() ) );

        GeodePtr geode ( new osg::Geode );
        geode->addDrawable( geometry.get() );
        
        // High level LODs
        lod->addChild( geode.get(), 0, highLODMax );
      }

      // Medium Distance LOD
      {
        Points vertices ( new osg::Vec3Array );
        for( unsigned int i = 0; i < _points->size(); i+=3 )
        {
          vertices->push_back( _points->at( i ) );
        }
        GeometryPtr geometry ( new osg::Geometry ); 
        geometry->setVertexArray( _points.get() );
        geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::POINTS, 0, vertices->size() ) );

        GeodePtr geode ( new osg::Geode );
        geode->addDrawable( geometry.get() );
        
        // High level LODs
        lod->addChild( geode.get(), highLODMax, medLODMax );
      }

      // Low Detail LOD
      {
        Points vertices ( new osg::Vec3Array );
        for( unsigned int i = 0; i < _points->size(); i+=6 )
        {
          vertices->push_back( _points->at( i ) );
        }
        GeometryPtr geometry ( new osg::Geometry ); 
        geometry->setVertexArray( _points.get() );
        geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::POINTS, 0, vertices->size() ) );

        GeodePtr geode ( new osg::Geode );
        geode->addDrawable( geometry.get() );
        
        // High level LODs
        lod->addChild( geode.get(), medLODMax, std::numeric_limits< float >::max() );
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
    _points->clear();
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
  for( unsigned int i = 0; i < _points->size(); ++i )
  {
    this->_addCellToChild( _points->at( i ) );
  }
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


