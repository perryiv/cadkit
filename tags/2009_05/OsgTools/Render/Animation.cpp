
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Helper class for animations.
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Render/Animation.h"

#include "Usul/System/Clock.h"

#include "osg/Matrixd"

using namespace OsgTools;
using namespace OsgTools::Render;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Animation::Animation() : 
  _start ( 0 ),
  _stop  ( 0 ),
  _t1    ( 0, 0, 0 ),
  _r1    ( 0, 0, 0, 0 ),
  _t2    ( 0, 0, 0 ),
  _r2    ( 0, 0, 0, 0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Animation::~Animation()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize local animation class.
//
///////////////////////////////////////////////////////////////////////////////

void Animation::init ( const osg::Vec3 &t1, const osg::Quat &r1, 
                       const osg::Vec3 &t2, const osg::Quat &r2,
                       double duration )
{
  _start = Usul::System::Clock::milliseconds();
  _stop  = _start + duration;
  _t1    = t1;
  _r1    = r1;
  _t2    = t2;
  _r2    = r2;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return matrix calculated from linear interpolation.
//
///////////////////////////////////////////////////////////////////////////////

bool Animation::matrix ( osg::Matrixd &mat ) const
{
  // Get the current time.
  const double current ( Usul::System::Clock::milliseconds() );

  // If we are not there yet...
  if ( current < _stop )
  {
    // The independent variable.
    const double u ( ( current - _start ) / ( _stop - _start ) );

    // Linearly interpolate.
    const osg::Vec3 t ( _t1 + ( _t2 - _t1 ) * u );
    osg::Quat r;
    r.slerp ( u, _r1, _r2 );

    // Set the matrix.
    mat.set ( osg::Matrixd ( r ) * osg::Matrixd::translate ( t ) );

    // Keep going.
    return true;
  }

  // Otherwise, we have arrived.
  else 
  {
    // Ensure that we end up where we are supposed to be. If we don't do this, 
    // then depending on how long things take in the above loop, the object may 
    // not be left in the final position.
    mat.set ( osg::Matrixd ( _r2 ) * osg::Matrixd::translate ( _t2 ) );

    // Should stop.
    return false;
  }
}
