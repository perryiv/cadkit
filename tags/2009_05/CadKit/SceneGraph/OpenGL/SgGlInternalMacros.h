
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
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
  if ( this->hasFlags ( PASS_THROUGH ) ) \
    ::glPassThrough ( (GLfloat) ( (GLuint) this ) ); \
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
  if ( passThrough ) \
    _size += 2; \
  return SL_TRUE; \
}


#endif // _CADKIT_SCENEGRAPH_OPENGL_FEEDBACK_INTERNAL_MACROS_H_
