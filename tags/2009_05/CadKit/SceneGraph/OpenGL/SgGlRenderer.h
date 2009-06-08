
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SgGlRenderer: The renderer class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_OPENGL_RENDERER_H_
#define _CADKIT_SCENEGRAPH_OPENGL_RENDERER_H_

#include "SgGlApi.h"
#include "SgGlState.h"
#include "SgGlContext.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlBitmask.h"
# include "SceneGraph/Core/SgScreenRenderer.h"
# include "SceneGraph/Core/SgCoordinate3.h"
# include "SceneGraph/Core/SgCoordinate4.h"
# include "SceneGraph/Core/SgNormal.h"
#endif
  

namespace CadKit
{
class SgCoordinate4;
class SgCoordinate3;
class SgNormal;

class SG_GL_API SgGlRenderer : public SgScreenRenderer
{
public:

  // Possible flags.
  enum
  {
    DOUBLE_BUFFER = (1L<<0), // Default is true.
  };

  SgGlRenderer();

  // Enable/disable the state. Use the above flags.
  bool                  disable ( const unsigned int &state );
  bool                  enable  ( const unsigned int &state );

  // Get the viewport.
  virtual bool          getViewport ( SlViewporti &viewport ) const;
  
  // Set the default OpenGL settings. The viewport is used to set a default 
  // projection matrix and the OpenGL viewport. Pass 0 to skip that part. 
  // Have to pass a valid rendering context though.
  virtual bool          init ( const SlViewporti &viewport, SgGlContext *context );

  // Check the state.
  bool                  isDisabled ( const unsigned int &state );
  bool                  isEnabled  ( const unsigned int &state );

  // Call these before and after render(). You can call render() any 
  // number of times between preRender() and postRender().
  virtual bool          preRender ( SgNode &scene );
  virtual bool          postRender ( SgNode &scene );

  // Render the scene.
  virtual bool          render ( SgNode &scene );

  // Set the background color.
  virtual bool          setBackgroundColor ( const SlColorf &color );

  // Set the clear depth. The depth has to be in the range [0,1].
  // The default is 1. Have to set the window first.
  bool                  setClearDepth ( const float &depth );

  // Set the viewport.
  virtual bool          setViewport ( const SlViewporti &viewport );

  // The visit functions.
  SG_DECLARE_ALL_VISITOR_FUNCTIONS;

protected:

  mutable SgGlContext::Ptr _context;
  SgGlState::Ptr _state;
  SlUint32 _flags;
  const void *_quadric;
  SlRefPtr<SgCoordinate3> _coordinate3;
  SlRefPtr<SgCoordinate4> _coordinate4;
  SlRefPtr<SgCoordinate> _coordinate;
  SlRefPtr<SgNormal> _normal;

  virtual ~SgGlRenderer();

  virtual bool          _makeContextCurrent() const;

  void                  _pushMatrices ( const SgSeparator &separator );
  void                  _popMatrices ( const SgSeparator &separator );

  void                  _setContext ( SgGlContext *context );

  virtual bool          _swapBuffers() const;

  SG_DECLARE_DYNAMIC_VISITOR(SgGlRenderer,0x00005002);
  SL_DECLARE_BITMASK_FUNCTIONS(Flags,SlUint32,_flags);
};

}; // namespace CadKit

#endif // _CADKIT_SCENEGRAPH_OPENGL_RENDERER_H_
