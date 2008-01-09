
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACES_STRING_GRID_SET_H_
#define _USUL_INTERFACES_STRING_GRID_SET_H_

#include "Usul/Interfaces/IUnknown.h"

#include <vector>


namespace Usul {
namespace Interfaces {


struct IStringGridSet : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IStringGridSet );

  // Typedefs.
  typedef std::vector < std::string > StringRow;
  typedef std::vector < StringRow > StringGrid;

  // Id for this interface.
  enum { IID = 4171312076u };

  // Set the grid of strings.
  virtual void                setStringGrid ( const StringGrid &grid ) = 0;
};


} // End namespace Interfaces
} // End namespace Usul


#endif // _USUL_INTERFACES_STRING_GRID_SET_H_
