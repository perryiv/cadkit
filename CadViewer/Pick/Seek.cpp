
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class for seeking to the intersected portion of the scene.
//
///////////////////////////////////////////////////////////////////////////////

#include "Precompiled.h"
#include "Seek.h"

#include "osg/Group"

using namespace CV;
using namespace CV::Pick;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Seek::Seek ( 
  Unknown *unknown, 
  Direction *dir,
  const std::string& name ) : 
  BaseClass ( unknown, dir, name ),
  _rc ( unknown )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

Seek::Seek ( const Seek &s ) : 
  BaseClass ( s ),
  _rc ( s._rc )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Seek::~Seek()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Seek to the intersected portion of the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Seek::seek()
{
  typedef BaseClass::Path Path;

  // Intersect the scene.
  this->_intersect ( this->_navigationScene() );

  // Did we intersect?
  if ( false == this->_valid() )
    return;

  // Get the intersected point in the navigation-scene's coordinate system.
  const osg::Vec3 &point = this->_worldIntersectPoint();

  // Make this the center of rotation.
  this->_rotationCenter ( point );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Do nothing.
//
///////////////////////////////////////////////////////////////////////////////

void Seek::operator()()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the rotation center.
//
///////////////////////////////////////////////////////////////////////////////

void Seek::_rotationCenter ( const osg::Vec3 &c )
{
  const Usul::Math::Vec3f center ( c[0], c[1], c[2] );
  _rc->rotationCenter ( center );
}
