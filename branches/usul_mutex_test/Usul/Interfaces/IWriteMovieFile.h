
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IWRITEMOVIEFILE_H__
#define __USUL_INTERFACES_IWRITEMOVIEFILE_H__

#include "Usul/Interfaces/IUnknown.h"

#include <vector>
#include <string>

namespace Usul {
namespace Interfaces {


struct IWriteMovieFile : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IWriteMovieFile );

  /// Id for this interface.
  enum { IID = 1139119008u };

  typedef std::string                           Filename;
  typedef std::vector< Filename >               Filenames;
  typedef std::pair<std::string,std::string>    Filter;
  typedef std::vector<Filter>                   Filters;

  virtual Filters             filtersWrite() const = 0;

  virtual bool                canWrite   ( const std::string &file ) const = 0;

  virtual void                writeMovie ( const Filename& filename, const Filenames& filenames, Usul::Interfaces::IUnknown *caller = 0x0 ) = 0;

}; // struct IWriteMovieFile


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_IWRITEMOVIEFILE_H__ */
