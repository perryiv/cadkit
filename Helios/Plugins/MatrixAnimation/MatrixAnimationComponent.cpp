
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

#include "MatrixAnimation/MatrixAnimationComponent.h"

#include "Usul/Components/Factory.h"
#include "Usul/Components/Manager.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Interfaces/IAnimationNotify.h"
#include "Usul/Interfaces/IRedraw.h"
#include "Usul/Interfaces/ITimerService.h"
#include "Usul/Interfaces/IViewMatrix.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"

#include "OsgTools/Convert/Matrix.h"

#include "osg/Matrixd"

USUL_DECLARE_COMPONENT_FACTORY ( MatrixAnimationComponent );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( MatrixAnimationComponent, MatrixAnimationComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MatrixAnimationComponent::MatrixAnimationComponent() : 
  BaseClass(),
  _matrices(),
  _current ( 0 ),
  _timer ( 0 ),
  _loop ( false ),
  _caller ( 0x0 )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MatrixAnimationComponent::~MatrixAnimationComponent()
{
  USUL_TRACE_SCOPE;

  // Remove the timer.
  this->_timerStop();
  
  // Remove paths.
  _matrices.clear();

  // Should be true.
  USUL_ASSERT ( false == _caller.valid() );

  // Just in case... This is probably a viewer.
  _caller = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *MatrixAnimationComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin* > ( this );
  case Usul::Interfaces::IAnimateMatrices::IID:
    return static_cast < Usul::Interfaces::IAnimateMatrices * > ( this );
  case Usul::Interfaces::ITimerNotify::IID:
    return static_cast < Usul::Interfaces::ITimerNotify * > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Animate through the path.
//
///////////////////////////////////////////////////////////////////////////////

void MatrixAnimationComponent::animateMatrices ( const Matrices &matrices, unsigned int milliSeconds, bool loop, IUnknown::RefPtr caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Stop current timer.
  this->_timerStop();

  // Notify current caller, if any.
  this->_notifyStopped();

  // Assign members.
  _matrices = matrices;
  _loop = loop;
  _caller = caller;

  // We're now at the beginning.
  _current = 0;

  // If we have matrices...
  if ( false == _matrices.empty() )
  {
    // Start the timer.
    this->_timerStart ( milliSeconds );

    // Notify the caller.
    this->_notifyStarted();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the timer fires (ITimerNotify).
//
///////////////////////////////////////////////////////////////////////////////

void MatrixAnimationComponent::timerNotify ( TimerID )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // If there are not matrices then return.
  if ( true == _matrices.empty() )
    return;

  // If we got to the end...
  if ( _current >= _matrices.size() )
  {
    _current = 0;

    // Are we not supposed to loop?
    if ( false == _loop )
    {
      this->_timerStop();
      _matrices.clear();
      this->_notifyStopped();
      _caller = 0x0;
      return;
    }
  }

  // This is needed.
  Usul::Interfaces::IViewMatrix::QueryPtr vm ( Usul::Documents::Manager::instance().activeView() );
  if ( false == vm.valid() )
    return;

  // Get next matrix.
  const unsigned int current ( _current );
  const Matrices::value_type m ( _matrices.at ( _current++ ) );

  // Set the new matrix. The convention in Usul::Math::Matrix44
  // is the same as OpenGL, which is the transpose of OSG.
  typedef Usul::Convert::Type < Matrices::value_type, osg::Matrixd > Converter;
  vm->setViewMatrix ( osg::Matrixd ( Converter::convert ( m ) ) );

  // Redraw.
  Usul::Interfaces::IRedraw::QueryPtr painter ( vm );
  if ( true == painter.valid() )
  {
    painter->redraw();
  }

  // Notify the caller.
  this->_notifyStep ( current, _matrices.size() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the timer.
//
///////////////////////////////////////////////////////////////////////////////

void MatrixAnimationComponent::_timerStart ( unsigned int milliSeconds )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  
  this->_timerStop();
  
  // Handle repeated calls.
  if ( 0 != _timer )
    return;
  
  // Get the timer-server interface.
  Usul::Interfaces::ITimerService::QueryPtr ts ( Usul::Components::Manager::instance().getInterface ( Usul::Interfaces::ITimerService::IID ) );
  if ( false == ts.valid() )
    return;
  
  // Add the timer.
  _timer = ts->timerAdd ( milliSeconds, Usul::Interfaces::IUnknown::QueryPtr ( this ), true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Stop the timer.
//
///////////////////////////////////////////////////////////////////////////////

void MatrixAnimationComponent::_timerStop()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  
  // Handle repeated calls.
  if ( 0 == _timer )
    return;
  
  // Get the timer-server interface.
  Usul::Interfaces::ITimerService::QueryPtr ts ( Usul::Components::Manager::instance().getInterface ( Usul::Interfaces::ITimerService::IID ) );
  if ( false == ts.valid() )
    return;
  
  // Remove the timer.
  ts->timerRemove ( _timer );
  
  // This is important!
  _timer = 0;  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify the caller.
//
///////////////////////////////////////////////////////////////////////////////

void MatrixAnimationComponent::_notifyStarted()
{
  // Notify the caller.
  typedef Usul::Interfaces::IAnimationNotify IAnimationNotify;
  IAnimationNotify::QueryPtr notify ( Usul::Threads::Safe::get ( this->mutex(), _caller.get() ) );
  if ( true == notify.valid() )
  {
    notify->animationStarted();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify the caller.
//
///////////////////////////////////////////////////////////////////////////////

void MatrixAnimationComponent::_notifyStep ( unsigned int step, unsigned int totalSteps )
{
  // Notify the caller.
  typedef Usul::Interfaces::IAnimationNotify IAnimationNotify;
  IAnimationNotify::QueryPtr notify ( Usul::Threads::Safe::get ( this->mutex(), _caller.get() ) );
  if ( true == notify.valid() )
  {
    notify->animationStep ( step, totalSteps );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify the caller.
//
///////////////////////////////////////////////////////////////////////////////

void MatrixAnimationComponent::_notifyStopped()
{
  // Notify the caller.
  typedef Usul::Interfaces::IAnimationNotify IAnimationNotify;
  IAnimationNotify::QueryPtr notify ( Usul::Threads::Safe::get ( this->mutex(), _caller.get() ) );
  if ( true == notify.valid() )
  {
    notify->animationStopped();
  }
}
