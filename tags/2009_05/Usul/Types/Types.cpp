
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Basic types.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Types/Types.h"
#include "Usul/MPL/StaticAssert.h"


///////////////////////////////////////////////////////////////////////////////
//
//  This function is here to test the sizes of the types.
//
///////////////////////////////////////////////////////////////////////////////

void _testSizes()
{
  // If these do not compile, then it's a good indication that Types.h needs 
  // to be changed for this platform.
  USUL_STATIC_ASSERT ( 8 == sizeof ( Usul::Types::Uint64  ) );
  USUL_STATIC_ASSERT ( 4 == sizeof ( Usul::Types::Uint32  ) );
  USUL_STATIC_ASSERT ( 2 == sizeof ( Usul::Types::Uint16  ) );
  USUL_STATIC_ASSERT ( 8 == sizeof ( Usul::Types::Int64   ) );
  USUL_STATIC_ASSERT ( 4 == sizeof ( Usul::Types::Int32   ) );
  USUL_STATIC_ASSERT ( 2 == sizeof ( Usul::Types::Int16   ) );
  USUL_STATIC_ASSERT ( 8 == sizeof ( Usul::Types::Float64 ) );
  USUL_STATIC_ASSERT ( 4 == sizeof ( Usul::Types::Float32 ) );
};
