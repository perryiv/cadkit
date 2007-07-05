
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
#include "Usul/Interfaces/IOpenGLContext.h"
#include "Usul/Interfaces/GUI/IWindow.h"

#include "OsgTools/Render/Viewer.h"

#include "QtOpenGL/QGLWidget"

namespace CadKit {
namespace Helios {
namespace Views {
namespace OSG {

class HELIOS_QT_VIEWS_OSG_EXPORT Viewer : public QGLWidget,
                                          public Usul::Interfaces::IOpenGLContext,
                                          public Usul::Interfaces::IWindow
{
public:
  /// Typedefs.
  typedef QGLWidget                    BaseClass;
  typedef Usul::Documents::Document    Document;

  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Constructor.
  Viewer ( Document *doc, const QGLFormat& format, QWidget* parent );
  virtual ~Viewer ();

  /// Get the document.
  Document *                              document ();
  const Document *                        document () const;

  /// Get the viewer.
  OsgTools::Render::Viewer*               viewer ();
  const OsgTools::Render::Viewer*         viewer () const;

  /// Usul::Interfaces::IOpenGLContext
  virtual void                            makeCurrent();
  virtual void                            swapBuffers();

  /// Usul::Interfaces::IWindow
  virtual void                            setFocus();
  virtual void                            setTitle ( const std::string& title );

protected:

  void                                    _initPlacement ();

  // Override these events.
  virtual void                            paintEvent  ( QPaintEvent * event );
  virtual void                            resizeEvent ( QResizeEvent * event );
  virtual void                            focusInEvent ( QFocusEvent * event );
  virtual void                            mouseMoveEvent ( QMouseEvent * event );
  virtual void                            mousePressEvent ( QMouseEvent * event );
  virtual void                            mouseReleaseEvent ( QMouseEvent * event );
  virtual void                            keyPressEvent ( QKeyEvent * event );
  virtual void                            keyReleaseEvent ( QKeyEvent * event );

private:

  Document::RefPtr                  _document;
  OsgTools::Render::Viewer::RefPtr  _viewer;

  unsigned int    _refCount;
};

}
}
}
}

#endif // __CADKIT_HELIOS_QT_VIEWS_OSG_VIEWER_H__
