
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////


#ifndef _USUL_HEADERS_OPEN_GL_H_
#define _USUL_HEADERS_OPEN_GL_H_


#ifdef __APPLE__
# include <OpenGL/gl.h>
#else
# ifdef _MSC_VER
#  ifndef NOMINMAX
#   define NOMINMAX
#  endif
#  include <windows.h>
# endif
# include <GL/gl.h>
#endif


#endif /*_USUL_HEADERS_OPEN_GL_H_*/
