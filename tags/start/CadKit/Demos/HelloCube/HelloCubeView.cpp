// HelloCubeView.cpp : implementation of the CHelloCubeView class
//

#include "stdafx.h"
#include "HelloCube.h"

#include "HelloCubeDoc.h"
#include "HelloCubeView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace CadKit;

/////////////////////////////////////////////////////////////////////////////
// CHelloCubeView

IMPLEMENT_DYNCREATE(CHelloCubeView, SgMfcView)

BEGIN_MESSAGE_MAP(CHelloCubeView, SgMfcView)
	//{{AFX_MSG_MAP(CHelloCubeView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, SgMfcView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, SgMfcView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, SgMfcView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHelloCubeView construction/destruction

CHelloCubeView::CHelloCubeView()
{
	// TODO: add construction code here

}

CHelloCubeView::~CHelloCubeView()
{
}

BOOL CHelloCubeView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return SgMfcView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CHelloCubeView drawing

void CHelloCubeView::OnDraw(CDC* pDC)
{
	CHelloCubeDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	SgMfcView::OnDraw ( pDC );
}

/////////////////////////////////////////////////////////////////////////////
// CHelloCubeView printing

BOOL CHelloCubeView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CHelloCubeView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CHelloCubeView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CHelloCubeView diagnostics

#ifdef _DEBUG
void CHelloCubeView::AssertValid() const
{
	SgMfcView::AssertValid();
}

void CHelloCubeView::Dump(CDumpContext& dc) const
{
	SgMfcView::Dump(dc);
}

CHelloCubeDoc* CHelloCubeView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CHelloCubeDoc)));
	return (CHelloCubeDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CHelloCubeView message handlers
