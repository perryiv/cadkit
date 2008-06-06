// MainFrm.cpp : implementation of the CMainFrame class
//

#pragma warning(disable:4786) // Truncated debug names.

#include "stdafx.h"
#include "Cube.h"

#include "MainFrm.h"
#include "BuildScene.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, SgMfcFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, SgMfcFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
  // Build the scene.
  _root->addChild ( CadKit::Demos::buildScene ( 10, true ) );
}


CMainFrame::~CMainFrame()
{
  // Should be zero.
#ifdef _DEBUG
  TRACE ( "In CMainFrame::~CMainFrame(), final number of nodes = %d\n", SgNode::getNumNodes() );
#endif
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (SgMfcFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !SgMfcFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	SgMfcFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	SgMfcFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	// Call this first.
	if ( FALSE == SgMfcFrameWnd::OnCreateClient ( lpcs, pContext ) ) 
    return FALSE;

	// Should be true now.
	SL_ASSERT ( _view );
	SL_ASSERT ( _root.isValid() );

  // Make the viewer.
  SgViewer::Ptr viewer = new SgViewer;

  // Set the view's viewer.
  _view->setViewer ( viewer );

  // Make the rendering context.
  SgGlContext::Ptr context = new SgGlWinContext;
  VERIFY ( context->create ( _view->GetSafeHwnd() ) );

  // Get the client area of the view.
  CRect rect;
  _view->GetClientRect ( rect );

  // Make the renderer.
  SgGlRenderer::Ptr renderer = new SgGlRenderer;
  VERIFY ( renderer->init ( SlViewporti ( 0, 0, rect.Width(), rect.Height() ), context ) );

  // Set the viewer's renderer.
  viewer->setRenderer ( renderer );

  // Set the viewer's scene.
  viewer->setScene ( _root );

  // Make the camera.
  SgPerspectiveCamera::Ptr camera = new SgPerspectiveCamera;

  // Set the camera's aspect ratio.
  camera->setAspectRatio ( rect.Width(), rect.Height() );

  // Set the viewer's camera.
  viewer->setCamera ( camera );

  // Look at the whole scene.
  viewer->viewAll();

	// We cool.
	return TRUE;
}
