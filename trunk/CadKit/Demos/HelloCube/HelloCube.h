// HelloCube.h : main header file for the TESTNEWSG application
//

#if !defined(AFX_TESTNEWSG_H__52AC5254_3392_11D4_98AA_0040054C86C7__INCLUDED_)
#define AFX_TESTNEWSG_H__52AC5254_3392_11D4_98AA_0040054C86C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'StdAfx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CHelloCubeApp:
// See HelloCube.cpp for the implementation of this class
//

class CHelloCubeApp : public CWinApp
{
public:
	CHelloCubeApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHelloCubeApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CHelloCubeApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTNEWSG_H__52AC5254_3392_11D4_98AA_0040054C86C7__INCLUDED_)
