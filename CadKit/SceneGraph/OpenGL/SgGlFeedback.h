
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
//  SgGlFeedback: The feedback buffer renderer class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_OPENGL_FEEDBACK_RENDERER_H_
#define _CADKIT_SCENEGRAPH_OPENGL_FEEDBACK_RENDERER_H_

#include "SgGlRenderer.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <stdlib.h>
# include <GL/gl.h>
#endif


namespace CadKit
{
class SG_GL_API SgGlFeedback : public SgGlRenderer
{
public:

  enum // Possible flags.
  {
    PASS_THROUGH =    (1L<<0), // Call glPassTrough() for each node. Default is off.
    CULL_BACK_FACES = (1L<<1), // Cull out triangles with their backs facing the view. Default is off.
    // Keep in sync with internal flags below.
  };
  
  enum SortDirection
  {
    BACK_TO_FRONT,  // Sort from back to front, default.
    FRONT_TO_BACK,  // Sort from front to back.
    UNSORTED        // Write to file in order drawn.
  } sortDirection;

  enum SortOption
  {
    MIN_Z,       // Use the minimum z-value of the triangle when sorting, default.
    MAX_Z,       // Use the minimum z-value of the triangle when sorting.
    AVERAGE      // Use the average debth of the triangle's vertices.
  } sortOption;

  SgGlFeedback();

  // Call these before and after render(). You can call render() any 
  // number of times between preRender() and postRender().
  virtual bool          preRender ( SgNode &scene );
  virtual bool          postRender ( SgNode &scene );

  // Render the scene.
  virtual bool          render ( SgNode &scene );

  // The visit functions.
  SG_DECLARE_ALL_VISITOR_FUNCTIONS;

protected:

  struct GL3DColorFeedback
  {
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat r;
    GLfloat g;
    GLfloat b;
    GLfloat a;
  };

  struct Primitive
  {
    GLfloat *buffer;
    GLfloat depth;
  };

  enum // Internal flags.
  {
    _CLIENT_OWNS_BUFFER = (1L<<2),
  };

  Primitive *_primitive;
  GLint _numPrimitives;
  GLint _bufferSize;
  GLint _numValuesInBuffer;
  GLfloat *_buffer;
  unsigned long _flags;

  virtual ~SgGlFeedback();

  bool                  _calculateBufferSize ( SlInt32 &size, SgNode &scene ) const;
  void                  _calculatePrimitiveDebths();
  static int            _compareBackToFront ( const void *a, const void *b );
  static int            _compareFrontToBack ( const void *a, const void *b );
  bool                  _countPrimitives ( GLint &numPrimitives ) const;

  void                  _deleteBuffer();
  void                  _deletePrimitives();

  bool                  _processFeedbackBuffer();

  bool                  _reallocateBufferIfNeeded ( GLint &size );
  bool                  _reallocatePrimitivesIfNeeded ( GLint &numPrimitives );
  
  SG_DECLARE_DYNAMIC_VISITOR(SgGlFeedback,0x00005000);
  SL_DECLARE_BITMASK_FUNCTIONS(Flags,unsigned long,_flags);
};

}; // namespace CadKit

#endif // _CADKIT_SCENEGRAPH_OPENGL_FEEDBACK_RENDERER_H_
