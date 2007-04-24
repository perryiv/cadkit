
//////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Magrathea/Manipulator.h"

#define USE_OSSIM_PLANET_MANIPULATOR 0

using namespace Magrathea;


//////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Manipulator::Manipulator() : BaseClass(),
_ga_t1 ( 0x0 ),
_ga_t0 ( 0x0 ),
_navigator ( 0x0 )
{
  _navigator = new Navigator(new ossimPlanetPrimaryBody("earth_wgs84",
                                                            6378137.0,
                                                         6356752.3142,
                                                                86400,
                                                            5.9742e24,
                                                                 -180,
                                                                  180));
}


//////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Manipulator::~Manipulator()
{
}

void Manipulator::gotoLocation( double lat, double lon, double height )
{
#if USE_OSSIM_PLANET_MANIPULATOR
  theNavigator->setLatLonHeight( lat, lon, height );
  theNavigator->update();
#else
  _navigator->setLatLonHeight ( lat, lon, height );
#endif
}


/** set the position of the matrix manipulator using a 4x4 Matrix.*/
void Manipulator::setByMatrix(const osg::Matrixd& matrix)
{
#if USE_OSSIM_PLANET_MANIPULATOR
  BaseClass::setByMatrix ( matrix );
#else
  _navigator->viewMatrix ( matrix );
#endif
}

/** get the position of the manipulator as 4x4 Matrix.*/
osg::Matrixd Manipulator::getMatrix() const
{
#if USE_OSSIM_PLANET_MANIPULATOR
  return BaseClass::getMatrix();
#else
  return _navigator->viewMatrix();
#endif
}

/** get the position of the manipulator as a inverse matrix of the manipulator, typically used as a model view matrix.*/
osg::Matrixd Manipulator::getInverseMatrix() const
{
#if USE_OSSIM_PLANET_MANIPULATOR
  return BaseClass::getInverseMatrix();
#else
  return osg::Matrix::inverse( this->getMatrix() );
#endif
}

/** Move the camera to the default position. 
   May be ignored by manipulators if home functionality is not appropriate.*/
void Manipulator::home(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& us)
{
#if USE_OSSIM_PLANET_MANIPULATOR
  BaseClass::home ( ea, us );
#else
  _navigator->home();
#endif
}

/** Start/restart the manipulator.*/
void Manipulator::init(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& us)
{
#if USE_OSSIM_PLANET_MANIPULATOR
  BaseClass::init ( ea, us );
#endif
}

/** handle events, return true if handled, false otherwise.*/
bool Manipulator::handle( const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& us )
{
#if USE_OSSIM_PLANET_MANIPULATOR
  return BaseClass::handle ( ea, us );
#else
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
#endif 
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

bool Manipulator::_mouseButtonPressed( const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& us )
{
  this->_flushEvents();
  this->_addEvent(ea);
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
  this->_addEvent(ea);
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
  // return if less then two events have been added.
  if ( false == _ga_t0.valid() || false == _ga_t1.valid() )
    return false;

  float dx ( _ga_t0->getXnormalized() - _ga_t1->getXnormalized() );
  float dy ( _ga_t0->getYnormalized() - _ga_t1->getYnormalized() );

  // return if there is no movement.
  if (dx==0.0f && dy==0.0f)
    return false;

  osg::Vec3f p0 ( _ga_t0->getXnormalized(), _ga_t0->getYnormalized(), 0.0 );
  osg::Vec3f p1 ( _ga_t1->getXnormalized(), _ga_t1->getYnormalized(), 0.0 );

  unsigned int buttonMask ( _ga_t1->getButtonMask() );
  if (buttonMask == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
  {
    _navigator->rotate( p0, p1 );
    return true;
  }
  else if (buttonMask == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON ||
      buttonMask == ( osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON | osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON))
  {
    _navigator->elevation( p0, p1 );
    return true;
  }
  else if (buttonMask == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)
  {
    _navigator->look( p0, p1 );
    return true;
  }
  else if ( buttonMask == ( osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON | osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON ) )
  {
    _navigator->zoomLOS( p0, p1 );
    return true;
  }
  else if ( buttonMask == ( osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON | osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON ) )
  {
    _navigator->rotateLOS( p0, p1 );
    return true;
  }

  return false;
}


//////////////////////////////////////////////////////////////////////////////
//
//  Set the planet.
//
///////////////////////////////////////////////////////////////////////////////

void Manipulator::planet( ossimPlanet * planet )
{
  _navigator->planet ( planet );
}
