// CubeView.cpp : implementation of the CCubeView class
//

#include "stdafx.h"
#include "Cube.h"

#include "CubeDoc.h"
#include "CubeView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCubeView

IMPLEMENT_DYNCREATE(CCubeView, SgMfcView)

BEGIN_MESSAGE_MAP(CCubeView, SgMfcView)
	//{{AFX_MSG_MAP(CCubeView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, SgMfcView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, SgMfcView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, SgMfcView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCubeView construction/destruction

CCubeView::CCubeView()
{
	// TODO: add construction code here

}

CCubeView::~CCubeView()
{
}

BOOL CCubeView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return SgMfcView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CCubeView drawing

void CCubeView::OnDraw(CDC* pDC)
{
	CCubeDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
  // Call base class's function.
  SgMfcView::OnDraw ( pDC );
}

/////////////////////////////////////////////////////////////////////////////
// CCubeView printing

BOOL CCubeView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CCubeView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CCubeView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CCubeView diagnostics

#ifdef _DEBUG
void CCubeView::AssertValid() const
{
	SgMfcView::AssertValid();
}

void CCubeView::Dump(CDumpContext& dc) const
{
	SgMfcView::Dump(dc);
}

CCubeDoc* CCubeView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCubeDoc)));
	return (CCubeDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCubeView message handlers
