
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  ILoadOptions: Interface for setting load options.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_LOAD_OPTIONS_H_
#define _CADKIT_INTERFACE_LOAD_OPTIONS_H_

#include "IUnknown.h"


namespace CadKit
{
class ILoadOptions : public IUnknown
{
public:

  // Id for this interface.
  enum { IID = 1032647722 };

  // Possible assembly load options (haven't played with this yet...)
  enum AssemblyLoadOption
  {
    INSTANCE_ASSEMBLY, // Load instances?
    EXPLODE_ASSEMBLY   // Explode instances into parts?
  };

  // Possible brep load options.
  enum BrepLoadOption
  {
    TESS_ONLY, // Tessellation only.
    BREP_ONLY, // Boundary-Representation data (NURBS curves, surfaces, etc).
    ALL
  };

  // Possible shape load options.
  enum ShapeLoadOption
  {
    ALL_LODS,
    HIGH_LOD
  };

  // Set the assembly load option.
  virtual void            setAssemblyLoadOption ( const AssemblyLoadOption &option ) = 0;

  // Set the brep load option.
  virtual void            setBrepLoadOption ( const BrepLoadOption &option ) = 0;

  // Set the shape load option.
  virtual void            setShapeLoadOption ( const ShapeLoadOption &option ) = 0;
};

}; // namespace CadKit

#endif // _CADKIT_INTERFACE_LOAD_OPTIONS_H_
