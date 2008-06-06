
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Handles: Unique handles.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_HANDLES_H_
#define _CADKIT_INTERFACE_HANDLES_H_


// Macro for declaring a handle.
#define CADKIT_DECLARE_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ *name


namespace CadKit
{
// Common handles.
CADKIT_DECLARE_HANDLE ( AssemblyHandle );
CADKIT_DECLARE_HANDLE ( PartHandle );
CADKIT_DECLARE_HANDLE ( InstanceHandle );
CADKIT_DECLARE_HANDLE ( LodHandle );
CADKIT_DECLARE_HANDLE ( ShapeHandle );
CADKIT_DECLARE_HANDLE ( PrimHandle );
CADKIT_DECLARE_HANDLE ( SetHandle );
CADKIT_DECLARE_HANDLE ( GroupHandle );

}; // namespace CadKit

#endif // _CADKIT_INTERFACE_HANDLES_H_
