
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
//  DbJtVisApiArray: Special array class to handle deleting of the arrays 
//  that are allocated inside the VisApi DLL (on their own heap).
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_JUPITER_LIBRARY_INTERNAL_VIS_API_ARRAY_CLASS_H_
#define _CADKIT_DATABASE_JUPITER_LIBRARY_INTERNAL_VIS_API_ARRAY_CLASS_H_

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlAssert.h"
# include "Standard/SlPrint.h"
# ifdef _WIN32
#  ifdef _DEBUG
#   include <string>
#  endif
# endif
#endif


namespace CadKit
{
template <class T> class DbJtVisApiArray
{
public:

  DbJtVisApiArray() : _p ( NULL ){}
  DbJtVisApiArray ( T *&p ) : _p ( p ){}
  ~DbJtVisApiArray();

  bool        isValid() const { return NULL != _p; }

  const T *&  getReference() const { return _p; }
  T *&        getReference()       { return _p; }

  const T &   operator[] ( int i ) const { SL_ASSERT ( _p ); return _p[i]; }

protected:

  T *_p;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

template <class T> inline DbJtVisApiArray<T>::~DbJtVisApiArray()
{
  SL_PRINT ( "In DbJtVisApiArray::~DbJtVisApiArray(), _p = %X\n", _p );

  if ( _p )
  {
    #ifdef _WIN32
    
      #ifdef _DEBUG

      // The purpose of this section is to somehow delete the pointer
      // without crashing (it was allocated on DMDTk's heap because DMDTk 
      // is statically linked to the c-runtime library).

      // Get the path to the DLL.
      std::string path ( ::getenv ( "DMDTK_DEV_PATH" ) );

      // If we got a path...
      if ( false == path.empty() )
      {
        // Make the full path.
        path += "\\lib\\Windows\\DMDataTk40.dll";

        // Lose any double-slashes from the append above.
        //path.replace ( "\\\\", "\\" );

        // Get the handle to the DRDTk DLL.
        HMODULE module = ::GetModuleHandle ( path.c_str() );

        // If we got a module...
        if ( module )
        {
          // Get the address of "free" function in the DRDTk DLL.
          typedef void FreeFunc ( void * );
          FreeFunc *func = (FreeFunc *) ::GetProcAddress ( module, "GlobalFree" );

          // If we found the function...
          if ( func )
          {
            // Call the function.
            func ( _p );
          }
        }
      }

      #else // release

        delete [] _p;

      #endif

    #else // unix

      delete [] _p;

    #endif
  }
}


}; // namespace CadKit

#endif // _CADKIT_DATABASE_JUPITER_LIBRARY_INTERNAL_VIS_API_ARRAY_CLASS_H_
