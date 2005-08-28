
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting and setting the PSF File
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACE_PSF_FILE_H__
#define __USUL_INTERFACE_PSF_FILE_H__

#include "Usul/Interfaces/IUnknown.h"

#include <string>

namespace Usul {
namespace Interfaces {

struct IPsfFile : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IPsfFile );

  /// Id for this interface.
  enum { IID = 1142677137u };

  virtual const std::string& psfFile() const = 0;
  virtual void psfFile( const std::string& file ) = 0;

}; // class IPsfFile


}; // namespace Interfaces
}; // namespace Usul


#endif // __USUL_INTERFACE_PSF_FILE_H__
