// ChildFrm.h : interface of the CChildFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHILDFRM_H__52AC525A_3392_11D4_98AA_0040054C86C7__INCLUDED_)
#define AFX_CHILDFRM_H__52AC525A_3392_11D4_98AA_0040054C86C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CChildFrame : public CadKit::SgMfcMdiChildWnd
{
	DECLARE_DYNCREATE(CChildFrame)
public:
	CChildFrame();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CChildFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(CChildFrame)
	afx_msg void OnOrthographicCamera();
	afx_msg void OnUpdateOrthographicCamera(CCmdUI* pCmdUI);
	afx_msg void OnPerspectiveCamera();
	afx_msg void OnUpdatePerspectiveCamera(CCmdUI* pCmdUI);
	afx_msg void OnFileSave();
	afx_msg void OnViewAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDFRM_H__52AC525A_3392_11D4_98AA_0040054C86C7__INCLUDED_)
