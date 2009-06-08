
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Qt viewer class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SHOWTIME_QT_VIEWER_H_
#define _SHOWTIME_QT_VIEWER_H_

#include "Display/View/Viewer.h"

#include "Usul/Interfaces/GUI/IWindow.h"
#include "Usul/Interfaces/IDocument.h"
#include "Usul/Interfaces/IInputListener.h"
#include "Usul/Interfaces/IModifiedObserver.h"
#include "Usul/Interfaces/IOpenGLContext.h"
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"

#include "QtOpenGL/QGLWidget"

#include <map>
#include <vector>

namespace Display { namespace Events { class Event; } }


namespace Showtime {


class Viewer : public QGLWidget,
               public Usul::Interfaces::IOpenGLContext,
               public Usul::Interfaces::IWindow,
               public Usul::Interfaces::IModifiedObserver
{
  Q_OBJECT

public:

  // Typedefs.
  typedef QGLWidget BaseClass;
  typedef Usul::Interfaces::IDocument IDocument;
  typedef Usul::Threads::RecursiveMutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Display::View::Viewer::RefPtr ViewerPtr;
  typedef Usul::Interfaces::IInputListener IInputListener;
  typedef IInputListener::KeysDown KeysDown;
  typedef IInputListener::ButtonsDown ButtonsDown;
  typedef std::pair<KeysDown,ButtonsDown> InputState;
  typedef std::vector<IUnknown::RefPtr> ListenerSequence;
  typedef std::map<InputState,ListenerSequence> ListenerMap;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Viewer );

  // Usul::Interfaces::IUnknown
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Constructor.
  Viewer ( IUnknown::RefPtr doc, const QGLFormat &format, QWidget *parent, IUnknown *caller );
  virtual ~Viewer();

  // Get the document.
  IUnknown::RefPtr                        document();
  const IUnknown::RefPtr                  document() const;

  // Get the caller.
  IUnknown::RefPtr                        caller();
  const IUnknown::RefPtr                  caller() const;

  // Return the set of keys that are currenty down.
  KeysDown                                keysDown() const;

  // Get the mutex.
  Mutex &                                 mutex() const;

  // Usul::Interfaces::IOpenGLContext
  virtual bool                            isContextThread() const;
  virtual void                            makeCurrent();
  virtual void                            swapBuffers();

  // Usul::Interfaces::IWindow
  virtual void                            setFocus();
  virtual void                            setTitle ( const std::string &title );
  virtual void                            forceClose();

  // Usul::Interfaces::IModifiedObserver
  virtual void                            subjectModified ( Usul::Interfaces::IUnknown *caller = 0x0 );

  // Preferred size. Needed for cascading.
  virtual QSize                           sizeHint() const;

  // Load the saved state.
  void                                    stateLoad();

  // Get the viewer.
  ViewerPtr                               viewer();
  const ViewerPtr                         viewer() const;

protected:

  void                                    _closeEvent ( QCloseEvent* event );
  void                                    _close();

  void                                    _initPlacement();

  void                                    _notify ( Display::Events::Event & );

  // Override these events.
  virtual void                            paintGL();
  virtual void                            resizeGL ( int width, int height );
  virtual void                            focusInEvent ( QFocusEvent * );
  virtual void                            focusOutEvent ( QFocusEvent * );
  virtual void                            mouseMoveEvent ( QMouseEvent * );
  virtual void                            mousePressEvent ( QMouseEvent * );
  virtual void                            mouseReleaseEvent ( QMouseEvent * );
  virtual void                            mouseDoubleClickEvent ( QMouseEvent * );
  virtual void                            keyPressEvent ( QKeyEvent * );
  virtual void                            keyReleaseEvent ( QKeyEvent * );
  virtual void                            dragEnterEvent ( QDragEnterEvent * );
  virtual void                            dropEvent ( QDropEvent * );
  virtual void                            closeEvent ( QCloseEvent * );
  virtual void                            wheelEvent ( QWheelEvent * );

private:

  void                                    _destroy();

  ViewerPtr _viewer;
  IUnknown::RefPtr _caller;
  IUnknown::RefPtr _document;
  unsigned int _refCount;
  unsigned long _threadId;
  mutable Mutex *_mutex;
  KeysDown _keysDown;
};


} // namespace Showtime


#endif // _SHOWTIME_QT_VIEWER_H_
