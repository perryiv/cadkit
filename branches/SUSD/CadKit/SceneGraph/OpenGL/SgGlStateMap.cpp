
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
//  SgGlStateMap: Keeps track of the OpenGL state.
//
///////////////////////////////////////////////////////////////////////////////

#include "SgGlPrecompiled.h"
#include "SgGlStateMap.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlPrint.h"
# include "Standard/SlAssert.h"
#endif

using namespace CadKit;

SL_IMPLEMENT_DYNAMIC_CLASS(SgGlStateMap,SlRefBase);


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SgGlStateMap::SgGlStateMap() : SlRefBase ( 0 )
{
  SL_PRINT2 ( "SgGlStateMap::SgGlStateMap(), this = %X\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SgGlStateMap::~SgGlStateMap()
{
  SL_PRINT2 ( "SgGlStateMap::~SgGlStateMap(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Enable the state-flag.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGlStateMap::enable  ( const GLenum &flag )
{
  SL_ASSERT ( this );

  // See if this state-flags is already enabled.
  if ( this->isEnabled ( flag ) )
    return true;

  // Should be true.
  SL_ASSERT ( GL_NO_ERROR == ::glGetError() );

  // Turn on the state.
  ::glEnable ( flag );

  // Save the state.
  bool state = ( GL_TRUE == ::glIsEnabled ( flag ) );
  _stateMap[flag] = state;

  // Should still be true.
  SL_ASSERT ( GL_NO_ERROR == ::glGetError() );

  // It worked.
  return true;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Disable the state-flag.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGlStateMap::disable  ( const GLenum &flag )
{
  SL_ASSERT ( this );

  // See if this state-flags is already disabled.
  if ( this->isDisabled ( flag ) )
    return true;

  // Should be true.
  SL_ASSERT ( GL_NO_ERROR == ::glGetError() );

  // Turn on the state.
  ::glDisable ( flag );

  // Save the state.
  bool state = ( GL_FALSE == ::glIsEnabled ( flag ) );
  _stateMap[flag] = state;

  // Should still be true.
  SL_ASSERT ( GL_NO_ERROR == ::glGetError() );

  // It worked.
  return true;
}


/////////////////////////////////////////////////////////////////////////////
//
//  See if the flag is enabled.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGlStateMap::isEnabled  ( const GLenum &flag )
{
  SL_ASSERT ( this );

  // Look for the flag.
  StateMap::iterator i = _stateMap.find ( flag );

  // have to declare up here.
  bool state ( false );

  // If we didn't find it...
  if ( _stateMap.end() == i )
  {
    // Ask OpenGL.
    state = ( GL_TRUE == ::glIsEnabled ( flag ) );

    // Save the state.
    _stateMap[flag] = state;

    // Return the state.
    return state;
  }

  // Sanity check.
  SL_ASSERT ( sizeof ( bool ) == sizeof ( i->second ) );

  // Otherwise, return what we found.
  state = i->second;
  return state;
}


/////////////////////////////////////////////////////////////////////////////
//
//  See if the flag is disabled.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGlStateMap::isDisabled  ( const GLenum &flag )
{
  SL_ASSERT ( this );

  // Call the other one.
  return ( false == this->isEnabled ( flag ) );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Clear the state. This will make it ask OpenGL again to refill the map.
//
/////////////////////////////////////////////////////////////////////////////

void SgGlStateMap::clear()
{
  SL_ASSERT ( this );
  _stateMap.clear();
}
