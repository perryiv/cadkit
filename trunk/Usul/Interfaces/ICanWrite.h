
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for seeing if files with the extenson can be written.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_CAN_WRITE_H_
#define _USUL_INTERFACE_CAN_WRITE_H_

#include "Usul/Interfaces/IUnknown.h"

#include <string>


namespace Usul {
namespace Interfaces {


struct ICanWrite : public Usul::Interfaces::IUnknown
{
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ICanWrite );

  /// Id for this interface.
  enum { IID = 3727325395u };

  /// See if the extension is supported. The string can be just the 
  /// extension or the whole filename.
  virtual bool canWrite ( const std::string &name ) = 0;
};


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_CAN_WRITE_H_
