
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Helper class for animation notifications.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_HELPER_ANIMATION_NOTIFY_H_
#define _USUL_HELPER_ANIMATION_NOTIFY_H_

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IAnimationNotify.h"
#include "Usul/Interfaces/ITrackball.h"
#include "Usul/Pointers/Pointers.h"


namespace Usul {
namespace Helper {


struct AnimationNotify : public Usul::Base::Referenced, 
                         public Usul::Interfaces::IAnimationNotify
{
  typedef Usul::Base::Referenced BaseClass;

  USUL_DECLARE_REF_POINTERS ( AnimationNotify );
  USUL_IMPLEMENT_INLINE_TYPE_ID ( AnimationNotify );

  AnimationNotify ( Usul::Interfaces::IUnknown::RefPtr trackBall, 
                    const osg::Vec3d &center, 
                    double distance, 
                    const osg::Quat &rotation, 
                    bool makeTrackball, 
                    bool setViewerToo ) : BaseClass(), 
    _trackBall ( trackBall ), 
    _center ( center ), 
    _distance ( distance ), 
    _rotation ( rotation ), 
    _makeTrackball ( makeTrackball ), 
    _setViewerToo ( setViewerToo )
  {
  }

  void ref()
  {
    BaseClass::ref();
  }
  void unref ( bool allowDeletion = true )
  {
    BaseClass::unref ( allowDeletion );
  }

  Usul::Interfaces::IUnknown *queryInterface ( unsigned long iid )
  {
    switch ( iid )
    {
      case Usul::Interfaces::IUnknown::IID:
      case Usul::Interfaces::IAnimationNotify::IID:
        return static_cast < Usul::Interfaces::IAnimationNotify* > ( this );
      default:
        return 0x0;
    }
  }

protected:

  virtual ~AnimationNotify()
  {
  }

  virtual void animationStarted()
  {
  }

  virtual void animationStep ( unsigned int step, unsigned int totalSteps )
  {
  }

  virtual void animationStopped()
  {
    if ( true == _trackBall.valid() )
    {
      _trackBall->setTrackball ( _center, _distance, _rotation, _makeTrackball, _setViewerToo );
    }
  }

private:

  // No copying or assignment.
  AnimationNotify ( const AnimationNotify & );
  AnimationNotify &operator = ( const AnimationNotify & );

  Usul::Interfaces::ITrackball::QueryPtr _trackBall;
  const osg::Vec3d _center;
  const double _distance;
  const osg::Quat _rotation;
  const bool _makeTrackball;
  const bool _setViewerToo;
};


} // namespace Helper
} // namespace Usul


#endif // _USUL_HELPER_ANIMATION_NOTIFY_H_
