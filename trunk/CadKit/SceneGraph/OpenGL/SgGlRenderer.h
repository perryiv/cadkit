
///////////////////////////////////////////////////////////////////////////////
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

  // The flags for the enable/disable function.
  enum GraphicsState
  {
    AUTO_NORMAL = 0, // Default is on.
    DEPTH_TEST,      // Default is on.
    NORMALIZE,       // Default is on.
    _LAST,           // Don't use this one.
  };

  // Enable/disable the state. Use the above flags.
  bool                  disable ( const GraphicsState &state );
  bool                  enable  ( const GraphicsState &state );

  // Get the viewport.
  virtual bool          getViewport ( SlViewporti &viewport ) const;
  
  // Set the default OpenGL settings. The viewport is used to set a default 
  // projection matrix and the OpenGL viewport. Pass 0 to skip that part. 
  // Have to pass a valid rendering context though.
  virtual bool          init ( const SlViewporti &viewport, SgGlContext *context );

  // Check the state.
  bool                  isDisabled ( const GraphicsState &state );
  bool                  isEnabled  ( const GraphicsState &state );

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

  // Internal graphics states.
  enum
  {
    _BLEND = _LAST,
  };

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

  void                  _setContext ( SgGlContext *context );

  virtual bool          _swapBuffers() const;

  SG_DECLARE_DYNAMIC_VISITOR(SgGlRenderer,0x00005002);
  SL_DECLARE_BITMASK_FUNCTIONS(Flags,SlUint32,_flags);
};

}; // namespace CadKit

#endif // _CADKIT_SCENEGRAPH_OPENGL_RENDERER_H_
