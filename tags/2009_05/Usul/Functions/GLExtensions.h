
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_FUNCTIONS_GL_EXTENSIONS_H__
#define __USUL_FUNCTIONS_GL_EXTENSIONS_H__

#if defined(_WIN32)
  #define NOMINMAX
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
#elif __APPLE__
  #include <dlfcn.h>
#endif

namespace Usul
{
  namespace Functions
  {
    inline void* glExtensionFunctionPointer ( const char* name )
    {
#ifdef WIN32
     return static_cast<void*> ( wglGetProcAddress ( name ) );
#elif __linux
      TODO
#elif __APPLE__
      static void *handle ( ::dlopen ( (const char *) 0x0, RTLD_LAZY ) );
      return ::dlsym ( handle, name );
#else
      TODO
#endif
    }

    inline void* glExtensionFunctionPointer ( const char* name, const char* fallbackName )
    {
      void* ptr ( glExtensionFunctionPointer ( name ) );
      if ( 0x0 != ptr ) return ptr;
      return glExtensionFunctionPointer ( fallbackName );
    }
  }
}


#endif // __USUL_FUNCTIONS_GL_EXTENSIONS_H__
