
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base dragger class.
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Draggers/Dragger.h"

#include "osgDB/Registry"

using namespace OsgTools;
using namespace OsgTools::Draggers;


///////////////////////////////////////////////////////////////////////////////
//
//  Static data member(s).
//
///////////////////////////////////////////////////////////////////////////////

Dragger::Dragging Dragger::_dragging ( 0x0 );


///////////////////////////////////////////////////////////////////////////////
//
//  Default constructor.
//
///////////////////////////////////////////////////////////////////////////////

Dragger::Dragger() : BaseClass(),
  _callbacks(),
  _useWindowCoords ( false )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

Dragger::Dragger ( const Dragger &dragger, const osg::CopyOp &op ) :
  BaseClass ( dragger, op ),
  _callbacks()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Dragger::~Dragger()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Dragger::Data::Data ( Dragger *d, const osg::Matrixd &m ) :
  dragger    ( d ),
  cumulative ( m )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

Dragger::Data::Data ( const Data &d ) :
  dragger    ( d.dragger    ),
  cumulative ( d.cumulative )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Traverse the correct child.
//
///////////////////////////////////////////////////////////////////////////////

void Dragger::traverse ( osg::NodeVisitor &nv )
{
  // If we are supposed to traverse the active children...
  if ( osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN == nv.getTraversalMode() )
  {
    // If we have enough children...
    if ( 2 == this->getNumChildren() )
    {
      // If we are being dragged...
      if ( this->isDragging() )
      {
        // Get the correct child and pass it the visitor.
        osg::Node *child = this->getChild ( Dragger::DRAGGING );
        if ( child )
          child->accept(nv);
      }

      // Otherwise...
      else
      {
        // Get the correct child and pass it the visitor.
        osg::Node *child = this->getChild ( Dragger::NORMAL );
        if ( child )
          child->accept(nv);
      }
    }
  }

  // Otherwise...
  else
  {
    // Let the base class handle it.
    BaseClass::traverse ( nv );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the dragger.
//
///////////////////////////////////////////////////////////////////////////////

bool Dragger::read ( osg::Object &object, osgDB::Input &fr )
{
  bool iteratorAdvanced = false;
 // Dragger *d = static_cast < Dragger * > ( &object );
  return iteratorAdvanced;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the dragger.
//
///////////////////////////////////////////////////////////////////////////////

bool Dragger::write ( const osg::Object &object, osgDB::Output &fw )
{
//  const Dragger *d = static_cast < const Dragger * > ( &object );
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the dragger currently being dragged. Could be null.
//
///////////////////////////////////////////////////////////////////////////////

void Dragger::dragging ( Dragger *d, const osg::Matrixd &m )
{
  Guard guard ( _dragging.mutex() );
  _dragging.value().dragger = d;
  _dragging.value().cumulative = m;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the dragger currently being dragged. Could be null.
//
///////////////////////////////////////////////////////////////////////////////

Dragger::Dragging &Dragger::dragging()
{
  return _dragging;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is this dragger being dragged?
//
///////////////////////////////////////////////////////////////////////////////

bool Dragger::isDragging() const
{
  Guard guard ( _dragging.mutex() );
  return ( this == _dragging.value().dragger.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the geometry drawn while dragging/normal.
//
///////////////////////////////////////////////////////////////////////////////

void Dragger::geometry ( State s, osg::Node *n )
{
  // Make sure there are at least two children.
  while ( this->getNumChildren() < 2 )
    this->addChild ( new osg::Group );

  // Make sure we're in range.
  unsigned int which ( s );
  if ( which < 2 )
    this->setChild ( which, n );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute the command.
//
///////////////////////////////////////////////////////////////////////////////

void Dragger::execute ( Command c, const osg::Vec3d &pt0, const osg::Vec3d &pt1 )
{
  if ( Dragger::FINISH == c )
  {
    _dragging.value().dragger = 0x0;
    _dragging.value().cumulative = osg::Matrixd::identity();
    this->dirtyBound();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Intersect this plane with the given line. Return false if parallel.
//
///////////////////////////////////////////////////////////////////////////////

bool Dragger::_intersect ( const osg::Vec3d &planeOrigin, 
                           const osg::Vec3d &planeNormal, 
                           const osg::Vec3d &linePoint, 
                           const osg::Vec3d &lineDir, 
                           osg::Vec3d &point ) const
{
	// Check for parallel line and plane.
	double denom ( planeNormal * lineDir );
  if ( 0 == denom )
    return false;

	// Compute the intersection point.
  double dist ( planeOrigin * planeNormal );
  double ndl ( planeNormal * linePoint );
	double tt ( - ( ndl + dist ) / denom );

	point[0] = linePoint[0] + ( tt * lineDir[0] );
	point[1] = linePoint[1] + ( tt * lineDir[1] );
	point[2] = linePoint[2] + ( tt * lineDir[2] );

	return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify the client.
//
///////////////////////////////////////////////////////////////////////////////

void Dragger::notify ( Command c )
{
  unsigned int command ( static_cast < unsigned int > ( c ) );
  if ( _callbacks.size() > command && _callbacks.at(command).valid() )
    (*_callbacks.at(command)) ( *this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the callback.
//
///////////////////////////////////////////////////////////////////////////////

void Dragger::callback ( Command c, Callback *cb )
{
  this->_resize ( c );
  unsigned int command ( static_cast < unsigned int > ( c ) );
  _callbacks.at(command) = cb;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the callback.
//
///////////////////////////////////////////////////////////////////////////////

Callback *Dragger::callback ( Command c )
{
  this->_resize ( c );
  unsigned int command ( static_cast < unsigned int > ( c ) );
  return _callbacks.at(command).get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Resize the container of callbacks if the command is out of range.
//
///////////////////////////////////////////////////////////////////////////////

void Dragger::_resize ( Command c )
{
  unsigned int command ( static_cast < unsigned int > ( c ) );
  if ( command >= _callbacks.size() )
    _callbacks.resize ( command + 1 );
}
