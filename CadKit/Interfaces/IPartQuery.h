
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  IPartQuery: Interface for querying an part.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_PART_QUERY_H_
#define _CADKIT_INTERFACE_PART_QUERY_H_

#include "IUnknown.h"
#include "Handles.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <string>
#endif


namespace CadKit
{
template <const unsigned int id, class Real> class IPartQuery : public IUnknown
{
public:

  // Id for this interface.
  enum { IID = id };

  // Get the name.
  virtual std::string     getName ( PartHandle part ) const = 0;

  // Get the transformation matrix.
  virtual bool            getTransform ( PartHandle part, Real matrix[16] ) const = 0;
};


// Common types.
typedef IPartQuery<1032920060,float>  IPartQueryFloat;
typedef IPartQuery<1032920067,double> IPartQueryDouble;


}; // namespace CadKit

#endif // _CADKIT_INTERFACE_PART_QUERY_H_
