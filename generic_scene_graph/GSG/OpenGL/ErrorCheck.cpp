
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Checks for a GL error in the constructor and destructor.
//
///////////////////////////////////////////////////////////////////////////////

#include "GSG/OpenGL/Precompiled.h"
#include "GSG/OpenGL/GlApi.h"

#include "GSG/Core/Config.h"


namespace GSG {
namespace Detail {


///////////////////////////////////////////////////////////////////////////////
//
//  Check for OpenGL errors.
//
///////////////////////////////////////////////////////////////////////////////

void checkOpenGlErrorState()
{
  GLenum error = GSG::GL::getError();
  
  switch ( error )
  {
  case GL_NO_ERROR:

    // No error has been recorded. The value of this symbolic constant 
    // is guaranteed to be zero. 
    break;
  
  case GL_INVALID_ENUM:
  
    // An unacceptable value is specified for an enumerated argument. 
    // The offending function is ignored, having no side effect other 
    // than to set the error flag.
    ErrorChecker ( false );
    break;
  
  case GL_INVALID_VALUE:

    // A numeric argument is out of range. The offending function is ignored, 
    // having no side effect other than to set the error flag.
    ErrorChecker ( false );
    break;

  case GL_INVALID_OPERATION:
    
    // The specified operation is not allowed in the current state. 
    // The offending function is ignored, having no side effect other 
    // than to set the error flag.
    ErrorChecker ( false );
    break;

  case GL_STACK_OVERFLOW:
    
    // This function would cause a stack overflow. The offending function 
    // is ignored, having no side effect other than to set the error flag.
    ErrorChecker ( false );
    break;

  case GL_STACK_UNDERFLOW:
    
    // This function would cause a stack underflow. The offending function 
    // is ignored, having no side effect other than to set the error flag.  
    ErrorChecker ( false );
    break;

  case GL_OUT_OF_MEMORY:
    
    // There is not enough memory left to execute the function. The state 
    // of OpenGL is undefined, except for the state of the error flags, 
    // after this error is recorded. 
    ErrorChecker ( false );
    break;

  default:

    ErrorChecker ( false ); // What error is this?
    break;
  }
}


}; // namespace Detail
}; // namespace GSG
