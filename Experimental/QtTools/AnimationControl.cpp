
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
#include "Usul/Trace/Trace.h"
#include "Usul/Threads/Named.h"

using namespace QtTools;

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

AnimationControl::AnimationControl ( Unknown *caller, QWidget *parent  ) : BaseClass ( parent ),
  _refCount ( 0 ),
  _mutex    ( new AnimationControl::Mutex ),
  _caller   ( caller ),
  _document ( Unknown::RefPtr ( 0x0 ) ),
  _state    ( 0 )
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

    // Done with these.
    _caller   = Unknown::RefPtr ( 0x0 );
    _document = Unknown::RefPtr ( 0x0 );

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
    Usul::Exceptions::Thrower<std::runtime_error> ( "Error 3225790374: Reference count is already 0" );
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
  Guard guard ( this );
  _document = newDoc;
}
