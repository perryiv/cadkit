
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __CADKIT_HELIOS_QT_VIEWS_OSG_VIEWER_H__
#define __CADKIT_HELIOS_QT_VIEWS_OSG_VIEWER_H__

#include "Helios/Qt/Views/OSG/Export.h"

#include "Usul/Documents/Document.h"
#include "Usul/Interfaces/GUI/IQuestion.h"
#include "Usul/Interfaces/GUI/ISaveFileDialog.h"
#include "Usul/Interfaces/GUI/IWindow.h"
#include "Usul/Interfaces/IOpenGLContext.h"
#include "Usul/Interfaces/ITimeoutAnimate.h"
#include "Usul/Interfaces/ITimeoutSpin.h"
#include "Usul/Interfaces/IModifiedObserver.h"
#include "Usul/Interfaces/IRenderLoop.h"
#include "Usul/Interfaces/IRedraw.h"
#include "Usul/Interfaces/IMenuAdd.h"
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"

#include "OsgTools/Render/Viewer.h"

#include "QtOpenGL/QGLWidget"

#include <map>

class QTimer;


namespace CadKit {
namespace Helios {
namespace Views {
namespace OSG {


class HELIOS_QT_VIEWS_OSG_EXPORT Viewer : public QGLWidget,
                                          public Usul::Interfaces::IOpenGLContext,
                                          public Usul::Interfaces::IWindow,
                                          public Usul::Interfaces::ITimeoutAnimate,
                                          public Usul::Interfaces::ITimeoutSpin,
                                          public Usul::Interfaces::IModifiedObserver,
                                          public Usul::Interfaces::IRenderLoop,
                                          public Usul::Interfaces::IRedraw,
                                          public Usul::Interfaces::IMenuAdd,
                                          public Usul::Interfaces::IQuestion,
                                          public Usul::Interfaces::ISaveFileDialog
{
  Q_OBJECT

public:

  /// Typedefs.
  typedef QGLWidget BaseClass;
  typedef Usul::Documents::Document Document;
  typedef std::map<int,bool> KeyMap;
  typedef OsgTools::Render::Viewer::ViewMode ViewMode;
  typedef Usul::Threads::RecursiveMutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;
  typedef Usul::Interfaces::IUnknown IUnknown;

  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Constructor.
  Viewer ( Document *doc, const QGLFormat& format, QWidget* parent, IUnknown *caller );
  virtual ~Viewer ();

  /// Get the document.
  Document *                              document();
  const Document *                        document() const;

  /// Get the caller.
  IUnknown *                              caller();
  const IUnknown *                        caller() const;

  /// Edit the background.
  void                                    editBackground();

  // Get the mutex.
  Mutex &                                 mutex() const;

  /// Get the viewer.
  OsgTools::Render::Viewer*               viewer();
  const OsgTools::Render::Viewer*         viewer() const;

  /// Usul::Interfaces::IOpenGLContext
  virtual bool                            isContextThread() const;
  virtual void                            makeCurrent();
  virtual void                            swapBuffers();

  /// Usul::Interfaces::IWindow
  virtual void                            setFocus();
  virtual void                            setTitle ( const std::string& title );
  virtual void                            forceClose();

  /// Usul::Interfaces::ITimeoutAnimate
  virtual void                            startAnimation ( double timeout );

  /// Usul::Interfaces::ITimeoutSpin
  virtual void                            stopSpin();
  virtual void                            startSpin ( double timeout );

  /// Called when the document is modified (Usul::Interfaces::IModifiedObserver).
  virtual void                            subjectModified ( Usul::Interfaces::IUnknown *caller = 0x0 );

  /// Get/Set render loop flag (IRenderLoop).
  virtual void                            renderLoop ( bool b );
  virtual bool                            renderLoop () const;

  /// Redraw the view (IRedraw).
  virtual void                            redraw();
  virtual void                            setStatsDisplay ( bool );

  /// Add to the menu (IMenuAdd).
  virtual void                            menuAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown * caller = 0x0 );

  /// Update the cursor.
  void                                    updateCursor();
  void                                    updateCursor( bool left, bool middle, bool right );

protected:

  void                                    _frameDump ( bool b );
  bool                                    _frameDumpProperties();
  bool                                    _isFrameDump() const;

  void                                    _initPlacement ();
  void                                    _resize ( unsigned int w, unsigned int h );
  bool                                    _isSize ( unsigned int w, unsigned int h ) const;
  void                                    _customSize();
  void                                    _closeEvent ( QCloseEvent* event );
  void                                    _close();

  /// Add to the menu.
  virtual void                            _menuAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown * caller = 0x0 );

  /// Prompt the user (IQuestion).
  virtual std::string                     question ( const std::string &buttons, const std::string &title, const std::string &text );

  // Get the name of the file to save to (ISaveFileDialog).
  virtual FileResult                      getSaveFileName  ( const std::string &title = "Save", const Filters &filters = Filters() );

  // Override these events.
  virtual void                            paintEvent  ( QPaintEvent * event );
  virtual void                            resizeEvent ( QResizeEvent * event );
  virtual void                            focusInEvent ( QFocusEvent * event );
  virtual void                            focusOutEvent ( QFocusEvent * event );
  virtual void                            mouseMoveEvent ( QMouseEvent * event );
  virtual void                            mousePressEvent ( QMouseEvent * event );
  virtual void                            mouseReleaseEvent ( QMouseEvent * event );
  virtual void                            keyPressEvent ( QKeyEvent * event );
  virtual void                            keyReleaseEvent ( QKeyEvent * event );
  virtual void                            dragEnterEvent ( QDragEnterEvent *event );
  virtual void                            dropEvent      ( QDropEvent      *event );
  virtual void                            closeEvent ( QCloseEvent *event );

private slots:

  void                                    _onTimeoutAnimation();
  void                                    _onTimeoutSpin();
  void                                    _onTimeoutRenderLoop();
  void                                    _onContextMenuShow ( const QPoint& pos );

private:

  IUnknown::QueryPtr _caller;
  Document::RefPtr _document;
  OsgTools::Render::Viewer::RefPtr _viewer;
  unsigned int _refCount;
  QTimer *_timer;
  QTimer *_timerRenderLoop;
  KeyMap _keys;
  ViewMode _lastMode;
  unsigned long _threadId;
  mutable Mutex *_mutex;
};


}
}
}
}


#endif // __CADKIT_HELIOS_QT_VIEWS_OSG_VIEWER_H__