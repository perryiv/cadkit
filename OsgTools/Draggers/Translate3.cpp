
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  3D translate dragger.
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Draggers/Translate3.h"
#include "OsgTools/Draggers/Translate2.h"

#include "osgDB/Registry"

using namespace OsgTools;
using namespace OsgTools::Draggers;


///////////////////////////////////////////////////////////////////////////////
//
//  Default constructor.
//
///////////////////////////////////////////////////////////////////////////////

Translate3::Translate3() : BaseClass(),
  _rxy ( new osg::MatrixTransform ),
  _ryz ( new osg::MatrixTransform ),
  _rzx ( new osg::MatrixTransform ),
  _xy  ( new Translate2 ),
  _yz  ( new Translate2 ),
  _zx  ( new Translate2 )
{
  // Attach.
  _rxy->addChild ( _xy.get() );
  _ryz->addChild ( _yz.get() );
  _rzx->addChild ( _zx.get() );

  // Rotate the 2D draggers.
  this->_initRotations();

  // Set the dragging geometry.
  osg::ref_ptr<osg::Group> group ( new osg::Group );
  group->addChild ( _rxy.get()  );
  group->addChild ( _ryz.get() );
  group->addChild ( _rzx.get() );
  this->geometry ( Translate3::DRAGGING, group.get() );
  this->geometry ( Translate3::NORMAL,   group.get() );

  // Hook up the callbacks.
  _xy->callback ( Dragger::START,  new LocalCallback ( *this, Dragger::START  ) );
  _yz->callback ( Dragger::START,  new LocalCallback ( *this, Dragger::START  ) );
  _zx->callback ( Dragger::START,  new LocalCallback ( *this, Dragger::START  ) );
  _xy->callback ( Dragger::MOVE,   new LocalCallback ( *this, Dragger::MOVE   ) );
  _yz->callback ( Dragger::MOVE,   new LocalCallback ( *this, Dragger::MOVE   ) );
  _zx->callback ( Dragger::MOVE,   new LocalCallback ( *this, Dragger::MOVE   ) );
  _xy->callback ( Dragger::FINISH, new LocalCallback ( *this, Dragger::FINISH ) );
  _yz->callback ( Dragger::FINISH, new LocalCallback ( *this, Dragger::FINISH ) );
  _zx->callback ( Dragger::FINISH, new LocalCallback ( *this, Dragger::FINISH ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

Translate3::Translate3 ( const Translate3 &dragger, const osg::CopyOp &op ) :
  BaseClass ( dragger, op )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Translate3::~Translate3()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Translate3::LocalCallback::LocalCallback ( Translate3 &d, Dragger::Command c ) : BaseClass(),
  _d ( d ),
  _c ( c )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Translate3::LocalCallback::~LocalCallback()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the dragger.
//
///////////////////////////////////////////////////////////////////////////////

bool Translate3::read ( osg::Object &object, osgDB::Input &fr )
{
  bool iteratorAdvanced = false;
//  Translate3 *t = static_cast < Translate3 * > ( &object );
  return iteratorAdvanced;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the dragger.
//
///////////////////////////////////////////////////////////////////////////////

bool Translate3::write ( const osg::Object &object, osgDB::Output &fw )
{
//  const Translate3 *t = static_cast < const Translate3 * > ( &object );
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Register read/write functions with osgDB::Registry.
//
///////////////////////////////////////////////////////////////////////////////

osgDB::RegisterDotOsgWrapperProxy io_proxy_translate3
(
  new Translate3,
  "Translate3",
  "Object Node Group Transform MatrixTransform",
  &Translate3::read,
  &Translate3::write,
  osgDB::DotOsgWrapper::READ_AND_WRITE
);


///////////////////////////////////////////////////////////////////////////////
//
//  Execute the command.
//
///////////////////////////////////////////////////////////////////////////////

void Translate3::execute ( Command c, const osg::Vec3d &pt0, const osg::Vec3d &pt1 )
{
  BaseClass::execute ( c, pt0, pt1 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Callback functor for the 2D draggers.
//
///////////////////////////////////////////////////////////////////////////////

void Translate3::LocalCallback::operator() ( Dragger &d )
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
//  Adjust the other draggers. Each 2D dragger has to remain in its local z=0 
//  plane, otherwise the ray intersection does not work. Consequently, any 
//  translation with respect to their local z-direction has to happen higher 
//  up in the scene. That is why below we sometimes move, for example, _rxy 
//  in addition to _xy.
//
///////////////////////////////////////////////////////////////////////////////

void Translate3::_adjust ( Dragger &d )
{
  // See which dragger it is and adjust the others.
  if ( _xy.get() == &d )
  {
    osg::Vec3d xy ( _xy->getMatrix().getTrans() );
    this->_setX (  xy[0], *_ryz );
    this->_setY (  xy[1], *_yz  );
    this->_setX (  xy[0], *_zx  );
    this->_setY (  xy[1], *_rzx );
  }
  else if ( _yz.get() == &d )
  {
    osg::Vec3d yz ( _yz->getMatrix().getTrans() );
    this->_setY (  yz[1], *_xy  );
    this->_setZ ( -yz[0], *_rxy );
    this->_setY ( -yz[0], *_zx  );
    this->_setY (  yz[1], *_rzx );
  }
  else if ( _zx.get() == &d )
  {
    osg::Vec3d zx ( _zx->getMatrix().getTrans() );
    this->_setX (  zx[0], *_ryz );
    this->_setX ( -zx[1], *_yz  );
    this->_setX (  zx[0], *_xy  );
    this->_setZ (  zx[1], *_rxy );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the rotations for the individual 2D draggers.
//
///////////////////////////////////////////////////////////////////////////////

void Translate3::_initRotations()
{
  _rxy->setMatrix ( osg::Matrixd::identity() );
  _ryz->setMatrix ( osg::Matrixd::rotate ( osg::PI_2, osg::Vec3d ( 0, 1, 0 ) ) );
  _rzx->setMatrix ( osg::Matrixd::rotate ( osg::PI_2, osg::Vec3d ( 1, 0, 0 ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the x-translation portion of the matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Translate3::_setX ( double x, osg::MatrixTransform &mt )
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

void Translate3::_setY ( double y, osg::MatrixTransform &mt )
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

void Translate3::_setZ ( double z, osg::MatrixTransform &mt )
{
  osg::Matrixd M ( mt.getMatrix() );
  osg::Vec3d t ( M.getTrans() );
  M.setTrans ( t[0], t[1], z );
  mt.setMatrix ( M );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the translation.
//
///////////////////////////////////////////////////////////////////////////////

void Translate3::translation ( const osg::Vec3d &t )
{
  this->_setX ( t[0], *_xy  );
  this->_setY ( t[1], *_xy  );
  this->_setZ ( t[2], *_rxy );
  this->_adjust ( *_xy );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the translation.
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec3d Translate3::translation() const
{
  osg::Vec3d rxy ( _rxy->getMatrix().getTrans() );
  osg::Vec3d xy  (  _xy->getMatrix().getTrans() );
  return osg::Vec3d ( xy[0], xy[1], rxy[2] );
}
