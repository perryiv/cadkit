
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Trackball manipulator.
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Render/Trackball.h"

#include "Usul/Registry/Constants.h"
#include "Usul/Shared/Preferences.h"

#include "osg/Quat"

using namespace OsgTools::Render;



///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Trackball::Trackball() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

Trackball::Trackball ( const Trackball &copy, const osg::CopyOp &options ) : BaseClass()
{
  this->center   ( copy.center() );
  this->distance ( copy.distance() );
  this->rotation ( copy.rotation() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Trackball::~Trackball()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the inverse matrix of the manipulator. Does the same thing as the 
//  base class's version, but I think this is easier to follow.
//
///////////////////////////////////////////////////////////////////////////////

osg::Matrixd Trackball::getInverseMatrix() const
{
  osg::Matrixd TC ( osg::Matrixd::translate ( -_center ) );
  osg::Matrixd IR ( osg::Matrixd::rotate ( _rotation.inverse() ) );
  osg::Matrixd TD ( osg::Matrixd::translate ( 0, 0, -_distance ) );
  osg::Matrixd M ( TC * IR * TD );
  return M;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Move the camera to the default position. Not sure if checking the scene 
//  is important. Keeping this because it is easier to debug this way.
//
///////////////////////////////////////////////////////////////////////////////

void Trackball::home ( const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa )
{
  // Get the scene.
  const osg::Node *scene = this->getNode();

  // Handle no scene.
  if ( !scene )
    return;

  // Call the base class's function.
  BaseClass::home ( ea, aa );

  // The base class uses a rotated coordinate system. Undo that.
  _rotation.set ( osg::Vec4 ( 0, 0, 0, 1 ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle events.
//
///////////////////////////////////////////////////////////////////////////////

bool Trackball::handle ( const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa )
{
  // If we released the mouse, and we are not allowed to spin, then flush 
  // the mouse events to prevent a slight motion.
  if ( osgGA::GUIEventAdapter::RELEASE == ea.getEventType() && 
    false == Usul::Shared::Preferences::instance().getBool ( Usul::Registry::Keys::ALLOW_SPIN ) )
  {
    this->flushMouseEventStack();
    this->addMouseEvent ( ea );
  }

  // Call the base class's function.
  return BaseClass::handle ( ea, aa );
}
