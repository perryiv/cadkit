// HelloCubeDoc.cpp : implementation of the CHelloCubeDoc class
//

#include "stdafx.h"
#include "HelloCube.h"

#include "HelloCubeDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHelloCubeDoc

IMPLEMENT_DYNCREATE(CHelloCubeDoc, CDocument)

BEGIN_MESSAGE_MAP(CHelloCubeDoc, CDocument)
	//{{AFX_MSG_MAP(CHelloCubeDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHelloCubeDoc construction/destruction

CHelloCubeDoc::CHelloCubeDoc()
{
	// TODO: add one-time construction code here

}

CHelloCubeDoc::~CHelloCubeDoc()
{
}

BOOL CHelloCubeDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CHelloCubeDoc serialization

void CHelloCubeDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CHelloCubeDoc diagnostics

#ifdef _DEBUG
void CHelloCubeDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CHelloCubeDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG
