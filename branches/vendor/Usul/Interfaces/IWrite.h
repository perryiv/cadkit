
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for writing to a target.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_WRITE_H_
#define _USUL_INTERFACE_WRITE_H_

#include "Usul/Interfaces/IUnknown.h"

#include <string>


namespace Usul {
namespace Interfaces {


struct IWrite : public Usul::Interfaces::IUnknown
{
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IWrite );

  /// Id for this interface.
  enum { IID = 1177035122 };

  /// Write the data to the named target.
  virtual void write ( const std::string &target, Unknown *data, Unknown *caller ) = 0;
};


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_WRITE_H_
