
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting and setting the Param File
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACE_PARAM_FILE_H__
#define __USUL_INTERFACE_PARAM_FILE_H__

#include "Usul/Interfaces/IUnknown.h"

#include <string>

namespace Usul {
namespace Interfaces {

struct IParamFile : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IParamFile );

  /// Id for this interface.
  enum { IID = 2140045974u };

  virtual const std::string& paramFile() const = 0;
  virtual void paramFile( const std::string& file ) = 0;

}; // class IParamFile


}; // namespace Interfaces
}; // namespace Usul


#endif // __USUL_INTERFACE_PARAM_FILE_H__
