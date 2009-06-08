
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SgRenderer: The base renderer class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_BASE_RENDERER_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_BASE_RENDERER_H_

#include "SgVisitor.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlColor.h"
# include "Standard/SlViewport.h"
#endif


namespace CadKit
{
class SG_API SgRenderer : public SgVisitor
{
public:

  // Get the viewport.
  virtual bool          getViewport ( SlViewporti &viewport ) const = 0;

  // Call these before and after render(). You can call render() any 
  // number of times between preRender() and postRender().
  virtual bool          preRender ( SgNode &scene ) = 0;
  virtual bool          postRender ( SgNode &scene ) = 0;

  // Render the scene.
  virtual bool          render ( SgNode &scene ) = 0;

  // Set/get the background color.
  virtual bool          setBackgroundColor ( const SlColorf &color ) = 0;
  const SlColorf &      getBackgroundColor() const { return _backgroundColor; }

  // Set the viewport.
  virtual bool          setViewport ( const SlViewporti &viewport ) = 0;

protected:

  SlColorf _backgroundColor;

  SgRenderer();
  virtual ~SgRenderer();

  SG_DECLARE_VISITOR ( SgRenderer, 0x00001025 );
};

}; // namespace CadKit

#endif // _CADKIT_SCENEGRAPH_CORE_LIBRARY_BASE_RENDERER_H_
