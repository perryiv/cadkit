// Cube.h : main header file for the CUBE application
//

#if !defined(AFX_CUBE_H__CEFB9BEE_BE20_4B32_A557_7F1CAFC0526F__INCLUDED_)
#define AFX_CUBE_H__CEFB9BEE_BE20_4B32_A557_7F1CAFC0526F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CCubeApp:
// See Cube.cpp for the implementation of this class
//

class CCubeApp : public CWinApp
{
public:
	CCubeApp();
	virtual ~CCubeApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCubeApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CCubeApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUBE_H__CEFB9BEE_BE20_4B32_A557_7F1CAFC0526F__INCLUDED_)
