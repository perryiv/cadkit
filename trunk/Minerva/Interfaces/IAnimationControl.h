
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_INTERFACES_IANIMATION_CONTROL_H__
#define __MINERVA_INTERFACES_IANIMATION_CONTROL_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Minerva {
namespace Interfaces {

struct IAnimationControl : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IAnimationControl );

  /// Id for this interface.
  enum { IID = 2210737686u };

  enum TimestepType
  {
    TIMESTEP_TYPE_SECOND,
    TIMESTEP_TYPE_MINUTE,
    TIMESTEP_TYPE_HOUR,
    TIMESTEP_TYPE_DAY,
    TIMESTEP_TYPE_MONTH,
    TIMESTEP_TYPE_YEAR
  };

  /// Start the animation.
  virtual void                 startAnimation () = 0;

  /// Stop the animation.
  virtual void                 stopAnimation () = 0;

   /// Stop the animation.
  virtual void                 pauseAnimation () = 0;

  /// Get/Set the animate speed.
  virtual void                 animateSpeed ( double speed ) = 0;
  virtual double               animateSpeed () const = 0;

  /// Get/Set the show past events flag.
  virtual void                 showPastEvents ( bool ) = 0;
  virtual bool                 showPastEvents () const = 0;

  /// Get/Set the timestep type.
  virtual TimestepType         timestepType() const = 0;
  virtual void                 timestepType ( TimestepType ) = 0;

}; // struct IAnimationControl

} // end namespace Interfaces
} // end namespace Minerva


#endif // __MINERVA_INTERFACES_IANIMATION_CONTROL_H__ 
