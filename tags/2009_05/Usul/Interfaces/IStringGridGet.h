
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACES_STRING_GRID_GET_H_
#define _USUL_INTERFACES_STRING_GRID_GET_H_

#include "Usul/Interfaces/IUnknown.h"

#include <vector>


namespace Usul {
namespace Interfaces {


struct IStringGridGet : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IStringGridGet );

  // Typedefs.
  typedef std::vector < std::string > StringRow;
  typedef std::vector < StringRow > StringGrid;

  // Id for this interface.
  enum { IID = 2102208475u };

  // Get the grid of strings.
  virtual void                getStringGrid ( StringGrid &grid ) const = 0;
};


} // End namespace Interfaces
} // End namespace Usul


#endif // _USUL_INTERFACES_STRING_GRID_GET_H_
