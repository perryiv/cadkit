
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
//  that are allocated inside the VisApi DLL (on its own heap).
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

#ifdef _CADKIT_USE_JTOPEN
#include "JtTk/JtkEntityFactory.h"
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
#ifdef _CADKIT_USE_JTOPEN
    JtkEntityFactory::deleteMemory ( _p );
#else
    delete [] _p;
#endif
  }
}


}; // namespace CadKit

#endif // _CADKIT_DATABASE_JUPITER_LIBRARY_INTERNAL_VIS_API_ARRAY_CLASS_H_
