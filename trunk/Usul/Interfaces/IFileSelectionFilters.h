
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting file-selection filters.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_FILE_SELECTION_FILTERS_H_
#define _USUL_INTERFACE_FILE_SELECTION_FILTERS_H_

#include "Usul/Interfaces/IUnknown.h"

#include <string>
#include <vector>


namespace Usul {
namespace Interfaces {


struct IFileSelectionFilters : public Usul::Interfaces::IUnknown
{
  typedef Usul::Interfaces::IUnknown          Unknown;
  typedef std::pair<std::string,std::string>  Filter;
  typedef std::vector<Filter>                 Filters;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IFileSelectionFilters );

  /// Id for this interface.
  enum { IID = 2606697626u };

  /// Get the filters.
  virtual Filters             filtersOpen()   const = 0;
  virtual Filters             filtersSave()   const = 0;
  virtual Filters             filtersInsert() const = 0;
  virtual Filters             filtersExport() const = 0;
};


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_FILE_SELECTION_FILTERS_H_
