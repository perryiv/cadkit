
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for writing a scene
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_WRITE_SCENE_H_
#define _USUL_INTERFACE_WRITE_SCENE_H_

#include "Usul/Interfaces/IUnknown.h"

#include <string>
#include <vector>
#include <utility>


namespace Usul {
namespace Interfaces {


struct IWriteScene : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IWriteScene );

  /// Id for this interface.
  enum { IID = 1836499409u };

  /// Useful typedefs.
  typedef std::pair<std::string,std::string>  Filter;
  typedef std::vector<Filter>                 Filters;

  /// Write the current scene to file.
  virtual bool                  writeSceneFile ( const std::string &name ) const = 0;

  /// Return appropriate file-selection filters.
  virtual Filters               filtersWriteScene() const = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_WRITE_SCENE_H_
