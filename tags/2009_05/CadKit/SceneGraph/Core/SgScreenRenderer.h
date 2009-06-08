
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgScreenRenderer: The base screen renderer class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_BASE_SCREEN_RENDERER_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_BASE_SCREEN_RENDERER_H_

#include "SgRenderer.h"


namespace CadKit
{
class SG_API SgScreenRenderer : public SgRenderer
{
protected:

  SgScreenRenderer();
  virtual ~SgScreenRenderer();

  SG_DECLARE_VISITOR ( SgScreenRenderer, 0x00001028 );
};
};

#endif
