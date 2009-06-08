// CubeDoc.h : interface of the CCubeDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CUBEDOC_H__F7215864_8496_472E_90F0_C7471BAE59A8__INCLUDED_)
#define AFX_CUBEDOC_H__F7215864_8496_472E_90F0_C7471BAE59A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CCubeDoc : public CDocument
{
protected: // create from serialization only
	CCubeDoc();
	DECLARE_DYNCREATE(CCubeDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCubeDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCubeDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CCubeDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUBEDOC_H__F7215864_8496_472E_90F0_C7471BAE59A8__INCLUDED_)
