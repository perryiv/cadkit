
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for exporting scene
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_EXPORT_H_
#define _USUL_INTERFACE_EXPORT_H_

#include "Usul/Interfaces/IUnknown.h"

#include <vector>

namespace Usul {
namespace Interfaces {


struct IExport : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IExport );

  /// Id for this interface.
  enum { IID = 4193063315u };

  typedef std::pair<std::string,std::string>    Filter;
  typedef std::vector<Filter>                   Filters;

  //// Write the current frame to an image file.
  virtual bool                  writeImageFile ( const std::string &filename ) const = 0;
  virtual bool                  writeImageFile ( const std::string &filename, unsigned int height, unsigned int width ) const = 0;

  //// Write the current scene to file.
  virtual Filters               filtersWriteImage() const = 0;

  virtual bool                  canExport ( const std::string &filename ) = 0;
  virtual Filters               filtersExport() const = 0;
  virtual bool                  exportFile ( const std::string& filename ) = 0;

}; // class IExport


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_EXPORT_H_
