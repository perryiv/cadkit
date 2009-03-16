
//////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Manipulator.h"

using namespace Minerva::Document;


//////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Manipulator::Manipulator( Minerva::Core::Navigator* navigator ) : BaseClass(),
  _ga_t1 ( 0x0 ),
  _ga_t0 ( 0x0 ),
  _navigator ( navigator )
{
}


//////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Manipulator::~Manipulator()
{
  _navigator = 0x0;
}


//////////////////////////////////////////////////////////////////////////////
//
//  Set by a matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Manipulator::setByMatrix(const osg::Matrixd& matrix)
{
  if ( true == _navigator.valid() )
    _navigator->viewMatrix ( matrix );
}


//////////////////////////////////////////////////////////////////////////////
//
//  Set by a matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Manipulator::setByInverseMatrix ( const osg::Matrixd& matrix )
{
  this->setByMatrix ( osg::Matrixd::inverse ( matrix ) );
}


//////////////////////////////////////////////////////////////////////////////
//
//  Get the matrix.
//
///////////////////////////////////////////////////////////////////////////////

osg::Matrixd Manipulator::getMatrix() const
{
  return ( _navigator.valid() ? _navigator->viewMatrix() : osg::Matrixd() );
}


//////////////////////////////////////////////////////////////////////////////
//
//  Get the inverse matrix.
//
///////////////////////////////////////////////////////////////////////////////

osg::Matrixd Manipulator::getInverseMatrix() const
{
  return osg::Matrix::inverse ( this->getMatrix() );
}


//////////////////////////////////////////////////////////////////////////////
//
//  Go home.
//
///////////////////////////////////////////////////////////////////////////////

void Manipulator::home(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& us)
{
  _navigator->home();
}


//////////////////////////////////////////////////////////////////////////////
//
//  Initialize.
//
///////////////////////////////////////////////////////////////////////////////

void Manipulator::init(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& us)
{
}


//////////////////////////////////////////////////////////////////////////////
//
//  Handle events, return true if handled, false otherwise.
//
///////////////////////////////////////////////////////////////////////////////

bool Manipulator::handle( const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& us )
{
  switch( ea.getEventType() )
  {
  case osgGA::GUIEventAdapter::PUSH:
    return this->_mouseButtonPressed( ea, us );
  case osgGA::GUIEventAdapter::RELEASE:
    return this->_mouseButtonReleased( ea, us );
  case osgGA::GUIEventAdapter::DRAG:
    return this->_mouseDragged( ea, us );
  }

  return false;
}


//////////////////////////////////////////////////////////////////////////////
//
//  Flush event cache.
//
///////////////////////////////////////////////////////////////////////////////

void Manipulator::_flushEvents()
{
  _ga_t1 = 0x0;
  _ga_t0 = 0x0;
}


//////////////////////////////////////////////////////////////////////////////
//
//  Add an event.
//
///////////////////////////////////////////////////////////////////////////////

void Manipulator::_addEvent( const osgGA::GUIEventAdapter& ea )
{
  _ga_t1 = _ga_t0;
  _ga_t0 = &ea;
}


//////////////////////////////////////////////////////////////////////////////
//
//  A button has been pressed.
//
///////////////////////////////////////////////////////////////////////////////

bool Manipulator::_mouseButtonPressed( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
{
  this->_flushEvents();
  this->_addEvent ( ea );

  if ( this->_calculateMovement())
    us.requestRedraw();

  return true;
}


//////////////////////////////////////////////////////////////////////////////
//
//  A button has been released.
//
///////////////////////////////////////////////////////////////////////////////

bool Manipulator::_mouseButtonReleased( const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& us )
{
  this->_flushEvents();
  this->_addEvent( ea );

  if ( this->_calculateMovement())
    us.requestRedraw();

  return true;
}


//////////////////////////////////////////////////////////////////////////////
//
//  The mouse is being dragged.
//
///////////////////////////////////////////////////////////////////////////////

bool Manipulator::_mouseDragged( const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& us )
{
  this->_addEvent( ea );
  
  if ( this->_calculateMovement() )
    us.requestRedraw();

  return true;
}


//////////////////////////////////////////////////////////////////////////////
//
//  Calculate any movement.
//
///////////////////////////////////////////////////////////////////////////////

bool Manipulator::_calculateMovement()
{
  // Return now if no navigator.
  if ( false == _navigator.valid() )
    return false;

  // Return if less then two events have been added.
  if ( false == _ga_t0.valid() || false == _ga_t1.valid() )
    return false;

  const float dx ( _ga_t0->getXnormalized() - _ga_t1->getXnormalized() );
  const float dy ( _ga_t0->getYnormalized() - _ga_t1->getYnormalized() );

  // return if there is no movement.
  if ( dx == 0.0f && dy == 0.0f )
    return false;

  Minerva::Core::Navigator::MousePosition p0 ( _ga_t0->getXnormalized(), _ga_t0->getYnormalized() );
  Minerva::Core::Navigator::MousePosition p1 ( _ga_t1->getXnormalized(), _ga_t1->getYnormalized() );

  const unsigned int buttonMask ( _ga_t1->getButtonMask() );
  if ( buttonMask == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON )
  {
    _navigator->rotate ( p0, p1 );
    return true;
  }
  else if ( buttonMask == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON ||
      buttonMask == ( osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON | osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON))
  {
    _navigator->zoomLOS ( p0, p1 );
    return true;
  }
  else if ( buttonMask == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON )
  {
    _navigator->look ( p0, p1 );
    return true;
  }
  else if ( buttonMask == ( osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON | osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON ) )
  {
    _navigator->elevation ( p0, p1 );
    return true;
  }

  return false;
}
