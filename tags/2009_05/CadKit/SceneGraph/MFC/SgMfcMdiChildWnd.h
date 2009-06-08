
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
// SgMfcMdiChildWnd.h: The frame window for an MDI application.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_MFC_WRAPPER_MDI_FRAME_WINDOW_H_
#define _CADKIT_SCENEGRAPH_MFC_WRAPPER_MDI_FRAME_WINDOW_H_

#include "SgMfcApi.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "SceneGraph/Core/SgGroup.h"
# include "Standard/SlRefPtr.h"
#endif


namespace CadKit
{
class SgMfcView;
class SgGroup;
struct SlType;

class SG_MFC_API SgMfcMdiChildWnd : public CMDIChildWnd
{
public:

  const SgGroup *     getRoot() const { return _root.getValue(); }
  SgGroup *           getRoot()       { return _root.getValue(); }

  const SgMfcView *   getView() const { return _view; }
  SgMfcView *         getView()       { return _view; }

protected:

  DECLARE_DYNCREATE(SgMfcMdiChildWnd)

  SgMfcView *_view;
  SlRefPtr<SgGroup> _root;

  SgMfcMdiChildWnd();
  virtual ~SgMfcMdiChildWnd();

  //{{AFX_VIRTUAL(SgMfcMdiChildWnd)
  virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
  //}}AFX_VIRTUAL

  //{{AFX_MSG(SgMfcMdiChildWnd)
	afx_msg void OnDestroy();
	//}}AFX_MSG

  // This has to be last.
  DECLARE_MESSAGE_MAP()
};

}; // namespace CadKit

#endif // _CADKIT_SCENEGRAPH_MFC_WRAPPER_MDI_FRAME_WINDOW_H_

