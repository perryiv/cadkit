
///////////////////////////////////////////////////////////////////////////////
//
//  BSD License
//  http://www.opensource.org/licenses/bsd-license.html
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
//
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
//  - Neither the name of the CAD Toolkit nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
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
