// HelloCubeDoc.h : interface of the CHelloCubeDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTNEWSGDOC_H__52AC525C_3392_11D4_98AA_0040054C86C7__INCLUDED_)
#define AFX_TESTNEWSGDOC_H__52AC525C_3392_11D4_98AA_0040054C86C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CHelloCubeDoc : public CDocument
{
protected: // create from serialization only
	CHelloCubeDoc();
	DECLARE_DYNCREATE(CHelloCubeDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHelloCubeDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHelloCubeDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CHelloCubeDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTNEWSGDOC_H__52AC525C_3392_11D4_98AA_0040054C86C7__INCLUDED_)
