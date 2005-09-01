
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for writing an image
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_WRITE_IMAGE_H_
#define _USUL_INTERFACE_WRITE_IMAGE_H_

#include "Usul/Interfaces/IUnknown.h"

#include <string>
#include <vector>
#include <utility>


namespace Usul {
namespace Interfaces {


struct IWriteImage : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IWriteImage );

  /// Id for this interface.
  enum { IID = 1133053793u };

  /// Useful typedefs.
  typedef std::pair<std::string,std::string>  Filter;
  typedef std::vector<Filter>                 Filters;

  /// Write the current frame to an image file.
  virtual bool                  writeImageFile ( const std::string &name ) const = 0;
  virtual bool                  writeImageFile ( const std::string &name, double percent ) const;
  virtual bool                  writeImageFile ( const std::string &name, unsigned int height, unsigned int width ) const;

  /// Return appropriate file-selection filters.
  virtual Filters               filtersWriteImage() const = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_WRITE_IMAGE_H_
