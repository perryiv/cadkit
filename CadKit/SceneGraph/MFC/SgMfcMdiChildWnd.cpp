
/////////////////////////////////////////////////////////////////////////////////
//
//  BSD License
//  http://www.opensource.org/licenses/bsd-license.html
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
//
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
//  - Neither the name of the CAD Toolkit nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
/////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
// SgMfcMdiChildWnd.h: The frame window for an MDI application.
//
///////////////////////////////////////////////////////////////////////////////

#include "SgMfcPrecompiled.h"
#include "SgMfcResource.h"
#include "SgMfcMdiChildWnd.h"
#include "SgMfcView.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace CadKit;

IMPLEMENT_DYNCREATE(SgMfcMdiChildWnd, CMDIChildWnd)

BEGIN_MESSAGE_MAP(SgMfcMdiChildWnd, CMDIChildWnd)
  //{{AFX_MSG_MAP(SgMfcMdiChildWnd)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SgMfcMdiChildWnd::SgMfcMdiChildWnd() : CMDIChildWnd(), 
  _view ( NULL ), 
  _root ( new SgGroup )
{
  // We have to declare the scene root here in the constructor instead of 
  // in _init() so that clients can add nodes to the scene in the 
  // constructors of their frame classes that inherit from this class.
  SL_ASSERT ( _root.isValid() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SgMfcMdiChildWnd::~SgMfcMdiChildWnd()
{
  // Don't delete _view, it's just a pointer to the view that MFC created.
  // Don't need to do anything with _root, it is a reference-pointer. It's 
  // destructor will unreference the pointer. If that is the last reference 
  // then the node will die. Since the node is a group, all it it's children 
  // will be unreferenced.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called by MFC from OnCreate().
//
///////////////////////////////////////////////////////////////////////////////

BOOL SgMfcMdiChildWnd::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
  SL_ASSERT ( this );
  SL_ASSERT ( _root );

  // Get the view.
  _view = STATIC_DOWNCAST ( SgMfcView, CreateView ( pContext ) );

  // Check for an error.
  if ( NULL == _view ) 
  {
    SL_ASSERT ( 0 );
    return FALSE;
  }

  // We cool.
  return TRUE;
}
