
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
//  Interface for exporting model.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_EXPORT_MODEL_H_
#define _USUL_INTERFACE_EXPORT_MODEL_H_

#include "Usul/Interfaces/IUnknown.h"

#include <vector>

namespace Usul {
namespace Interfaces {


struct IExportModel : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IExportModel );

  /// Id for this interface.
  enum { IID = 1361056932u };

  typedef std::pair<std::string,std::string>    Filter;
  typedef std::vector<Filter>                   Filters;

  /// Get filters for model files
  virtual Filters               filtersWriteModel() const = 0;

  /// Write the current model to file.
  virtual bool                  writeModelFile ( const std::string &filename, const std::string &options = std::string() ) const = 0;

}; // class IExportModel


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_EXPORT_MODEL_H_
