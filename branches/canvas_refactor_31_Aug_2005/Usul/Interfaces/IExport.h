
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

#include <string>
#include <vector>
#include <utility>


namespace Usul {
namespace Interfaces {


struct IExport : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IExport );

  /// Id for this interface.
  enum { IID = 4193063315u };

  /// Useful typedefs.
  typedef std::pair<std::string,std::string>  Filter;
  typedef std::vector<Filter>                 Filters;

  /// Write the current frame to an image file.
  virtual bool                  writeImageFile ( const std::string &name ) const = 0;

  /// Write the current scene to file.
  virtual bool                  writeSceneFile ( const std::string &name ) const = 0;

  /// Return appropriate file-selection filters.
  virtual Filters               filtersWriteScene() const = 0;
  virtual Filters               filtersWriteImage() const = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_EXPORT_H_
