
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

//////////////////////////////////////////////////////////////////////////
//
//  SgGlInternalMacros: Macros that are used throughout SgGl (g++ doesn't 
//  like macros in .cpp files).
//
//////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_OPENGL_FEEDBACK_INTERNAL_MACROS_H_
#define _CADKIT_SCENEGRAPH_OPENGL_FEEDBACK_INTERNAL_MACROS_H_

/////////////////////////////////////////////////////////////////////////////
//
//  Macro for implementing all the SgGlFeedback::visit() functions. All 
//  they do is call glPassTrough() and then call the base class's function.
//
/////////////////////////////////////////////////////////////////////////////

#define SG_GL_IMPLEMENT_FEEDBACK_VISITOR_FUNCTION(NodeName) \
bool SgGlFeedback::visit ( NodeName &node ) \
{ \
  if ( this->hasFlags ( PASS_THROUGH ) ) ::glPassThrough ( (GLfloat) ( (GLuint) this ) ); \
  return SgGlRenderer::visit ( node ); \
}


/////////////////////////////////////////////////////////////////////////////
//
//  Macro for implementing the SgGlGetFeedbackSize::visit() functions for 
//  nodes without vertices.
//
/////////////////////////////////////////////////////////////////////////////

#define SG_GL_IMPLEMENT_GET_FEEDBACK_VISITOR_FUNCTION_NO_VERTICES(NodeName) \
bool SgGlGetFeedbackSize::visit ( NodeName &node ) \
{ \
  if ( passThrough ) _size += 2; \
  return SL_TRUE; \
}


///////////////////////////////////////////////////////////////////////////////
//
//  Macro used when rendering a separator node.
//
///////////////////////////////////////////////////////////////////////////////

#define SG_GL_RENDER_SEPARATOR_SAVE_STATE \
  SgCoordinate3::Ptr coordinate3 ( NULL ); \
  SgCoordinate4::Ptr coordinate4 ( NULL ); \
  SgCoordinate::Ptr coordinate ( NULL ); \
  SgNormal::Ptr normal ( NULL ); \
\
  if ( separator.hasPushPopFlags ( SgSeparator::COORDINATES ) ) \
  { \
    coordinate3 = _coordinate3; \
    coordinate4 = _coordinate4; \
    coordinate = _coordinate; \
  } \
\
  if ( separator.hasPushPopFlags ( SgSeparator::NORMALS ) ) \
  { \
    normal = _normal; \
  } \
\
  if ( separator.hasPushPopFlags ( SgSeparator::PROJECTION ) ) \
  { \
    _state->pushProjectionMatrix(); \
  } \
\
  if ( separator.hasPushPopFlags ( SgSeparator::MODELVIEW ) ) \
  { \
    _state->pushModelviewMatrix(); \
  } \
\
  if ( separator.hasPushPopFlags ( SgSeparator::ATTRIBUTES ) ) \
  { \
    ::glPushAttrib ( GL_ALL_ATTRIB_BITS ); \
  } \
\
  if ( separator.hasPushPopFlags ( SgSeparator::NAMES ) ) \
  { \
    PUSH_NAME; \
  }


///////////////////////////////////////////////////////////////////////////////
//
//  Macro used when rendering a separator node.
//
///////////////////////////////////////////////////////////////////////////////

#define SG_GL_RENDER_SEPARATOR_RESTORE_STATE \
  if ( separator.hasPushPopFlags ( SgSeparator::NAMES ) ) \
  { \
    POP_NAME; \
  } \
\
  if ( separator.hasPushPopFlags ( SgSeparator::ATTRIBUTES ) ) \
  { \
    ::glPopAttrib(); \
    _state->clear(); \
  } \
\
  if ( separator.hasPushPopFlags ( SgSeparator::MODELVIEW ) ) \
  { \
    _state->popModelviewMatrix(); \
  } \
\
  if ( separator.hasPushPopFlags ( SgSeparator::PROJECTION ) ) \
  { \
    _state->popProjectionMatrix(); \
  } \
\
  if ( separator.hasPushPopFlags ( SgSeparator::COORDINATES ) ) \
  { \
    _coordinate3 = coordinate3; \
    _coordinate4 = coordinate4; \
    _coordinate = coordinate; \
  } \
\
  if ( separator.hasPushPopFlags ( SgSeparator::NORMALS ) ) \
  { \
    _normal = normal; \
  }


#endif // _CADKIT_SCENEGRAPH_OPENGL_FEEDBACK_INTERNAL_MACROS_H_
