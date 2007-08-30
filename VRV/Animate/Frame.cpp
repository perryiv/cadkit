
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "VRV/Animate/Frame.h"

using namespace VRV::Animate;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Frame::Frame () : 
  BaseClass (),
  _m ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Frame::Frame ( const osg::Matrix& m ) : 
  BaseClass (),
  _m ( m )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Frame::~Frame ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Frame::matrix ( const osg::Matrix& m )
{
  Guard guard ( this->mutex () );
  _m = m;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the matrix.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Matrix& Frame::matrix () const
{
  Guard guard ( this->mutex () );
  return _m;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the rotation.
//
///////////////////////////////////////////////////////////////////////////////

osg::Quat Frame::rotation () const
{
  Guard guard ( this->mutex () );
  return _m.getRotate ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the translation.
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec3 Frame::translation () const
{
  osg::Vec3 v;
  
  {
    Guard guard ( this->mutex () );
    v.set ( _m ( 3, 0 ), _m ( 3, 1 ), _m ( 3, 2 ) );
  }
  
  return v;
}
