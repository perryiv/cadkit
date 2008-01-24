
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  1D rotate dragger.
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Draggers/Rotate1.h"

#include "osgDB/Registry"

#ifdef _DEBUG
#ifdef _WIN32
#include <sstream>
#include <windows.h>
#endif
#endif

using namespace OsgTools;
using namespace OsgTools::Draggers;

///////////////////////////////////////////////////////////////////////////////
//
//  Default constructor.
//
///////////////////////////////////////////////////////////////////////////////

Rotate1::Rotate1( ) : BaseClass(),
_axis( 1.0, 0.0, 0.0 ),
_diff( )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Rotate1::Rotate1( const osg::Vec3d& axis ) : BaseClass(),
_axis( axis ),
_diff( )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

Rotate1::Rotate1 ( const Rotate1 &dragger, const osg::CopyOp &op ) :
  BaseClass ( dragger, op ),
  _axis ( dragger._axis ),
  _diff ( dragger._diff )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Rotate1::~Rotate1()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the dragger.
//
///////////////////////////////////////////////////////////////////////////////

bool Rotate1::read ( osg::Object &object, osgDB::Input &fr )
{
  bool iteratorAdvanced = false;
 // Rotate1 *t = static_cast < Rotate1 * > ( &object );
  return iteratorAdvanced;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the dragger.
//
///////////////////////////////////////////////////////////////////////////////

bool Rotate1::write ( const osg::Object &object, osgDB::Output &fw )
{
//  const Rotate1 *t = static_cast < const Rotate1 * > ( &object );
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Register read/write functions with osgDB::Registry.
//
///////////////////////////////////////////////////////////////////////////////

osgDB::RegisterDotOsgWrapperProxy io_proxy_rotate1
(
  new Rotate1,
  "Rotate1",
  "Object Node Group Transform MatrixTransform",
  &Rotate1::read,
  &Rotate1::write,
  osgDB::DotOsgWrapper::READ_AND_WRITE
);



///////////////////////////////////////////////////////////////////////////////
//
//  Execute the command.
//
///////////////////////////////////////////////////////////////////////////////

void Rotate1::execute ( Command c, const osg::Vec3d &pt0, const osg::Vec3d &pt1 )
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

void Rotate1::_start ( const osg::Vec3d &pt0, const osg::Vec3d &pt1 )
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

void Rotate1::_common ( const osg::Vec3d &pt0, const osg::Vec3d &pt1 )
{
  // Find the closest point. It will fail if they are parallel lines.
  osg::Vec3d point;
  if ( !this->_closest ( pt0, pt1, point ) )
    return;

  osg::Vec3 from ( _diff );
  osg::Vec3 to ( point );

  from.normalize();
  to.normalize();

  double angle ( ::acos( from * to ) );

  osg::MatrixTransform::setMatrix( osg::Matrixd::rotate( angle, _axis ) );

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

void Rotate1::_move ( const osg::Vec3d &pt0, const osg::Vec3d &pt1 )
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

void Rotate1::_finish ( const osg::Vec3d &pt0, const osg::Vec3d &pt1 )
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

bool Rotate1::_closest ( const osg::Vec3d &wp0, const osg::Vec3d &wp1, osg::Vec3d &point )
{
  // Grab the matrices.
  const osg::Matrixd &C = Rotate1::dragging().value().cumulative;
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

