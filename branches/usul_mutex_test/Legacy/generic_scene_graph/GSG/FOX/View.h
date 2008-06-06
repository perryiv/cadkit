
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  OpenGL view.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_FOX_BINDING_VIEW_H_
#define _GENERIC_SCENE_GRAPH_FOX_BINDING_VIEW_H_

#include "GSG/FOX/Canvas.h"
#include "GSG/FOX/Cursor.h"

#include "GSG/Core/Viewer.h"


namespace GSG {

class Path;


namespace FOX {


class GSG_FOX_EXPORT View : public Canvas
{
public:

  // Possible cursors to get/set.
  enum CursorType { ROTATE, TRANSLATE, SCALE, VIEWING, PICKING, SEEKING };

  View ( FXComposite *parent, FXGLVisual *visual );
  virtual ~View();

  // Get the cursor.
  const Cursor *      getCursor ( CursorType which ) const;
  Cursor *            getCursor ( CursorType which );

  // Notify this instance of an event.
  void                notify ( Viewer::Message id, Path *path, Viewer &viewer );

  // Message map(s).
  long                onConfigure        ( FXObject *object, FXSelector id, void *data );
  long                onKeyPress         ( FXObject *object, FXSelector id, void *data );
  long                onKeyRelease       ( FXObject *object, FXSelector id, void *data );
  long                onLeftBtnPress     ( FXObject *object, FXSelector id, void *data );
  long                onLeftBtnRelease   ( FXObject *object, FXSelector id, void *data );
  long                onMiddleBtnPress   ( FXObject *object, FXSelector id, void *data );
  long                onMiddleBtnRelease ( FXObject *object, FXSelector id, void *data );
  long                onMotion           ( FXObject *object, FXSelector id, void *data );
  long                onRightBtnPress    ( FXObject *object, FXSelector id, void *data );
  long                onRightBtnRelease  ( FXObject *object, FXSelector id, void *data );
  long                onPaint            ( FXObject *object, FXSelector id, void *data );
  long                onTimeout          ( FXObject *object, FXSelector id, void *data );

  // Render the viewer if there is one.
  void                render();

  // Set the cursor.
  void                cursor ( CursorType which, FXCursor *cursor );

  // Set the viewer.
  void                viewer ( Viewer *viewer );

  // Start/stop the geometry spinning.
  void                spin ( bool );

  // View the whole scene. This only works if there is a viewer set.
  void                viewAll ( bool renderNow = true );

protected:

  View();

  unsigned int        _buttonsDown ( const FXEvent *event ) const;

  void                _setCursor ( FXCursor *cursor );

private:

  Pointer<Viewer> _viewer;
  FXTimer *_timer;
  std::auto_ptr<FX::FXCursor> _rotateCursor;
  std::auto_ptr<FX::FXCursor> _translateCursor;
  std::auto_ptr<FX::FXCursor> _scaleCursor;
  std::auto_ptr<FX::FXCursor> _viewingCursor;
  std::auto_ptr<FX::FXCursor> _pickingCursor;
  std::auto_ptr<FX::FXCursor> _seekingCursor;

  FXDECLARE ( View );
};


}; // namespace FOX
}; // namespace GSG


#endif
