
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

#ifndef _CADKIT_QT_TOOLS_ANIMATION_CONTROL_PANEL_CLASS_H_
#define _CADKIT_QT_TOOLS_ANIMATION_CONTROL_PANEL_CLASS_H_

#include "QtTools/Export.h"

#include "ui_AnimationControl.h" // Cannot have path here.

#include "Usul/Interfaces/IActiveDocumentListener.h"
#include "Usul/Interfaces/ITimeVaryingData.h"
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"

#include "QtGui/QWidget"

class QTimer;


namespace QtTools {


class QT_TOOLS_EXPORT AnimationControl : public QWidget,
                                         private Ui::AnimationControl,
                                         public Usul::Interfaces::IActiveDocumentListener
{
  Q_OBJECT;

public:

  // Useful typedefs.
  typedef QWidget BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Usul::Threads::RecursiveMutex Mutex;
  typedef Usul::Threads::Guard < Mutex > Guard;
  typedef Usul::Interfaces::ITimeVaryingData ITimeVaryingData;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( AnimationControl );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  AnimationControl ( Unknown *caller, QWidget *parent );
  virtual ~AnimationControl();

  // Possible states.
  enum State
  {
    PLAYING   = 0x00000001,
    FORWARD   = 0x00000002,
    BACKWARD  = 0x00000004
  };

  // The active document has changed (IActiveDocumentListener).
  virtual void                activeDocumentChanged ( Unknown *oldDoc, Unknown *newDoc );

  // Get the mutex.
  Mutex &                     mutex() const;

protected slots:

  void                        _onLoop ( bool );
  void                        _onPlayForward();
  void                        _onPlayBackward();
  void                        _onSliderChanged ( int );
  void                        _onSpeedChanged ( double );
  void                        _onStepForward();
  void                        _onStepBackward();
  void                        _onStopPlaying();
  void                        _onTimer();

protected:

  void                        _destroy();

  void                        _render();

  void                        _loopEvent ( bool );

  void                        _playForwardEvent();
  void                        _playBackwardEvent();

	void                        _setEnabledState();
  void                        _slotsConnect();
  void                        _slotsDisconnect();
  void                        _speedChangedEvent ( double );
  void                        _stepForward();
  void                        _stepForwardEvent();
  void                        _stepBackward();
  void                        _stepBackwardEvent();
  void                        _stopPlaying();
  void                        _stopPlayingEvent();

  void                        _startTimer();
  void                        _stopTimer();

  void                        _timerEvent();

  unsigned long _refCount;
  mutable Mutex *_mutex;
  Unknown::QueryPtr _caller;
  Unknown::QueryPtr _document;
  ITimeVaryingData::QueryPtr _data;
  unsigned int _state;
  QTimer *_timer;
  double _milliSeconds;
  bool _loop;
};


} // namespace QtTools


#endif // _CADKIT_QT_TOOLS_ANIMATION_CONTROL_PANEL_CLASS_H_
