
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Jeff Conner
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IMPDNAVIGATOR_H__
#define __USUL_INTERFACES_IMPDNAVIGATOR_H__

#include "Usul/Interfaces/IUnknown.h"

//namespace osg { class Vec4; }

namespace Usul {
namespace Interfaces {


struct IMpdNavigator : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IMpdNavigator );

  /// Id for this interface.
  enum { IID = 2186138810u };

  // go to the next group in the set
  virtual void              nextGroup ( unsigned int index ) = 0;

  // start time step animation
  virtual void              startAnimation() = 0;

  // stop time step animation
  virtual void              stopAnimation() = 0;

  // Go to the next time step
  virtual void              nextStep() = 0;

  // Go to the previous time step
  virtual void              prevStep() = 0;

  // Go to the first time step
  virtual void              firstStep() = 0;

  // Setup and execute an animation path
  virtual void              setAnimationPath( const std::string & ) = 0;

  // Output the current view matrix
  virtual void              displayViewMatrix() = 0;

  


}; //struct IVertices



}
}

#endif // __USUL_INTERFACES_IMPDNAVIGATOR_H__
