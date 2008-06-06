
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
//  SgGlStateMap: Keeps track of the OpenGL state.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_OPENGL_STATE_MAP_H_
#define _CADKIT_SCENEGRAPH_OPENGL_STATE_MAP_H_

#include "SgGlApi.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlRefBase.h"
# include <stdlib.h>
# include <GL/gl.h>
# include <map>
#endif
  

namespace CadKit
{
class SG_GL_API SgGlStateMap : public SlRefBase
{
public:

  SgGlStateMap();

  // Clear the state. This will make it ask OpenGL again.
  void                  clear();

  // Enable/disable the state-flag.
  bool                  enable     ( const GLenum &flag );
  bool                  disable    ( const GLenum &flag );
  bool                  isEnabled  ( const GLenum &flag );
  bool                  isDisabled ( const GLenum &flag );

protected:

  typedef std::map<GLenum,bool> StateMap;
  StateMap _stateMap;

  virtual ~SgGlStateMap();

  SL_DECLARE_REFERENCE_POINTER ( SgGlStateMap );
  SL_DECLARE_DYNAMIC_CLASS ( SgGlStateMap, 1032912103 );
};

}; // namespace CadKit

#endif // _CADKIT_SCENEGRAPH_OPENGL_STATE_MAP_H_
