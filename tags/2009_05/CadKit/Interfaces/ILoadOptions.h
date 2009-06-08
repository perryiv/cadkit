
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

  // Possible assembly load options.
  enum AssemblyLoadOption
  {
    INSTANCE_ASSEMBLY, // Instances that are assemblies get traversed as instances.
    EXPLODE_ASSEMBLY   // Instances that are assemblies get traversed as assemblies.
  };

  // Possible part load options.
  enum PartLoadOption
  {
    INSTANCE_PART, // Instances that are parts get traversed as instances.
    EXPLODE_PART   // Instances that are parts get traversed as parts.
  };

  // Possible brep load options.
  enum BrepLoadOption
  {
    TESS_ONLY, // Tessellation only.
    BREP_ONLY, // Boundary-Representation data (NURBS curves, surfaces, etc).
    ALL
  };

  // Possible shape load options. (Depreciated in favor of ILodOption.)
  enum ShapeLoadOption
  {
    ALL_LODS,
    HIGH_LOD
  };

  // Set the assembly load option. Returns false if not supported.
  virtual bool            setAssemblyLoadOption ( const AssemblyLoadOption &option ) = 0;

  // Set the brep load option. Returns false if not supported.
  virtual bool            setBrepLoadOption ( const BrepLoadOption &option ) = 0;

  // Set the part load option. Returns false if not supported.
  virtual bool            setPartLoadOption ( const PartLoadOption &option ) = 0;

  // Set the shape load option. Returns false if not supported.
  virtual bool            setShapeLoadOption ( const ShapeLoadOption &option ) = 0;
};

}; // namespace CadKit

#endif // _CADKIT_INTERFACE_LOAD_OPTIONS_H_
