
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  1D translate dragger.
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Draggers/Translate1.h"

#include "osgDB/Registry"

#ifdef _DEBUG
#ifdef _WIN32
#include <sstream>
#include <windows.h>
#endif
#endif

#include <stdexcept>

using namespace OsgTools;
using namespace OsgTools::Draggers;

///////////////////////////////////////////////////////////////////////////////
//
//  Default constructor.
//
///////////////////////////////////////////////////////////////////////////////

Translate1::Translate1( ) : BaseClass(),
_plane ( X ),
_diff( )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Translate1::Translate1( Plane p ) : BaseClass(),
_plane ( p ),
_diff( )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

Translate1::Translate1 ( const Translate1 &dragger, const osg::CopyOp &op ) :
  BaseClass ( dragger, op ),
  _plane ( dragger._plane ),
  _diff ( dragger._diff )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Translate1::~Translate1()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the dragger.
//
///////////////////////////////////////////////////////////////////////////////

bool Translate1::read ( osg::Object &object, osgDB::Input &fr )
{
  bool iteratorAdvanced = false;
//  Translate1 *t = static_cast < Translate1 * > ( &object );
  return iteratorAdvanced;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the dragger.
//
///////////////////////////////////////////////////////////////////////////////

bool Translate1::write ( const osg::Object &object, osgDB::Output &fw )
{
//  const Translate1 *t = static_cast < const Translate1 * > ( &object );
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Register read/write functions with osgDB::Registry.
//
///////////////////////////////////////////////////////////////////////////////

osgDB::RegisterDotOsgWrapperProxy io_proxy_translate1
(
  new Translate1,
  "Translate1",
  "Object Node Group Transform MatrixTransform",
  &Translate1::read,
  &Translate1::write,
  osgDB::DotOsgWrapper::READ_AND_WRITE
);



///////////////////////////////////////////////////////////////////////////////
//
//  Execute the command.
//
///////////////////////////////////////////////////////////////////////////////

void Translate1::execute ( Command c, const osg::Vec3d &pt0, const osg::Vec3d &pt1 )
{
  // Call the appropriate function.
  switch ( c )
  {
    case Dragger::START:
      this->_start ( pt0, pt1 );
      break;
    case Dragger::MOVE:
      this->_move ( pt0, pt1 );
      break;
    case Dragger::FINISH:
      this->_finish ( pt0, pt1 );
      break;
  };

  // Call the base class's function.
  BaseClass::execute ( c, pt0, pt1 );
}


///////////////////////////////////////////////////////////////////////////////
//
//	This gets called when the user clicks on a dragger.
//
///////////////////////////////////////////////////////////////////////////////

void Translate1::_start ( const osg::Vec3d &pt0, const osg::Vec3d &pt1 )
{
  // Find the closest point. It will fail if they are parallel lines.
  osg::Vec3d initial;
  if ( !this->_closest ( pt0, pt1, initial ) )
    return;

  // Notify the client.
  this->notify ( Dragger::START );

  // Get the current translation.
  const osg::Matrixd &M = this->getMatrix();
  osg::Vec3d trans ( M.getTrans() );

  // Save the initial difference in the click location and the dragger's 
  // location. This is to prevent it from "jumping".
  _diff = trans - initial;
}


///////////////////////////////////////////////////////////////////////////////
//
//	Move and finish both call this.
//
///////////////////////////////////////////////////////////////////////////////

void Translate1::_common ( const osg::Vec3d &pt0, const osg::Vec3d &pt1 )
{
  // Find the closest point. It will fail if they are parallel lines.
  osg::Vec3d point;
  if ( !this->_closest ( pt0, pt1, point ) )
    return;

  // Set the matrix.
  osg::Vec3d trans ( 0.0, 0.0, 0.0 );
  
  if( _plane == X )
  {
    trans.x() = point.x() + _diff.x();
  }
  else if ( _plane == Y )
  {
    trans.y() = point.y() + _diff.y();
  }
  else if ( _plane == Z )
  {
    trans.z() = point.x() + _diff.z();
  }
  else
  {
    throw std::runtime_error ( "Error 1441441782: Invalid plane value." );
  }

  osg::MatrixTransform::setMatrix ( osg::Matrixd::translate ( trans ) );

#if 0
  std::ostringstream out;
  const osg::Matrixf M = this->getMatrix();
  out << "translation = " << M.getTrans()[0] << " " << M.getTrans()[1] << " " << M.getTrans()[2] << std::endl;
  ::OutputDebugString ( out.str().c_str() );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//	This gets called when the user moves a dragger.
//
///////////////////////////////////////////////////////////////////////////////

void Translate1::_move ( const osg::Vec3d &pt0, const osg::Vec3d &pt1 )
{
  // Delegate.
  this->_common ( pt0, pt1 );

  // Notify the client.
  this->notify ( Dragger::MOVE );
}


///////////////////////////////////////////////////////////////////////////////
//
//	This gets called when the user releases a dragger.
//
///////////////////////////////////////////////////////////////////////////////

void Translate1::_finish ( const osg::Vec3d &pt0, const osg::Vec3d &pt1 )
{
  // Delegate.
  this->_common ( pt0, pt1 );

  // Notify the client.
  this->notify ( Dragger::FINISH );
}


/////////////////////////////////////////////////////////////////////////////
//
//	Get the closest point on the draggers plane to the given line. Return
//	false if they are parallel.
//
/////////////////////////////////////////////////////////////////////////////

bool Translate1::_closest ( const osg::Vec3d &wp0, const osg::Vec3d &wp1, osg::Vec3d &point )
{
  // Grab the matrices.
  const osg::Matrixd &C = Translate1::dragging().value().cumulative;
  osg::Matrixd IC;
  if ( !IC.invert ( C ) )
    return false;

  // Define a local xy-plane.
  osg::Vec3d planeOrigin ( 0, 0, 0 );
  osg::Vec3d planeNormal ( 0, 0, 1 );

  // Transform the given line to local space.
  const osg::Vec3d lp0 ( wp0 * IC );
  const osg::Vec3d lp1 ( wp1 * IC );

  // Define a line as a point and direction vector.
  osg::Vec3d linePoint ( lp0 );
  osg::Vec3d lineDir   ( lp1 - linePoint );
  lineDir.normalize();

  // Find the location on the plane that is closest to this line.
  // It will fail if they are parallel.
  return this->_intersect ( planeOrigin, planeNormal, linePoint, lineDir, point );
}

