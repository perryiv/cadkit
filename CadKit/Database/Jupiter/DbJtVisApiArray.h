
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
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

#include "Standard/SlAssert.h"
#include "Standard/SlPrint.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
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

  DbJtVisApiArray() : _p ( 0x0 ){}
  DbJtVisApiArray ( T *&p ) : _p ( p ){}
  ~DbJtVisApiArray();

  bool        isValid() const { return 0x0 != _p; }

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
  SL_PRINT2 ( "In DbJtVisApiArray::~DbJtVisApiArray(), _p = %X\n", _p );

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
        path += "\\lib\\Windows\\DMDataTk40.dll"; // TODO, what about version 5.0 ?

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
