
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgMfcFrameWnd.h: The frame window for an SDI application.
//
/////////////////////////////////////////////////////////////////////////////

#include "SgMfcPrecompiled.h"
#include "SgMfcResource.h"    // Resource ID's
#include "SgMfcFrameWnd.h"
#include "SgMfcView.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace CadKit;

IMPLEMENT_DYNCREATE(SgMfcFrameWnd, CFrameWnd)

BEGIN_MESSAGE_MAP(SgMfcFrameWnd, CFrameWnd)
  //{{AFX_MSG_MAP(SgMfcFrameWnd)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

SgMfcFrameWnd::SgMfcFrameWnd() : CFrameWnd(), 
  _view ( NULL ), 
  _root ( new SgGroup )
{
  // We have to declare the scene root here in the constructor instead of 
  // in _init() so that clients can add nodes to the scene in the 
  // constructors of their frame classes that inherit from this class.
  SL_ASSERT ( _root.isValid() );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
/////////////////////////////////////////////////////////////////////////////

SgMfcFrameWnd::~SgMfcFrameWnd()
{
  // Don't delete _view, it's just a pointer to the view that MFC created.
  // Don't need to do anything with _root, it is a reference-pointer. It's 
  // destructor will unreference the pointer. If that is the last reference 
  // then the node will die. Since the node is a group, all its children 
  // will be unreferenced.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Called by MFC from OnCreate().
//
/////////////////////////////////////////////////////////////////////////////

BOOL SgMfcFrameWnd::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
  SL_ASSERT ( this );
  SL_ASSERT ( _root );

  // Get the view.
  _view = STATIC_DOWNCAST ( SgMfcView, CreateView ( pContext ) );

  // Check for an error.
  if ( !_view ) 
  {
    SL_ASSERT ( 0 );
    return FALSE;
  }

  // We cool.
  return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
//
//	Clean up here. If you wait for the destructor then the virtual tables
//  are no longer valid.
//
/////////////////////////////////////////////////////////////////////////////

void SgMfcFrameWnd::OnDestroy() 
{
  // This will release the viewer's pointer and unreference it.
  _root.setValue ( NULL );

  // Call the base class's function.
	CFrameWnd::OnDestroy();
}
