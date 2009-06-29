
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for logging string message
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_ILOG_H__
#define __USUL_INTERFACES_ILOG_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct ILog : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ILog );
  
  /// Id for this interface.
  enum { IID = 3478825104u };
  
  // Write the string.
  virtual void write ( const std::string &s, bool appendNewLine = true, bool prependEventCount = true ) = 0;
  
}; // struct IBusyState


} // End namespace Interfaces
} // End namespace Usul


#endif // __USUL_INTERFACES_ILOG_H__
