
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for reading properties of a file. For example, if the file is an 
//  image, use this interface to query the width and height of the image, etc.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_READ_PROPERTIES_H_
#define _USUL_INTERFACE_READ_PROPERTIES_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {


struct IReadProperties : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IReadProperties );

  /// Id for this interface.
  enum { IID = 1268433335u };

  // Read the image properties.
  virtual void readProperties ( const std::string &filename ) = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_READ_PROPERTIES_H_
