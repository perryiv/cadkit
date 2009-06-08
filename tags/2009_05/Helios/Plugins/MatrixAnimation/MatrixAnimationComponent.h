
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MATRIX_ANIMATION_COMPONENT_CLASS_H_
#define _MATRIX_ANIMATION_COMPONENT_CLASS_H_

#include "Usul/Base/Object.h"
#include "Usul/Interfaces/IAnimateMatrices.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/ITimerNotify.h"


class MatrixAnimationComponent : public Usul::Base::Object,
                                 public Usul::Interfaces::IPlugin,
                                 public Usul::Interfaces::IAnimateMatrices,
                                 public Usul::Interfaces::ITimerNotify
{
public:

  // Typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Interfaces::IAnimateMatrices IAnimateMatrices;
  typedef IAnimateMatrices::Matrices Matrices;
  typedef Usul::Interfaces::ITimerNotify::TimerID TimerID;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( MatrixAnimationComponent );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Default construction.
  MatrixAnimationComponent();

  // Usul::Interfaces::IAnimateMatrices.
  virtual void                  animateMatrices ( const Matrices &, unsigned int milliSeconds, bool loop, IUnknown::RefPtr caller );

  // Usul::Interfaces::IPlugin
  virtual std::string           getPluginName() const { return "Matrix Animation"; }

  // Called when the timer fires (ITimerNotify).
  virtual void                  timerNotify ( TimerID );
  
protected:

  // Use reference counting.
  virtual ~MatrixAnimationComponent();

  void                          _notifyStarted();
  void                          _notifyStep ( unsigned int step, unsigned int totalSteps );
  void                          _notifyStopped();

  void                          _timerStart ( unsigned int milliSeconds );
  void                          _timerStop();

private:

  // Do not copy.
  MatrixAnimationComponent ( const MatrixAnimationComponent & );
  MatrixAnimationComponent &operator = ( const MatrixAnimationComponent & );

  Matrices _matrices;
  unsigned int _current;
  TimerID _timer;
  bool _loop;
  IUnknown::RefPtr _caller;
};


#endif // _MATRIX_ANIMATION_COMPONENT_CLASS_H_
