
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_VIEWER_FOX_BINDING_CANVAS_H_
#define _OSG_TOOLS_VIEWER_FOX_BINDING_CANVAS_H_

#include "Helios/Fox/Views/Export.h"

#include "Usul/Interfaces/IDocument.h"

#include "FoxTools/Headers/GLCanvas.h"
#include "FoxTools/Cursors/Factory.h"

#include "OsgTools/Render/Viewer.h"

namespace Helios {
namespace Views {

class OSG_FOX_VIEWS_EXPORT FoxCanvas : public FX::FXGLCanvas,
                                      public Usul::Interfaces::ITimeoutAnimate,
                                      public Usul::Interfaces::ITimeoutSpin,
                                      public Usul::Interfaces::ISetCursorType
{
public:

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Useful typedefs.
  typedef FX::FXGLCanvas              BaseClass;
  typedef OsgTools::Render::Viewer    Viewer;
  typedef Usul::Interfaces::IDocument Document;
  typedef FoxTools::Cursors::Factory  Cursor;
  typedef std::auto_ptr<FX::FXCursor> FXCursorPtr;

  // Message ids
  enum
  {
    ID_ANIMATION_TIMEOUT = BaseClass::ID_LAST,
    ID_SPIN_TIMEOUT,
    ID_LAST
  };

  // Construction/destruction.
  FoxCanvas ( Document *doc, FX::FXComposite *parent, FX::FXGLVisual *visual );
  virtual ~FoxCanvas();

  // Create.
  virtual void          create();

  // Get the cursor.
  const FX::FXCursor *    cursorRot()    const { return _rotCursor.get();    }
  FX::FXCursor *          cursorRot()          { return _rotCursor.get();    }
  const FX::FXCursor *    cursorTrans()  const { return _transCursor.get();  }
  FX::FXCursor *          cursorTrans()        { return _transCursor.get();  }
  const FX::FXCursor *    cursorScale()  const { return _scaleCursor.get();  }
  FX::FXCursor *          cursorScale()        { return _scaleCursor.get();  }
  const FX::FXCursor *    cursorNav()    const { return _navCursor.get();    }
  FX::FXCursor *          cursorNav()          { return _navCursor.get();    }
  const FX::FXCursor *    cursorPick()   const { return _pickCursor.get();   }
  FX::FXCursor *          cursorPick()         { return _pickCursor.get();   }

  // Get the document.
  Document *            document();
  const Document *      document() const;

  // Message maps.
  long                  onResize            ( FX::FXObject *, FX::FXSelector, void * );
  long                  onPaint             ( FX::FXObject *, FX::FXSelector, void * );
  long                  onTimeoutAnimation  ( FX::FXObject *, FX::FXSelector, void * );
  long                  onTimeoutSpin       ( FX::FXObject *, FX::FXSelector, void * );

  // Set the cursor.
  void                  setCursor ( FX::FXCursor *cursor );

  // Get the viewer.  
  Viewer*               viewer();
  const Viewer*         viewer() const;
  void                  viewer ( Viewer* );

protected:
  // Needed for a fox-macro. Do not use.
  FoxCanvas();
  
  virtual void          _resize ( unsigned int width, unsigned int height );

  /// Usul::Interfaces::ITimeoutSpin
  virtual void stopSpin ( );
  virtual void startSpin ( double timeout );

  /// Usul::Interfaces::ITimeoutAnimate
  virtual void startAnimation ( double timeout );

  /// Usul::Interfaces::ISetCursorType
  virtual void updateCursor ();
  virtual void setCursorNav();
  virtual void setCursorTrans();
  virtual void setCursorRot();
  virtual void setCursorScale();

private:
  // Do not copy.
  FoxCanvas ( const FoxCanvas & );
  FoxCanvas &operator = ( const FoxCanvas & );

  typedef std::map < unsigned int, double > TimeoutMap;

  Viewer::RefPtr  _viewer;
  TimeoutMap      _timeouts;
  FXCursorPtr     _rotCursor;
  FXCursorPtr     _transCursor;
  FXCursorPtr     _scaleCursor;
  FXCursorPtr     _navCursor;
  FXCursorPtr     _pickCursor;

  FXDECLARE ( FoxCanvas );
};

}
}

#endif // _OSG_TOOLS_VIEWER_FOX_BINDING_CANVAS_H_
