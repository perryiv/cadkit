// MainFrm.cpp : implementation of the CMainFrame class
//

#pragma warning(disable:4786) // Truncated debug names.

#include "stdafx.h"
#include "Cube.h"

#include "MainFrm.h"

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
  // Seed the random-number generator with current time so that
  // the numbers will be different every time we run.
  ::srand ( (unsigned int) ::time ( NULL ) );

  // For making randome numbers in the range [0,1]
  float normalize = 1.0f / ( (float) RAND_MAX );
  #define RANDOM_NUMBER ((float) ::rand()) * normalize

  // Make a separator for our scene and attach it to the inherited _root.
  SgSeparator::Ptr root = new SgSeparator;
  _root->addChild ( root );

  for ( int i = 0; i < 10; ++i )
  {
    // A separator for the i'th group of nodes.
    SgSeparator::Ptr branch = new SgSeparator;
    branch->removePushPopFlags ( SgSeparator::PROJECTION );

    // Add some material to the scene.
	  SgMaterial::Ptr material = new SgMaterial;
    SlVec4f diffuse ( RANDOM_NUMBER, RANDOM_NUMBER, RANDOM_NUMBER, 1.0f );
    material->material.setDiffuse ( diffuse );
	  material->side = SgMaterial::FRONT;
	  branch->addChild ( material );

    // Now the cube.
    SgCube::Ptr cube = new SgCube;
	  cube->setSize ( 10.0f );
	  cube->setCenter ( SlVec3f ( 3.0f, 0.0f, -10.0f ) );
	  branch->addChild ( cube );

    // A sphere.
//    SgSphere::Ptr sphere = new SgSphere;
//    sphere->setNumSubdivisions ( 4 );
//    branch->addChild ( sphere );

    // A rotation about an axis.
    SlVec3f axis ( 1.0f, 1.0f, 1.0f );
    axis.normalize();
    branch->addChild ( new SgRotation ( axis, 10.0f ) );

    // Make a translation.
    SgTranslation::Ptr trans = new SgTranslation;
//    trans->setTranslation ( SlVec3f ( sphere->getRadius() * 2, 0.0f, 0.0f ) );
    trans->setTranslation ( SlVec3f ( cube->getSize() * 2, 0.0f, 0.0f ) );
    branch->addChild ( trans );

    // Add the i'th branch to the root.
    root->addChild ( branch );

    // With this line we create nested groups.
    root = branch;
  }
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
