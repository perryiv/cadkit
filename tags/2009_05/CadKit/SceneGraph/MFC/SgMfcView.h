
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgMfcView.h: A view class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_MFC_WRAPPER_VIEWER_H_
#define _CADKIT_SCENEGRAPH_MFC_WRAPPER_VIEWER_H_

#include "SgMfcApi.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "SceneGraph/Core/SgViewer.h"
# include "SceneGraph/Core/SgMessage.h"
#endif


namespace CadKit
{
class SG_MFC_API SgMfcView : public CView
{
public:

  SgMfcView();

  // Get the viewer.
  const SgViewer *      getViewer() const { return _viewer.getValue(); }
  SgViewer *            getViewer()       { return _viewer.getValue(); }

  // Render the viewer if there is one.
  bool                  render();

  // Set the viewer.
  void                  setViewer ( SgViewer *viewer );

  // Stop the motion. This does nothing if it's not spinning.
  void                  stopMotion();

  // View the whole scene. This only works if there is a viewer set.
  bool                  viewAll();

protected:

  SgViewer::Ptr _viewer;
  HCURSOR _cursor;

  virtual ~SgMfcView();

  static bool           _viewerCallback ( SgViewer::Message &message );
  virtual bool          _viewerMessage  ( SgViewer::Message &message );
  static void CALLBACK  _timerCallback ( HWND hWnd, UINT message, UINT timerId, DWORD systemTime );

  //{{AFX_MSG(SgMfcView)
  afx_msg void OnSize ( UINT nType, int cx, int cy );
  afx_msg BOOL OnEraseBkgnd(CDC* pDC);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
  afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnDestroy();
	//}}AFX_MSG

  //{{AFX_VIRTUAL(SgMfcView)
  virtual void OnDraw ( CDC* pDC );
  //}}AFX_VIRTUAL

  DECLARE_DYNCREATE(SgMfcView)

  // This has to be last.
  DECLARE_MESSAGE_MAP()
};

}; // namespace CadKit

#endif // _CADKIT_SCENEGRAPH_MFC_WRAPPER_VIEWER_H_
