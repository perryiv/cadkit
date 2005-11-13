// HelloCubeView.h : interface of the CHelloCubeView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTNEWSGVIEW_H__52AC525E_3392_11D4_98AA_0040054C86C7__INCLUDED_)
#define AFX_TESTNEWSGVIEW_H__52AC525E_3392_11D4_98AA_0040054C86C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CHelloCubeView : public CadKit::SgMfcView
{
protected: // create from serialization only
	CHelloCubeView();
	DECLARE_DYNCREATE(CHelloCubeView)

// Attributes
public:
	CHelloCubeDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHelloCubeView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHelloCubeView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CHelloCubeView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in HelloCubeView.cpp
inline CHelloCubeDoc* CHelloCubeView::GetDocument()
   { return (CHelloCubeDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTNEWSGVIEW_H__52AC525E_3392_11D4_98AA_0040054C86C7__INCLUDED_)
