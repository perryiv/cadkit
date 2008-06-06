
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Screen translate dragger.
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Draggers/TranslateScreen.h"

#include "osgDB/Registry"

#include "osg/ShapeDrawable"
#include "osg/Geode"

#include <sstream>

#ifdef _WIN32
# include "windows.h"
#endif

using namespace OsgTools;
using namespace OsgTools::Draggers;


///////////////////////////////////////////////////////////////////////////////
//
//  Default constructor.
//
///////////////////////////////////////////////////////////////////////////////

TranslateScreen::TranslateScreen() : BaseClass(),
  _diff ( 0, 0, 0 )
{
  // Make a sphere.
  osg::ref_ptr<osg::Sphere> sphere ( new osg::Sphere );
  sphere->setCenter ( osg::Vec3 ( 0.0f, 0.0f, 0.0f ) );
  sphere->setRadius ( 1.0f );
  osg::ref_ptr<osg::ShapeDrawable> sd ( new osg::ShapeDrawable ( sphere.get() ) );

  // Add the sphere to a geode.
  osg::ref_ptr<osg::Geode> geode ( new osg::Geode );
  geode->addDrawable ( sd.get() );

  // Add the geode.
  this->geometry ( TranslateScreen::DRAGGING, geode.get() );
  this->geometry ( TranslateScreen::NORMAL,   geode.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

TranslateScreen::TranslateScreen ( const TranslateScreen &dragger, const osg::CopyOp &op ) :
  BaseClass ( dragger, op ),
  _diff ( dragger._diff )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

TranslateScreen::~TranslateScreen()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the dragger.
//
///////////////////////////////////////////////////////////////////////////////

bool TranslateScreen::read ( osg::Object &object, osgDB::Input &fr )
{
  bool iteratorAdvanced = false;
//  TranslateScreen *t = static_cast < TranslateScreen * > ( &object );
  return iteratorAdvanced;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the dragger.
//
///////////////////////////////////////////////////////////////////////////////

bool TranslateScreen::write ( const osg::Object &object, osgDB::Output &fw )
{
//  const TranslateScreen *t = static_cast < const TranslateScreen * > ( &object );
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Register read/write functions with osgDB::Registry.
//
///////////////////////////////////////////////////////////////////////////////

osgDB::RegisterDotOsgWrapperProxy io_proxy_TranslateScreen
(
  new TranslateScreen,
  "TranslateScreen",
  "Object Node Group Transform MatrixTransform",
  &TranslateScreen::read,
  &TranslateScreen::write,
  osgDB::DotOsgWrapper::READ_AND_WRITE
);


///////////////////////////////////////////////////////////////////////////////
//
//  Execute the command.
//
///////////////////////////////////////////////////////////////////////////////

void TranslateScreen::execute ( Command c, const osg::Vec3d &pt0, const osg::Vec3d &pt1 )
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

void TranslateScreen::_start ( const osg::Vec3d &pt0, const osg::Vec3d &pt1 )
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

void TranslateScreen::_common ( const osg::Vec3d &pt0, const osg::Vec3d &pt1 )
{
  // Find the closest point. It will fail if they are parallel lines.
  osg::Vec3d point;
  if ( !this->_closest ( pt0, pt1, point ) )
    return;

  // Set the matrix.
  const osg::Vec3d trans ( point + _diff );
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

void TranslateScreen::_move ( const osg::Vec3d &pt0, const osg::Vec3d &pt1 )
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

void TranslateScreen::_finish ( const osg::Vec3d &pt0, const osg::Vec3d &pt1 )
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

bool TranslateScreen::_closest ( const osg::Vec3d &wp0, const osg::Vec3d &wp1, osg::Vec3d &point )
{
  // Grab the matrices.
  osg::Matrixd IC;
  IC.makeIdentity();

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
