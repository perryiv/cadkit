
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgMfcFrameWnd.h: The frame window for an SDI application.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_MFC_WRAPPER_SDI_FRAME_WINDOW_H_
#define _CADKIT_SCENEGRAPH_MFC_WRAPPER_SDI_FRAME_WINDOW_H_

#include "SgMfcApi.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "SceneGraph/Core/SgGroup.h"
# include "Standard/SlRefPtr.h"
#endif


namespace CadKit
{
class SgMfcView;
struct SlType;

class SG_MFC_API SgMfcFrameWnd : public CFrameWnd
{
public:

  const SgGroup *     getRoot() const { return _root.getValue(); }
  SgGroup *           getRoot()       { return _root.getValue(); }

  const SgMfcView *   getView() const { return _view; }
  SgMfcView *         getView()       { return _view; }

protected:

  DECLARE_DYNCREATE(SgMfcFrameWnd)

  SgMfcView *_view;
  SlRefPtr<SgGroup> _root;

  SgMfcFrameWnd();
  virtual ~SgMfcFrameWnd();

  //{{AFX_VIRTUAL(SgMfcFrameWnd)
  virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
  //}}AFX_VIRTUAL

  //{{AFX_MSG(SgMfcFrameWnd)
	afx_msg void OnDestroy();
	//}}AFX_MSG

  // This has to be last.
  DECLARE_MESSAGE_MAP()
};

}; // namespace CadKit

#endif // _CADKIT_SCENEGRAPH_MFC_WRAPPER_SDI_FRAME_WINDOW_H_
