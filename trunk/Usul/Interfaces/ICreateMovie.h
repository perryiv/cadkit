
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_CREATE_MOVIE_H__
#define __USUL_INTERFACES_CREATE_MOVIE_H__

#include "Usul/Interfaces/IUnknown.h"

#include <string>
#include <vector>

namespace Usul {
namespace Interfaces {


struct ICreateMovie : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ICreateMovie );

  /// Id for this interface.
  enum { IID = 1258410266u };

  typedef std::string Filename;
  typedef std::vector< Filename > Filenames;
  typedef std::pair < std::string, std::string > Filter;
  typedef std::vector<Filter> Filters;

  virtual Filters             getFilters  () = 0;
  virtual void                createMovie ( const Filename& filename, const Filenames& filenames ) = 0;

}; // struct ICreateMovie


}
}


#endif // __USUL_INTERFACES_CREATE_MOVIE_H__

