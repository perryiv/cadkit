
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
//  SgGlState: Keeps track of the OpenGL state.
//
///////////////////////////////////////////////////////////////////////////////

#include "SgGlPrecompiled.h"
#include "SgGlState.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlPrint.h"
# include "Standard/SlAssert.h"
#endif

using namespace CadKit;

SL_IMPLEMENT_DYNAMIC_CLASS(SgGlState,SlRefBase);


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SgGlState::SgGlState() : SlRefBase ( 0 ),
  _matrixMode ( MODELVIEW )
{
  SL_PRINT2 ( "SgGlState::SgGlState(), this = %X\n", this );

  // Initialize the stacks.
  this->initModelviewMatrixStack();
  this->initProjectionMatrixStack();
  this->initNameStack();

  StateMap top;
  _stateStack.push ( top );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SgGlState::~SgGlState()
{
  SL_PRINT2 ( "SgGlState::~SgGlState(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Enable the state-flag.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGlState::enable  ( const GLenum &flag )
{
  SL_ASSERT ( this );
  SL_ASSERT ( GL_NO_ERROR == ::glGetError() );

  // See if this state-flag is already enabled.
  if ( _ENABLED == this->_getState ( flag ) )
    return true;

  // Turn on the state.
  ::glEnable ( flag );
  SL_ASSERT ( GL_NO_ERROR == ::glGetError() );

  // Should be turned on.
  SL_ASSERT ( GL_TRUE == ::glIsEnabled ( flag ) );
  SL_ASSERT ( GL_NO_ERROR == ::glGetError() );

  // Save the state.
  _getCurrentStateMap()[flag] = true;

  // It worked.
  return true;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Disable the state-flag.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGlState::disable  ( const GLenum &flag )
{
  SL_ASSERT ( this );
  SL_ASSERT ( GL_NO_ERROR == ::glGetError() );

  // See if this state-flag is already enabled.
  if ( _DISABLED == this->_getState ( flag ) )
    return true;

  // Turn on the state.
  ::glDisable ( flag );
  SL_ASSERT ( GL_NO_ERROR == ::glGetError() );

  // Should be turned on.
  SL_ASSERT ( GL_FALSE == ::glIsEnabled ( flag ) );
  SL_ASSERT ( GL_NO_ERROR == ::glGetError() );

  // Save the state.
  _getCurrentStateMap()[flag] = true;

  // It worked.
  return true;
}


/////////////////////////////////////////////////////////////////////////////
//
//  See if the flag is enabled.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGlState::isEnabled  ( const GLenum &flag )
{
  SL_ASSERT ( this );
  SL_ASSERT ( GL_NO_ERROR == ::glGetError() );

  // Ask OpenGL.
  bool result = ( GL_TRUE == ::glIsEnabled ( flag ) );
  SL_ASSERT ( GL_NO_ERROR == ::glGetError() );

  // Return the result.
  return result;
}


/////////////////////////////////////////////////////////////////////////////
//
//  See if the flag is disabled.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGlState::isDisabled  ( const GLenum &flag )
{
  SL_ASSERT ( this );

  // Call the other one.
  return ( false == this->isEnabled ( flag ) );
}


//Was gutting this class to just be a thin wrapper. 
//Someday when I have time I can look into making it more efficient. 
//For now, just get some pictures on the screen.




/////////////////////////////////////////////////////////////////////////////
//
//  Get the state from the map (don't ask OpenGL).
//
/////////////////////////////////////////////////////////////////////////////

SgGlState::State SgGlState::_getState ( const GLenum &flag ) const
{
  SL_ASSERT ( this );

  // Look for the flag.
  StateMap::const_iterator i = _getCurrentStateMap().find ( flag );

  // If we didn't find it...
  if ( _getCurrentStateMap().end() == i ) 
    return _UNDETERMINED;

  // Sanity check.
  SL_ASSERT ( sizeof ( bool ) == sizeof ( i->second ) );

  // Otherwise, return what we found.
  if ( i->second )
    return _ENABLED;
  else
    return _DISABLED;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Clear the state. This will make it ask OpenGL again to refill the map.
//
/////////////////////////////////////////////////////////////////////////////

void SgGlState::clear()
{
  SL_ASSERT ( this );
  _getCurrentStateMap().clear();
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the matrix mode.
//
/////////////////////////////////////////////////////////////////////////////

void SgGlState::setMatrixMode ( const MatrixMode &mode )
{
  SL_ASSERT ( this );
  SL_ASSERT ( GL_NO_ERROR == ::glGetError() );

  // If we are already in the state we want then just return.
  if ( mode == _matrixMode )
    return;

  // If we are supposed to set the modelview...
  if ( MODELVIEW == mode )
  {
    // Set our flag and tell OpenGL too.
    SL_ASSERT ( MODELVIEW == mode );
    _matrixMode = MODELVIEW;
    ::glMatrixMode ( GL_MODELVIEW );
    SL_ASSERT ( GL_NO_ERROR == ::glGetError() );
  }

  // Otherwise...
  else 
  {
    // Set projection mode.
    SL_ASSERT ( PROJECTION == mode );
    _matrixMode = PROJECTION;
    ::glMatrixMode ( GL_PROJECTION );
    SL_ASSERT ( GL_NO_ERROR == ::glGetError() );
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Get the modelview matrix.
//
/////////////////////////////////////////////////////////////////////////////

const SlMatrix4f &SgGlState::getModelviewMatrix() const
{
  SL_ASSERT ( this );
  SL_ASSERT ( false == _modelviewStack.empty() );
  return _modelviewStack.top();
}


/////////////////////////////////////////////////////////////////////////////
//
//  Push the modelview matrix.
//
/////////////////////////////////////////////////////////////////////////////

void SgGlState::pushModelviewMatrix()
{
  SL_ASSERT ( this );
  SL_ASSERT ( false == _modelviewStack.empty() );
  SL_ASSERT ( GL_NO_ERROR == ::glGetError() );

  // Make sure we're in the right matrix mode.
  this->setMatrixMode ( MODELVIEW );

  // Push OpenGL's matrix.
  //::glPushMatrix();

  #ifdef _DEBUG
  GLenum error = ::glGetError();
  GLint maxDepth, depth;
  if ( GL_NO_ERROR != error )
  {
    SL_ASSERT ( 0 );
    ::glGetIntegerv ( GL_MODELVIEW_STACK_DEPTH, &depth );
    ::glGetIntegerv ( GL_MAX_MODELVIEW_STACK_DEPTH, &maxDepth );
  }
  #endif
  
  SL_ASSERT ( GL_NO_ERROR == ::glGetError() );

  // Push our matrix.
  _modelviewStack.push();
}


/////////////////////////////////////////////////////////////////////////////
//
//  Pop the modelview matrix.
//
/////////////////////////////////////////////////////////////////////////////

void SgGlState::popModelviewMatrix()
{
  SL_ASSERT ( this );
  SL_ASSERT ( false == _modelviewStack.empty() );
  SL_ASSERT ( GL_NO_ERROR == ::glGetError() );

  // Make sure we're in the right matrix mode.
  this->setMatrixMode ( MODELVIEW );

  // Pop OpenGL's matrix.
  //::glPopMatrix();
  SL_ASSERT ( GL_NO_ERROR == ::glGetError() );

  // Pop our matrix.
  _modelviewStack.pop();
}


/////////////////////////////////////////////////////////////////////////////
//
//  Make the modelview matrix the identity.
//
/////////////////////////////////////////////////////////////////////////////

void SgGlState::makeModelviewMatrixIdentity()
{
  SL_ASSERT ( this );
  SL_ASSERT ( false == _modelviewStack.empty() );
  SL_ASSERT ( GL_NO_ERROR == ::glGetError() );

  // Make sure we're in the right matrix mode.
  this->setMatrixMode ( MODELVIEW );

  // Make OpenGL's identity.
  ::glLoadIdentity();
  SL_ASSERT ( GL_NO_ERROR == ::glGetError() );

  // Make ours identity.
  _modelviewStack.top().identity();
}


/////////////////////////////////////////////////////////////////////////////
//
//  Make the projection matrix the identity.
//
/////////////////////////////////////////////////////////////////////////////

void SgGlState::makeProjectionMatrixIdentity()
{
  SL_ASSERT ( this );
  SL_ASSERT ( false == _projectionStack.empty() );
  SL_ASSERT ( GL_NO_ERROR == ::glGetError() );

  // Make sure we're in the right matrix mode.
  this->setMatrixMode ( PROJECTION );

  // Make OpenGL's identity.
  ::glLoadIdentity();
  SL_ASSERT ( GL_NO_ERROR == ::glGetError() );

  // Make ours identity.
  _projectionStack.top().identity();
}


/////////////////////////////////////////////////////////////////////////////
//
//  Get the projection matrix.
//
/////////////////////////////////////////////////////////////////////////////

const SlMatrix4f &SgGlState::getProjectionMatrix() const
{
  SL_ASSERT ( this );
  SL_ASSERT ( false == _projectionStack.empty() );
  return _projectionStack.top();
}


/////////////////////////////////////////////////////////////////////////////
//
//  Push the projection matrix.
//
/////////////////////////////////////////////////////////////////////////////

void SgGlState::pushProjectionMatrix()
{
  SL_ASSERT ( this );
  SL_ASSERT ( false == _projectionStack.empty() );
  SL_ASSERT ( GL_NO_ERROR == ::glGetError() );

  // Make sure we're in the right matrix mode.
  this->setMatrixMode ( PROJECTION );

  // Push OpenGL's matrix.
  //::glPushMatrix();

  #ifdef _DEBUG
  GLenum error = ::glGetError();
  GLint maxDepth, depth;
  if ( GL_NO_ERROR != error )
  {
    SL_ASSERT ( 0 );
    ::glGetIntegerv ( GL_PROJECTION_STACK_DEPTH, &depth );
    ::glGetIntegerv ( GL_MAX_PROJECTION_STACK_DEPTH, &maxDepth );
  }
  #endif

  SL_ASSERT ( GL_NO_ERROR == ::glGetError() );

  // Push our matrix.
  _projectionStack.push();
}


/////////////////////////////////////////////////////////////////////////////
//
//  Pop the projection matrix.
//
/////////////////////////////////////////////////////////////////////////////

void SgGlState::popProjectionMatrix()
{
  SL_ASSERT ( this );
  SL_ASSERT ( false == _projectionStack.empty() );
  SL_ASSERT ( GL_NO_ERROR == ::glGetError() );

  // Make sure we're in the right matrix mode.
  this->setMatrixMode ( PROJECTION );

  // Pop OpenGL's matrix.
  //::glPopMatrix();
  SL_ASSERT ( GL_NO_ERROR == ::glGetError() );

  // Pop our matrix.
  _projectionStack.pop();
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the modelview matrix.
//
/////////////////////////////////////////////////////////////////////////////

void SgGlState::setModelviewMatrix ( const SlMatrix4f &M )
{
  SL_ASSERT ( this );
  SL_ASSERT ( false == _modelviewStack.empty() );
  SL_ASSERT ( GL_NO_ERROR == ::glGetError() );

  // Make sure we're in the right matrix mode.
  this->setMatrixMode ( MODELVIEW );

  // Set OpenGL's matrix.
  ::glLoadMatrixf ( M );
  SL_ASSERT ( GL_NO_ERROR == ::glGetError() );

  // Set our matrix.
  _modelviewStack.top().setValue ( M );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the projection matrix.
//
/////////////////////////////////////////////////////////////////////////////

void SgGlState::setProjectionMatrix ( const SlMatrix4f &M )
{
  SL_ASSERT ( this );
  SL_ASSERT ( false == _projectionStack.empty() );
  SL_ASSERT ( GL_NO_ERROR == ::glGetError() );

  // Make sure we're in the right matrix mode.
  this->setMatrixMode ( PROJECTION );

  // Set OpenGL's matrix.
  ::glLoadMatrixf ( M );
  SL_ASSERT ( GL_NO_ERROR == ::glGetError() );

  // Set our matrix.
  _projectionStack.top().setValue ( M );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Get the top state map.
//
/////////////////////////////////////////////////////////////////////////////

const SgGlState::StateMap &SgGlState::_getCurrentStateMap() const
{
  SL_ASSERT ( this );
  SL_ASSERT ( false == _stateStack.empty() );
  return _stateStack.top();
}


/////////////////////////////////////////////////////////////////////////////
//
//  Get the top state map.
//
/////////////////////////////////////////////////////////////////////////////

SgGlState::StateMap &SgGlState::_getCurrentStateMap()
{
  SL_ASSERT ( this );
  SL_ASSERT ( false == _stateStack.empty() );
  return _stateStack.top();
}


/////////////////////////////////////////////////////////////////////////////
//
//  Push a name.
//
/////////////////////////////////////////////////////////////////////////////

void SgGlState::pushName ( const GLuint &name )
{
  SL_ASSERT ( this );
  _nameStack.push ( name );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Pop a name.
//
/////////////////////////////////////////////////////////////////////////////

void SgGlState::popName()
{
  SL_ASSERT ( this );
  _nameStack.pop();
}


/////////////////////////////////////////////////////////////////////////////
//
//  Initialize the name stack.
//
/////////////////////////////////////////////////////////////////////////////

void SgGlState::initNameStack()
{
  SL_ASSERT ( this );
  _nameStack.clear();
  // The name stack can have a depth of zero.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Initialize the matrix stack.
//
/////////////////////////////////////////////////////////////////////////////

void SgGlState::initModelviewMatrixStack()
{
  SL_ASSERT ( this );
  _modelviewStack.clear();
  _modelviewStack.push ( SlMatrix4f ( SL_MATRIX4_IDENTITY_F ) );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Initialize the matrix stack.
//
/////////////////////////////////////////////////////////////////////////////

void SgGlState::initProjectionMatrixStack()
{
  SL_ASSERT ( this );
  _projectionStack.clear();
  _projectionStack.push ( SlMatrix4f ( SL_MATRIX4_IDENTITY_F ) );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Return the stack's depth.
//
/////////////////////////////////////////////////////////////////////////////

unsigned long SgGlState::getModelviewMatrixStackDepth() const
{
  SL_ASSERT ( this );
  return static_cast<unsigned long>(_modelviewStack.size());
}


/////////////////////////////////////////////////////////////////////////////
//
//  Return the stack's depth.
//
/////////////////////////////////////////////////////////////////////////////

unsigned long SgGlState::getProjectionMatrixStackDepth() const
{
  SL_ASSERT ( this );
  return static_cast<unsigned long>(_projectionStack.size());
}


/////////////////////////////////////////////////////////////////////////////
//
//  Return the stack's depth.
//
/////////////////////////////////////////////////////////////////////////////

unsigned long SgGlState::getNameStackDepth() const
{
  SL_ASSERT ( this );
  return static_cast<unsigned long>(_nameStack.size());
}
