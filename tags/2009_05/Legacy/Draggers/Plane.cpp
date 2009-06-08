
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Dragger to use with planes.
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Draggers/Plane.h"

#include "osgDB/Registry"

using namespace OsgTools;
using namespace OsgTools::Draggers;


///////////////////////////////////////////////////////////////////////////////
//
//  Default constructor.
//
///////////////////////////////////////////////////////////////////////////////

Plane::Plane() : BaseClass(),
  _rx ( new osg::MatrixTransform ),
  _ry ( new osg::MatrixTransform ),
  _rz ( new osg::MatrixTransform ),
  _x  ( new Rotate1 ( osg::Vec3( 1.0, 0.0, 0.0 ) ) ),
  _y  ( new Rotate1 ( osg::Vec3( 0.0, 1.0, 0.0 ) ) ),
  _z  ( new Translate1 ( Translate1::Z ) )
{
  _rx->addChild( _x.get() );
  _ry->addChild( _y.get() );
  _rz->addChild( _z.get() );

  osg::ref_ptr< osg::Group > group ( new osg::Group );

  group->addChild( _rx.get() );
  group->addChild( _ry.get() );
  group->addChild( _rz.get() );

  this->geometry ( Plane::DRAGGING, group.get() );
  this->geometry ( Plane::NORMAL,   group.get() );

  // Hook up the callbacks.
  _x->callback ( Dragger::START,  new LocalCallback ( *this, Dragger::START  ) );
  _y->callback ( Dragger::START,  new LocalCallback ( *this, Dragger::START  ) );
  _z->callback ( Dragger::START,  new LocalCallback ( *this, Dragger::START  ) );
  _x->callback ( Dragger::MOVE,   new LocalCallback ( *this, Dragger::MOVE   ) );
  _y->callback ( Dragger::MOVE,   new LocalCallback ( *this, Dragger::MOVE   ) );
  _z->callback ( Dragger::MOVE,   new LocalCallback ( *this, Dragger::MOVE   ) );
  _x->callback ( Dragger::FINISH, new LocalCallback ( *this, Dragger::FINISH ) );
  _y->callback ( Dragger::FINISH, new LocalCallback ( *this, Dragger::FINISH ) );
  _z->callback ( Dragger::FINISH, new LocalCallback ( *this, Dragger::FINISH ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

Plane::Plane ( const Plane &dragger, const osg::CopyOp &op ) :
  BaseClass ( dragger, op )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Plane::~Plane()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Plane::LocalCallback::LocalCallback ( Plane &d, Dragger::Command c ) : BaseClass(),
  _d ( d ),
  _c ( c )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Plane::LocalCallback::~LocalCallback()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the dragger.
//
///////////////////////////////////////////////////////////////////////////////

bool Plane::read ( osg::Object &object, osgDB::Input &fr )
{
  bool iteratorAdvanced = false;
 // Plane *t = static_cast < Plane * > ( &object );
  return iteratorAdvanced;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the dragger.
//
///////////////////////////////////////////////////////////////////////////////

bool Plane::write ( const osg::Object &object, osgDB::Output &fw )
{
 // const Plane *t = static_cast < const Plane * > ( &object );
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Register read/write functions with osgDB::Registry.
//
///////////////////////////////////////////////////////////////////////////////

osgDB::RegisterDotOsgWrapperProxy io_proxy_plane
(
  new Plane,
  "Plane",
  "Object Node Group Transform MatrixTransform",
  &Plane::read,
  &Plane::write,
  osgDB::DotOsgWrapper::READ_AND_WRITE
);


///////////////////////////////////////////////////////////////////////////////
//
//  Execute the command.
//
///////////////////////////////////////////////////////////////////////////////

void Plane::execute ( Command c, const osg::Vec3d &pt0, const osg::Vec3d &pt1 )
{
  BaseClass::execute ( c, pt0, pt1 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Callback functor for the 2D draggers.
//
///////////////////////////////////////////////////////////////////////////////

void Plane::LocalCallback::operator() ( Dragger &d )
{
  // Notify the client if we should.
  if ( Dragger::START == _c )
    _d.notify ( _c );

  // Adjust the other draggers.
  _d._adjust ( d );

  // Notify the client if we should.
  if ( Dragger::START != _c )
    _d.notify ( _c );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Adjust the other draggers. Each dragger has to remain in its local z=0 
//  plane, otherwise the ray intersection does not work. Consequently, any 
//  translation with respect to their local z-direction has to happen higher 
//  up in the scene. That is why below we sometimes move, for example, _rxy 
//  in addition to _xy.
//
///////////////////////////////////////////////////////////////////////////////

void Plane::_adjust ( Dragger &d )
{
  // See which dragger it is and adjust the others.
  if ( _x.get() == &d )
  {
    
  }
  else if ( _y.get() == &d )
  {

  }
  else if ( _z.get() == &d )
  {

  }
}

void Plane::geometryX ( State s, osg::Node *n )
{
  _x->geometry( s, n );
}

void Plane::geometryY ( State s, osg::Node *n )
{
  _y->geometry( s, n );
}

void Plane::geometryZ ( State s, osg::Node *n )
{
  _z->geometry( s, n );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the x-translation portion of the matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Plane::_setX ( double x, osg::MatrixTransform &mt )
{
  osg::Matrixd M ( mt.getMatrix() );
  osg::Vec3d t ( M.getTrans() );
  M.setTrans ( x, t[1], t[2] );
  mt.setMatrix ( M );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the y-translation portion of the matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Plane::_setY ( double y, osg::MatrixTransform &mt )
{
  osg::Matrixd M ( mt.getMatrix() );
  osg::Vec3d t ( M.getTrans() );
  M.setTrans ( t[0], y, t[2] );
  mt.setMatrix ( M );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the z-translation portion of the matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Plane::_setZ ( double z, osg::MatrixTransform &mt )
{
  osg::Matrixd M ( mt.getMatrix() );
  osg::Vec3d t ( M.getTrans() );
  M.setTrans ( t[0], t[1], z );
  mt.setMatrix ( M );
}
