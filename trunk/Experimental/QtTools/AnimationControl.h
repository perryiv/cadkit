
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
#include "QtTools/ui_AnimationControl.h"

#include "Usul/Interfaces/IActiveDocumentListener.h"
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"

#include "QtGui/QWidget"


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

protected:

  void                        _destroy();

  unsigned long _refCount;
  mutable Mutex *_mutex;
  Unknown::QueryPtr _caller;
  Unknown::QueryPtr _document;
  unsigned int _state;
};


} // namespace QtTools


#endif // _CADKIT_QT_TOOLS_ANIMATION_CONTROL_PANEL_CLASS_H_
