
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach & Perry Miller
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Factory class.
//
///////////////////////////////////////////////////////////////////////////////

// Disable deprecated warning in Visual Studio 8 for std::basic_istream<_Elem,_Traits>::read
#if defined ( _MSC_VER ) && _MSC_VER == 1400
#pragma warning ( disable : 4996 )
#endif

#include "Images/Core/Factory.h"
#include "Images/Core/ImageImpl.h"

#include "Usul/Types/Types.h"
#include "Usul/Exceptions/Thrower.h"

#include <stdexcept>
#include <sstream>

using namespace Images;


///////////////////////////////////////////////////////////////////////////////
//
//  Create the image.
//
///////////////////////////////////////////////////////////////////////////////

BaseImage *Factory::create ( unsigned int bytes, bool integer, Usul::Interfaces::IUnknown *pixels )
{
  if ( integer )
  {
    switch ( bytes )
    {
      case 1:
        return new Images::ImageImpl < Usul::Types::Uint8 > ( pixels );
      case 2:
        return new Images::ImageImpl < Usul::Types::Uint16 > ( pixels );
      case 4:
        return new Images::ImageImpl < Usul::Types::Uint32 > ( pixels );
      default:
        Usul::Exceptions::Thrower<std::runtime_error>  ( "Error 3759393049: ", bytes, " bytes is not supported for integer pixels" );
    }
  }

  else
  {
    switch ( bytes )
    {
      case 4:
        return new Images::ImageImpl < Usul::Types::Float32 > ( pixels );
      case 8:
        return new Images::ImageImpl < Usul::Types::Float64 > ( pixels );
      default:
        Usul::Exceptions::Thrower<std::runtime_error> ( "Error 1318051482: ", bytes, " bytes is not supported for floating-point pixels" );
    }
  }

  // Should never get here.
  return 0x0;
}
