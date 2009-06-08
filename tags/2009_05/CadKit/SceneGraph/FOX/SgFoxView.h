
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  OpenGL view for the GUI-FOX library.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_GUI_FOX_OPENGL_VIEW_H_
#define _CADKIT_GUI_FOX_OPENGL_VIEW_H_

#include "SgFoxCanvas.h"

#include "SceneGraph/Core/SgViewer.h"

#include "Standard/SlAutoPtr.h"


namespace CadKit
{
class SgViewer;
class SgMessage;

class SG_FOX_API SgFoxView : public SgFoxCanvas
{
 public:

  // Possible cursors to get/set.
  enum Cursor { ROTATE, TRANSLATE, SCALE, VIEWING, PICKING, SEEKING };

  SgFoxView ( FXComposite *parent, FXGLVisual *visual );
  virtual ~SgFoxView();

  // Get the cursor.
  const FXCursor *              getCursor ( const Cursor &which ) const;
  FXCursor *                    getCursor ( const Cursor &which );

  // Initialize.
  bool                          init();

  // Message map(s).
  long                          onConfigure ( FXObject *object, FXSelector id, void *data );
  long                          onKeyPress ( FXObject *object, FXSelector id, void *data );
  long                          onKeyRelease ( FXObject *object, FXSelector id, void *data );
  long                          onLeftBtnPress ( FXObject *object, FXSelector id, void *data );
  long                          onLeftBtnRelease ( FXObject *object, FXSelector id, void *data );
  long                          onMiddleBtnPress ( FXObject *object, FXSelector id, void *data );
  long                          onMiddleBtnRelease ( FXObject *object, FXSelector id, void *data );
  long                          onMotion ( FXObject *object, FXSelector id, void *data );
  long                          onRightBtnPress ( FXObject *object, FXSelector id, void *data );
  long                          onRightBtnRelease ( FXObject *object, FXSelector id, void *data );
  long                          onPaint ( FXObject *object, FXSelector id, void *data );
  long                          onTimeout ( FXObject *object, FXSelector id, void *data );

  // Render the viewer if there is one.
  bool                          render();

  // Set the cursor.
  bool                          setCursor ( const Cursor &which, FXCursor *cursor );

  // Set the viewer.
  void                          setViewer ( SgViewer *viewer );

  // Spin the geometry.
  bool                          spin();

  // Stop the spinning.
  bool                          stopSpin();

  // View the whole scene. This only works if there is a viewer set.
  bool                          viewAll ( bool renderNow );

 protected:

  SgViewer::Ptr _viewer;
  FXTimer *_timer;
  SlAutoPtr<FXCursor> _rotateCursor;
  SlAutoPtr<FXCursor> _translateCursor;
  SlAutoPtr<FXCursor> _scaleCursor;
  SlAutoPtr<FXCursor> _viewingCursor;
  SlAutoPtr<FXCursor> _pickingCursor;
  SlAutoPtr<FXCursor> _seekingCursor;

  SgFoxView();

  SlUint32                      _getMouseButtonsDown ( const FXEvent *event ) const;

  void                          _setCursor ( FXCursor *cursor );

  static bool                   _viewerMessageCallback ( SgViewer::Message &message );
  bool                          _viewerMessageExecution ( SgViewer::Message &message );

  FXDECLARE ( SgFoxView );
};


};


#endif
