
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for exporting scene.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_EXPORT_SCENE_H_
#define _USUL_INTERFACE_EXPORT_SCENE_H_

#include "Usul/Interfaces/IUnknown.h"

#include <vector>

namespace Usul {
namespace Interfaces {


struct IExportScene : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IExportScene );

  /// Id for this interface.
  enum { IID = 2464539226u };

  typedef std::pair<std::string,std::string>    Filter;
  typedef std::vector<Filter>                   Filters;

  /// Get filters for scene files
  virtual Filters               filtersWriteScene() const = 0;

  /// Write the current scene to file.
  virtual bool                  writeSceneFile ( const std::string &filename, const std::string &options = std::string() ) const = 0;

}; // class IExportScene


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_EXPORT_SCENE_H_
