// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "StdAfx.h"
#include "HelloCube.h"
#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace CadKit;

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, SgMfcMdiChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, SgMfcMdiChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
	ON_COMMAND(ID_ORTHOGRAPHIC_CAMERA, OnOrthographicCamera)
	ON_UPDATE_COMMAND_UI(ID_ORTHOGRAPHIC_CAMERA, OnUpdateOrthographicCamera)
	ON_COMMAND(ID_PERSPECTIVE_CAMERA, OnPerspectiveCamera)
	ON_UPDATE_COMMAND_UI(ID_PERSPECTIVE_CAMERA, OnUpdatePerspectiveCamera)
	ON_COMMAND(ID_FILE_WRITE, OnFileSave)
	ON_COMMAND(ID_VIEW_ALL, OnViewAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
	//SgDrawStyle *drawStyle = new SgDrawStyle;
	//drawStyle->polygonStyle = SgDrawStyle::FILLED;
	//_root->addChild ( drawStyle );

	SgMaterial *material = new SgMaterial;
	material->diffuse.setValue ( 0.0f, 0.7f, 0.0f, 1.0f );
	material->side = SgMaterial::FRONT_AND_BACK;
	_root->addChild ( material );

	bool soccerBall = true;

	for ( int i = 0; i < 10; ++i )
	{
		SgSphere *sphere = new SgSphere;
		sphere->setCenter ( SlVec3f ( ((float) i) * 4.0f, 0.0f, -10.0f ) );
		sphere->setRadius ( 1.0f + ((float) i) * 0.1f );
		
		if ( soccerBall )
		{
			sphere->setTessellationType ( SgSphere::NO_POLES );
			sphere->setNumSubdivisions ( 1 );
		}

		else
		{
			sphere->setTessellationType ( SgSphere::POLES );
			sphere->setNumSlices ( 10 );
			sphere->setNumStacks ( 10 );
		}

		_root->addChild ( sphere );
	}

	material = new SgMaterial;
	material->diffuse.setValue ( 0.7f, 0.0f, 0.0f, 1.0f );
	_root->addChild ( material );

	float scale = 0.5;
	_root->addChild ( new SgScale ( scale, scale, scale ) );

	SgRotation *rot = new SgRotation;
	rot->axis.setValue ( 1.0f, 0.0f, 0.0f );
	rot->axis.normalize();
	rot->degrees = 45.0f;
	_root->addChild ( rot );
	_root->addChild ( rot );

	SgCube *cube = new SgCube;
	cube->size = 10.0f;
	cube->center.setValue ( 3.0f, 0.0f, -10.0f );
	_root->addChild ( cube );

}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.cx = cs.cy = 300;
	return SgMfcMdiChildWnd::PreCreateWindow ( cs );
}



/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	SgMfcMdiChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	SgMfcMdiChildWnd::Dump(dc);
}

#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
//
//	Called by MFC from OnCreate().
//
/////////////////////////////////////////////////////////////////////////////

BOOL CChildFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	SL_ASSERT ( this );
	SL_ASSERT ( _root );

	// Call this first.
	if ( !SgMfcMdiChildWnd::OnCreateClient ( lpcs, pContext ) ) 
    return FALSE;

	// Should be true.
	SL_ASSERT ( _view );
	SL_ASSERT ( _root );

  // Make the viewer.
  SgViewer::Ptr viewer = new SgViewer;

  // Set the view's viewer.
  _view->setViewer ( viewer );

  // Make the rendering context.
  SgGlRenderingContext::Ptr context = new SgGlWinRenderingContext;
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






/*
	//const SlType *cameraType = 0x0;
	const SlType *cameraType = SgPerspectiveCamera::getClassType();
	//const SlType *cameraType = SgOrthographicCamera::getClassType();

	// Now set up the viewer, renderer, and camera.
  if ( false == this->_init (	SgViewer::getClassType(), SgGlRenderer::getClassType(), SgGlWinRenderingContext::getClassType(), cameraType ) ) 
    return FALSE;

	// Set the background color.
	VERIFY ( _view->getViewer()->getRenderer()->setBackgroundColor ( SlColorf ( 1.0f, 1.0f, 1.0f, 1.0f ) ) );

	// Move the camera to see everything.
	//SL_VERIFY ( _view->viewAll() );
*/
	// Repaint.
	_view->Invalidate ( FALSE );

	// We cool.
	return TRUE;
}


void CChildFrame::OnFileSave() 
{
	// Should have these.
	SL_ASSERT ( _view );
	SL_ASSERT ( _view->getViewer() );
	SL_ASSERT ( _view->getViewer()->getRenderer() );
	SL_ASSERT ( _view->getViewer()->getRenderer()->getRefCount() > 0 );

	// Get a pointer to the current viewer.
	SgViewer *viewer = _view->getViewer();

	// Get a pointer to the current renderer and ref it.
	SgRenderer *originalRenderer = viewer->getRenderer();
	originalRenderer->ref();
/*
	// Declare the EPS renderer.
	SgEpsWrite *epsWrite = new SgEpsWrite;

	// Set its window.
	SL_VERIFY ( epsWrite->setWindow ( _view->GetSafeHwnd() ) );

	// Set some other data members.
	epsWrite->filename = "perry.eps";
	epsWrite->creator = "Perry Miller";
	epsWrite->setGouraudShadingThreshold ( 0.01 );
	epsWrite->setBackgroundColor ( originalRenderer->getBackgroundColor() );
	epsWrite->backFaces = false;

	// Set the viewer's renderer to be the EPS renderer.
	viewer->setRenderer ( epsWrite );
*/
	// Render the scene.
	SL_VERIFY ( viewer->render() );

	// Reset the original renderer. This will delete epsWrite.
	viewer->setRenderer ( originalRenderer );
}


void CChildFrame::OnOrthographicCamera() 
{
	SL_ASSERT ( this && _view );
	SgViewer *viewer = _view->getViewer();
	if ( viewer )
	{
		SgRenderer *renderer = viewer->getRenderer();
		if ( renderer )
		{
			SlViewporti viewport;
			if ( renderer->getViewport ( viewport ) )
			{
				SgOrthographicCamera *camera = new SgOrthographicCamera;
				if ( viewer->getCamera() ) 
				{
					camera->setValue ( *(viewer->getCamera()) );
				}
				viewer->setCamera ( camera );
				camera->setAspectRatio ( viewport.getWidth(), viewport.getHeight() );
				_view->Invalidate ( FALSE );
			}
		}
	}
}

void CChildFrame::OnUpdateOrthographicCamera(CCmdUI* pCmdUI) 
{
	SL_ASSERT ( this && _view );
	SgViewer *viewer = _view->getViewer();
	if ( viewer )
	{
		pCmdUI->SetRadio ( viewer->getCamera() && viewer->getCamera()->isOfType ( SgOrthographicCamera::getClassType() ) );
	}
	else
	{
		pCmdUI->Enable ( FALSE );
	}
}

void CChildFrame::OnPerspectiveCamera() 
{
	SL_ASSERT ( this && _view );
	SgViewer *viewer = _view->getViewer();
	if ( viewer )
	{
		SgRenderer *renderer = viewer->getRenderer();
		if ( renderer )
		{
			SlViewporti viewport;
			if ( renderer->getViewport ( viewport ) )
			{
				SgPerspectiveCamera *camera = new SgPerspectiveCamera;
				if ( viewer->getCamera() ) 
				{
					camera->setValue ( *(viewer->getCamera()) );
				}
				viewer->setCamera ( camera );
				camera->setAspectRatio ( viewport.getWidth(), viewport.getHeight() );
				_view->Invalidate ( FALSE );
			}
		}
	}
}

void CChildFrame::OnUpdatePerspectiveCamera(CCmdUI* pCmdUI) 
{
	SL_ASSERT ( this && _view );
	SgViewer *viewer = _view->getViewer();
	if ( viewer )
	{
		pCmdUI->SetRadio ( viewer->getCamera() && viewer->getCamera()->isOfType ( SgPerspectiveCamera::getClassType() ) );
	}
	else
	{
		pCmdUI->Enable ( FALSE );
	}
}

void CChildFrame::OnViewAll() 
{
	// Move the camera to see everything.
	SL_VERIFY ( _view->viewAll() );

	// Repaint.
	_view->Invalidate ( FALSE );
}
