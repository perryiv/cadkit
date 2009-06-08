
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Jeff Conner
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_ILOADCOLORFILE_H__
#define __USUL_INTERFACES_ILOADCOLORFILE_H__

#include "Usul/Interfaces/IUnknown.h"

#include <vector>

namespace Usul {
namespace Interfaces {


struct ILoadColorFile : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ILoadColorFile );

  typedef std::vector< float > HeaderInfo;

  /// Id for this interface.
  enum { IID = 1846529055u };

  // Load color information into a triangle set from an image file
  virtual void      loadColorFile( const std::string &filename, const HeaderInfo& header ) = 0;

}; //struct IVertices


}
}

#endif // __USUL_INTERFACES_ILOADCOLORFILE_H__
