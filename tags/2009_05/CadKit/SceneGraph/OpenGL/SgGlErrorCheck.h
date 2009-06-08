
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SgGlErrorCheck: Checks for a GL error in the constructor and destructor.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_OPENGL_ERROR_CHECKING_CLASS_H_
#define _CADKIT_SCENEGRAPH_OPENGL_ERROR_CHECKING_CLASS_H_

#include "SgGlApi.h"


namespace CadKit
{
class SG_GL_API SgGlErrorCheck
{
public:

  SgGlErrorCheck();
  ~SgGlErrorCheck();

  static void checkError();
};

}; // namespace CadKit


///////////////////////////////////////////////////////////////////////////////
//
//  Convenient macro.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
# define SG_GL_ERROR_CHECKER CadKit::SgGlErrorCheck _temp_instance_of_SgGlErrorCheck
# define SG_GL_ERROR_CHECK_NOW CadKit::SgGlErrorCheck::checkError()
#else
# define SG_GL_ERROR_CHECKER
# define SG_GL_ERROR_CHECK_NOW
#endif


#endif // _CADKIT_SCENEGRAPH_OPENGL_ERROR_CHECKING_CLASS_H_
