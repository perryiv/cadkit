
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_GET_OPTIONS_H_
#define _USUL_INTERFACE_GET_OPTIONS_H_

#include "Usul/Interfaces/IUnknown.h"

#include <string>
#include <map>

namespace Usul {
namespace Interfaces {


struct IGetOptions : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IGetOptions );

  /// Id for this interface.
  enum { IID = 3488750597u };

  typedef std::map<std::string,std::string>     Options;

  virtual void   getOptions ( Options& ) = 0;


}; // class IGetOptions


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_GET_OPTIONS_H_
