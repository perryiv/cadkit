
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  IAssemblyQuery: Interface for querying an assembly.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_ASSEMBLY_QUERY_H_
#define _CADKIT_INTERFACE_ASSEMBLY_QUERY_H_

#include "IUnknown.h"
#include "Handles.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <string>
#endif


namespace CadKit
{
template <const unsigned int id, class Real> class IAssemblyQuery : public IUnknown
{
public:

  // Id for this interface.
  enum { IID = id };

  // Get the name.
  virtual std::string     getName ( AssemblyHandle assembly ) const = 0;

  // Get the transformation matrix.
  virtual bool            getTransform ( AssemblyHandle assembly, Real matrix[16] ) const = 0;
};


// Common types.
typedef IAssemblyQuery<1032844642,float>  IAssemblyQueryFloat;
typedef IAssemblyQuery<1032844873,double> IAssemblyQueryDouble;


}; // namespace CadKit

#endif // _CADKIT_INTERFACE_ASSEMBLY_QUERY_H_
