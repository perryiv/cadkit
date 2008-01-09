
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Control panel for animations.
//
///////////////////////////////////////////////////////////////////////////////

#include "QtTools/AnimationControl.h"
#include "QtTools/Image.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Bits/Bits.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Exceptions/Thrower.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/IRedraw.h"
#include "Usul/Scope/Caller.h"
#include "Usul/Trace/Trace.h"
#include "Usul/Threads/Named.h"

#include "QtCore/QTimer"

using namespace QtTools;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

AnimationControl::AnimationControl ( Unknown *caller, QWidget *parent  ) : BaseClass ( parent ),
  _refCount     ( 0 ),
  _mutex        ( new AnimationControl::Mutex ),
  _caller       ( caller ),
  _document     ( static_cast < Unknown* > ( 0x0 ) ),
  _data         ( static_cast < ITimeVaryingData * > ( 0x0 ) ),
  _state        ( 0 ),
  _timer        ( 0x0 ),
  _milliSeconds ( 1000 ),
  _loop         ( false )
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD_OR_THROW ( "1692127533" );

  // We want to be notified when the active document changes.
  Usul::Documents::Manager::instance().addActiveDocumentListener ( this );

  // Initialize code from Designer.
  this->setupUi ( this );

  // Set icon buttons.
  QtTools::Image::icon ( "animation_control_play_forward.png",  _playForwardButton );
  QtTools::Image::icon ( "animation_control_play_backward.png", _playBackwardButton );
  QtTools::Image::icon ( "animation_control_step_forward.png",  _stepForwardButton );
  QtTools::Image::icon ( "animation_control_step_backward.png", _stepBackwardButton );
  QtTools::Image::icon ( "animation_control_stop.png",          _stopButton );

  // Set icon sizes.
  QSize size ( 32, 32 );
  _playForwardButton->setIconSize ( size );
  _playBackwardButton->setIconSize ( size );
  _stepForwardButton->setIconSize ( size );
  _stepBackwardButton->setIconSize ( size );
  _stopButton->setIconSize ( size );

  // Set gui members before attaching slots.
  _loopCheckBox->setChecked ( _loop );
  _speedSpinBox->setValue ( _milliSeconds / 1000.0 );

  // Make the timer.
  _timer = new QTimer ( this );

  // Connect slots.
  this->_slotsConnect();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

AnimationControl::~AnimationControl()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &AnimationControl::_destroy ), "3603850681" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy.
//
///////////////////////////////////////////////////////////////////////////////

void AnimationControl::_destroy()
{
  USUL_TRACE_SCOPE;

  // Remove this from the list of active document listeners.
  Usul::Documents::Manager::instance().removeActiveDocumentListener ( this );

  // Need local scope.
  {
    Guard guard ( this );

    // Stop and delete the timer.
    if ( 0x0 != _timer )
    {
      _timer->stop();
      delete _timer;
      _timer = 0x0;
    }

    // Done with these.
    _caller   = static_cast < Unknown* > ( 0x0 );
    _document = static_cast < Unknown* > ( 0x0 );
    _data     = static_cast < ITimeVaryingData* > ( 0x0 );

    // Should be true.
    USUL_ASSERT ( 0 == _refCount );
  }

  // Do this now.
  USUL_THREADS_ENSURE_GUI_THREAD_OR_THROW ( "2023346938" );

  // Done with mutex.
  delete _mutex; _mutex = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *AnimationControl::queryInterface ( unsigned long iid )
{
  USUL_TRACE_SCOPE;

  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IActiveDocumentListener::IID:
    return static_cast < Usul::Interfaces::IActiveDocumentListener * > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Increment the reference count.
//
///////////////////////////////////////////////////////////////////////////////

void AnimationControl::ref()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  ++_refCount;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Decrement the reference count.
//
///////////////////////////////////////////////////////////////////////////////

void AnimationControl::unref ( bool )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( 0 == _refCount )
  {
    USUL_ASSERT ( 0 );
    Usul::Exceptions::Thrower<std::runtime_error> ( "Error 3256730757: Reference count is already 0" );
  }

  --_refCount;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the mutex.
//
///////////////////////////////////////////////////////////////////////////////

AnimationControl::Mutex &AnimationControl::mutex() const
{
  USUL_TRACE_SCOPE;
  return *_mutex;
}


///////////////////////////////////////////////////////////////////////////////
//
//  The active document has changed.
//
///////////////////////////////////////////////////////////////////////////////

void AnimationControl::activeDocumentChanged ( Usul::Interfaces::IUnknown *oldDoc, Usul::Interfaces::IUnknown *newDoc )
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );
  Guard guard ( this );

  // Disconnect the slots and make sure they are re-connected.
  Usul::Scope::Caller::RefPtr scope ( Usul::Scope::makeCaller ( 
    Usul::Adaptors::memberFunction ( this, &AnimationControl::_slotsDisconnect ),
    Usul::Adaptors::memberFunction ( this, &AnimationControl::_slotsConnect ) ) );

  // Set the members.
  _document = newDoc;
  _data = newDoc;

  // Set the values.
  const bool enabled ( true == _data.valid() );
  _playForwardButton->setEnabled ( enabled );
  _playBackwardButton->setEnabled ( enabled );
  _stepForwardButton->setEnabled ( enabled );
  _stepBackwardButton->setEnabled ( enabled );
  _stopButton->setEnabled ( enabled );
  _loopCheckBox->setEnabled ( enabled );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Macros to make slot.
//
///////////////////////////////////////////////////////////////////////////////

#define IMPLEMENT_SLOT_0(slot_name,function_name,error_id) \
void AnimationControl::slot_name() \
{ \
  USUL_TRACE_SCOPE; \
  USUL_THREADS_ENSURE_GUI_THREAD ( return ); \
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &AnimationControl::function_name ), error_id ); \
}

#define IMPLEMENT_SLOT_1(slot_name,function_name,error_id,argument_type) \
void AnimationControl::slot_name ( argument_type the_value ) \
{ \
  USUL_TRACE_SCOPE; \
  USUL_THREADS_ENSURE_GUI_THREAD ( return ); \
  Usul::Functions::safeCallV1 ( Usul::Adaptors::memberFunction ( this, &AnimationControl::function_name ), the_value, error_id ); \
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make slots.
//
///////////////////////////////////////////////////////////////////////////////

IMPLEMENT_SLOT_0 ( _onPlayForward,  _playForwardEvent,  "2991793146" );
IMPLEMENT_SLOT_0 ( _onPlayBackward, _playBackwardEvent, "3231596619" );
IMPLEMENT_SLOT_0 ( _onStepForward,  _stepForwardEvent,  "1110757894" );
IMPLEMENT_SLOT_0 ( _onStepBackward, _stepBackwardEvent, "1598944220" );
IMPLEMENT_SLOT_0 ( _onStopPlaying,  _stopPlayingEvent,  "7441062000" );
IMPLEMENT_SLOT_0 ( _onTimer,        _timerEvent,        "4231156446" );
IMPLEMENT_SLOT_1 ( _onLoop,         _loopEvent,         "1080906911", bool );
IMPLEMENT_SLOT_1 ( _onSpeedChanged, _speedChangedEvent, "1431849635", double );


///////////////////////////////////////////////////////////////////////////////
//
//  Connect slots.
//
///////////////////////////////////////////////////////////////////////////////

void AnimationControl::_slotsConnect()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );
  Guard guard ( this );

  QObject::connect ( _playForwardButton,  SIGNAL ( clicked() ), this, SLOT ( _onPlayForward()  ) );
  QObject::connect ( _playBackwardButton, SIGNAL ( clicked() ), this, SLOT ( _onPlayBackward() ) );
  QObject::connect ( _stepForwardButton,  SIGNAL ( clicked() ), this, SLOT ( _onStepForward()  ) );
  QObject::connect ( _stepBackwardButton, SIGNAL ( clicked() ), this, SLOT ( _onStepBackward() ) );
  QObject::connect ( _stopButton,         SIGNAL ( clicked() ), this, SLOT ( _onStopPlaying()  ) );

  QObject::connect ( _loopCheckBox, SIGNAL ( toggled ( bool ) ), this, SLOT ( _onLoop ( bool ) ) );

  QObject::connect ( _speedSpinBox, SIGNAL ( valueChanged ( double ) ), this, SLOT ( _onSpeedChanged ( double ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Disconnect slots.
//
///////////////////////////////////////////////////////////////////////////////

void AnimationControl::_slotsDisconnect()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );
  Guard guard ( this );

  _playForwardButton->disconnect ( this );
  _playBackwardButton->disconnect ( this );
  _stepForwardButton->disconnect ( this );
  _stepBackwardButton->disconnect ( this );
  _stopButton->disconnect ( this );
  _loopCheckBox->disconnect ( this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the timer.
//
///////////////////////////////////////////////////////////////////////////////

void AnimationControl::_startTimer()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );
  Guard guard ( this );

  this->_stopTimer();

  if ( 0x0 != _timer )
  {
    this->connect ( _timer, SIGNAL ( timeout() ), this, SLOT ( _onTimer() ) );
    _timer->start ( static_cast<int> ( _milliSeconds ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Stop the timer.
//
///////////////////////////////////////////////////////////////////////////////

void AnimationControl::_stopTimer()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );
  Guard guard ( this );

  if ( 0x0 != _timer )
  {
    _timer->stop();
    _timer->disconnect ( this );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Callback for the timer.
//
///////////////////////////////////////////////////////////////////////////////

void AnimationControl::_timerEvent()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );
  Guard guard ( this );

  if ( false == Usul::Bits::has ( _state, AnimationControl::PLAYING ) )
  {
    return;
  }

  if ( true == Usul::Bits::has ( _state, AnimationControl::FORWARD ) )
  {
    this->_stepForward();
    return;
  }

  if ( true == Usul::Bits::has ( _state, AnimationControl::BACKWARD ) )
  {
    this->_stepBackward();
    return;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Play the animation forward.
//
///////////////////////////////////////////////////////////////////////////////

void AnimationControl::_playForwardEvent()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );
  Guard guard ( this );

  // Set the state.
  _state = AnimationControl::PLAYING | AnimationControl::FORWARD;

  // Start the timer.
  this->_startTimer();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Play the animation backward.
//
///////////////////////////////////////////////////////////////////////////////

void AnimationControl::_playBackwardEvent()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );
  Guard guard ( this );

  // Set the state.
  _state = AnimationControl::PLAYING | AnimationControl::BACKWARD;

  // Start the timer.
  this->_startTimer();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Move forward one step.
//
///////////////////////////////////////////////////////////////////////////////

void AnimationControl::_stepForwardEvent()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );
  Guard guard ( this );

  // Set the state.
  _state = 0;

  // Make sure the timer is stopped.
  this->_stopTimer();

  // Step forward.
  this->_stepForward();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Move backward one step.
//
///////////////////////////////////////////////////////////////////////////////

void AnimationControl::_stepBackwardEvent()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );
  Guard guard ( this );

  // Set the state.
  _state = 0;

  // Make sure the timer is stopped.
  this->_stopTimer();

  // Step backward.
  this->_stepBackward();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Stop playing the animation.
//
///////////////////////////////////////////////////////////////////////////////

void AnimationControl::_stopPlayingEvent()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );
  this->_stopPlaying();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Stop playing the animation.
//
///////////////////////////////////////////////////////////////////////////////

void AnimationControl::_stopPlaying()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );
  Guard guard ( this );

  // Set the state.
  _state = 0;

  // Make sure the timer is stopped.
  this->_stopTimer();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Move forward one step.
//
///////////////////////////////////////////////////////////////////////////////

void AnimationControl::_stepForward()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Move forward if we can.
  const unsigned int numSteps ( ( true == _data.valid() ) ? _data->getNumberOfTimeSteps() : 0 );
  if ( numSteps > 0 )
  {
    const unsigned int current ( _data->getCurrentTimeStep() );
    const bool last ( ( current + 1 ) == numSteps );

    // If we're on the last one, and we're not supposed to loop, then we're done.
    if ( ( true == last ) && ( false == _loop ) )
    {
      this->_stopPlaying();
      return;
    }

    // Otherwise, go to the next step (which might be the first one).
    const unsigned int next ( ( true == last ) ? 0 : ( current + 1 ) );

#if 0
#ifdef _DEBUG
    std::cout << "Forward: current: " << current << ", next: " << next << ", last: " << last << std::endl;
#endif
#endif

    _data->setCurrentTimeStep ( next );

    // Render the views now.
    this->_render();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Move backward one step.
//
///////////////////////////////////////////////////////////////////////////////

void AnimationControl::_stepBackward()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Move backward if we can.
  const unsigned int numSteps ( ( true == _data.valid() ) ? _data->getNumberOfTimeSteps() : 0 );
  if ( numSteps > 0 )
  {
    const unsigned int current ( _data->getCurrentTimeStep() );
    const bool first ( 0 == current );

    // If we're on the first one, and we're not supposed to loop, then we're done.
    if ( ( true == first ) && ( false == _loop ) )
    {
      this->_stopPlaying();
      return;
    }

    // Otherwise, go to the next step (which might be the last one).
    const unsigned int last ( _data->getNumberOfTimeSteps() - 1 );
    const unsigned int next ( ( true == first ) ? last : ( current - 1 ) );

#if 0
#ifdef _DEBUG
    std::cout << "Backward: current: " << current << ", next: " << next << ", last: " << last << std::endl;
#endif
#endif

    _data->setCurrentTimeStep ( next );

    // Render the views now.
    this->_render();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the document's modified flag.
//
///////////////////////////////////////////////////////////////////////////////

void AnimationControl::_render()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );
  Guard guard ( this );

  Usul::Interfaces::IRedraw::QueryPtr draw ( _document );
  if ( true == draw.valid() )
  {
    draw->redraw();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the button is pressed.
//
///////////////////////////////////////////////////////////////////////////////

void AnimationControl::_loopEvent ( bool state )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _loop = state;
}


///////////////////////////////////////////////////////////////////////////////
//
//  The property changed.
//
///////////////////////////////////////////////////////////////////////////////

void AnimationControl::_speedChangedEvent ( double speed )
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );
  Guard guard ( this );

  _milliSeconds = speed * 1000.0;

  if ( true == _timer->isActive() )
  {
    this->_stopTimer();
    this->_startTimer();
  }
}
