
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base Preferences class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Precompiled.h"
#include "Preferences.h"

using namespace VRV;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Preferences::Preferences() :
  _numGridBlocks ( 40, 40 ),
  _gridScale     ( 10, 10 ),
  _gridColor     ( 1, 1, 1, 1 ),
  _zNear         ( 0.1f ),
  _zScale        ( 2 ),
  _ambientLight  ( 0.2f, 0.2f, 0.2f, 1.0f ),
  _diffuseLight  ( 1.0f, 1.0f, 1.0f, 1.0f ),
  _specularLight ( 0.7f, 0.7f, 0.7f, 1.0f ),
  _lightDir      ( 0, 0, -1 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Preferences::~Preferences()
{
}