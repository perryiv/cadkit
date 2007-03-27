
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgOffScreenRenderer: The base off-screen renderer class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_BASE_OFF_SCREEN_RENDERER_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_BASE_OFF_SCREEN_RENDERER_H_

#include "SgRenderer.h"


namespace CadKit
{
class SG_API SgOffScreenRenderer : public SgRenderer
{
protected:

  SgOffScreenRenderer();
  virtual ~SgOffScreenRenderer();

  SG_DECLARE_VISITOR ( SgOffScreenRenderer, 0x0000101E );
};
};

#endif
