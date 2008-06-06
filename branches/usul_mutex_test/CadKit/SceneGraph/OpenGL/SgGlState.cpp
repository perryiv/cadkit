
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SgGlState: Keeps track of the OpenGL state.
//
///////////////////////////////////////////////////////////////////////////////

#include "SgGlPrecompiled.h"
#include "SgGlState.h"
#include "SgGlErrorCheck.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlPrint.h"
# include "Standard/SlAssert.h"
# include <stdlib.h>
# include <GL/gl.h>
#endif

using namespace CadKit;

SL_IMPLEMENT_DYNAMIC_CLASS(SgGlState,SlRefBase);


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SgGlState::SgGlState() : SlRefBase ( 0 ),
  _matrixMode ( MODELVIEW ),
  _modelviewStack ( new MatrixStack ),
  _projectionStack ( new MatrixStack ),
  _stateMap ( new StateMap )
{
  SL_PRINT2 ( "SgGlState::SgGlState(), this = %X\n", this );
  SL_ASSERT ( NULL != _modelviewStack.get() );
  SL_ASSERT ( NULL != _projectionStack.get() );
  SL_ASSERT ( NULL != _stateMap.get() );

  // Initialize the matrix stacks. Don't call the respective init functions 
  // because they require a current context (which may not exist yet).
  SlMatrix44f I ( SL_MATRIX_44_IDENTITY_F );
  _modelviewStack->push ( I );
  _projectionStack->push ( I );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SgGlState::~SgGlState()
{
  SL_PRINT2 ( "SgGlState::~SgGlState(), this = %X\n", this );

  // The stacks are auto-pointers. They get deleted automatically.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Enable the state-flag.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlState::enable  ( const GLenum &flag )
{
  SL_ASSERT ( this );
  SG_GL_ERROR_CHECKER;

  // See if this state-flag is already enabled.
  if ( _ENABLED == this->_getState ( flag ) )
  {
    // Should be turned on.
    SL_ASSERT ( GL_TRUE == ::glIsEnabled ( flag ) );
    return true;
  }

  // Turn on the state.
  ::glEnable ( flag );
  SG_GL_ERROR_CHECK_NOW;

  // Should be turned on.
  SL_ASSERT ( GL_TRUE == ::glIsEnabled ( flag ) );
  SG_GL_ERROR_CHECK_NOW;

  // Save the state.
  (*_stateMap)[flag] = true;

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Disable the state-flag.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlState::disable  ( const GLenum &flag )
{
  SL_ASSERT ( this );
  SG_GL_ERROR_CHECKER;

  // See if this state-flag is already enabled.
  if ( _DISABLED == this->_getState ( flag ) )
  {
    // Should be turned off.
    SL_ASSERT ( GL_FALSE == ::glIsEnabled ( flag ) );
    return true;
  }

  // Turn on the state.
  ::glDisable ( flag );
  SG_GL_ERROR_CHECK_NOW;

  // Should be turned on.
  SL_ASSERT ( GL_FALSE == ::glIsEnabled ( flag ) );
  SG_GL_ERROR_CHECK_NOW;

  // Save the state.
  (*_stateMap)[flag] = true;

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the flag is enabled.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlState::isEnabled  ( const GLenum &flag )
{
  SL_ASSERT ( this );
  SG_GL_ERROR_CHECKER;

  // Ask OpenGL.
  bool result = ( GL_TRUE == ::glIsEnabled ( flag ) );
  SG_GL_ERROR_CHECK_NOW;

  // Return the result.
  return result;
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the flag is disabled.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlState::isDisabled  ( const GLenum &flag )
{
  SL_ASSERT ( this );

  // Call the other one.
  return ( false == this->isEnabled ( flag ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the state from the map (don't ask OpenGL).
//
///////////////////////////////////////////////////////////////////////////////

SgGlState::State SgGlState::_getState ( const GLenum &flag ) const
{
  SL_ASSERT ( this );

  // Look for the flag.
  StateMap::const_iterator i = _stateMap->find ( flag );

  // If we didn't find it...
  if ( i == _stateMap->end() ) 
    return _UNDETERMINED;

  // Sanity check.
  SL_ASSERT ( sizeof ( bool ) == sizeof ( i->second ) );

  // Otherwise, return what we found.
  if ( i->second )
    return _ENABLED;
  else
    return _DISABLED;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the state. This will make it ask OpenGL again to refill the map.
//
///////////////////////////////////////////////////////////////////////////////

void SgGlState::clear()
{
  SL_ASSERT ( this );
  _stateMap->clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the matrix mode.
//
///////////////////////////////////////////////////////////////////////////////

void SgGlState::setMatrixMode ( const MatrixMode &mode )
{
  SL_ASSERT ( this );
  SG_GL_ERROR_CHECKER;

  // If we are already in the state we want then just return.
  if ( mode == _matrixMode )
    return;

  // If we are supposed to set the modelview...
  if ( MODELVIEW == mode )
  {
    // Set our flag and tell OpenGL too.
    _matrixMode = MODELVIEW;
    ::glMatrixMode ( GL_MODELVIEW );
    SG_GL_ERROR_CHECK_NOW;
  }

  // Otherwise...
  else 
  {
    // Set projection mode.
    SL_ASSERT ( PROJECTION == mode );
    _matrixMode = PROJECTION;
    ::glMatrixMode ( GL_PROJECTION );
    SG_GL_ERROR_CHECK_NOW;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the modelview matrix.
//
///////////////////////////////////////////////////////////////////////////////

const SlMatrix44f &SgGlState::getModelviewMatrix() const
{
  SL_ASSERT ( this );
  SL_ASSERT ( false == _modelviewStack->empty() );
  SG_GL_ERROR_CHECKER;

  // Get the current modelview matrix from OpenGL.
  ::glGetFloatv ( GL_MODELVIEW_MATRIX, _modelViewMatrix );
  SG_GL_ERROR_CHECK_NOW;

  // Return the current matrix.
  return _modelViewMatrix;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the projection matrix.
//
///////////////////////////////////////////////////////////////////////////////

const SlMatrix44f &SgGlState::getProjectionMatrix() const
{
  SL_ASSERT ( this );
  SL_ASSERT ( false == _projectionStack->empty() );
  SG_GL_ERROR_CHECKER;

  // Get the current projection matrix from OpenGL.
  ::glGetFloatv ( GL_PROJECTION_MATRIX, _projectionMatrix );
  SG_GL_ERROR_CHECK_NOW;

  // Return the current matrix.
  return _projectionMatrix;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Push the modelview matrix.
//
///////////////////////////////////////////////////////////////////////////////

void SgGlState::pushModelviewMatrix()
{
  SL_ASSERT ( this );
  SL_ASSERT ( false == _modelviewStack->empty() );
  SG_GL_ERROR_CHECKER;

  // Push OpenGL's current matrix onto our stack.
  const SlMatrix44f &M = this->getModelviewMatrix();
  _modelviewStack->push ( M );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Push the projection matrix.
//
///////////////////////////////////////////////////////////////////////////////

void SgGlState::pushProjectionMatrix()
{
  SL_ASSERT ( this );
  SL_ASSERT ( false == _projectionStack->empty() );
  SG_GL_ERROR_CHECKER;

  // Push OpenGL's current matrix onto our stack.
  const SlMatrix44f &P = this->getProjectionMatrix();
  _projectionStack->push ( P );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pop the modelview matrix.
//
///////////////////////////////////////////////////////////////////////////////

void SgGlState::popModelviewMatrix()
{
  SL_ASSERT ( this );
  SL_ASSERT ( false == _modelviewStack->empty() );
  SG_GL_ERROR_CHECKER;

  // Make sure we're in the right matrix mode.
  this->setMatrixMode ( MODELVIEW );

  // Load the matrix that is saved on top of the stack.
  const SlMatrix44f &M = _modelviewStack->top();
  ::glLoadMatrixf ( M );
  SG_GL_ERROR_CHECK_NOW;

  // Pop our matrix.
  _modelviewStack->pop();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pop the projection matrix.
//
///////////////////////////////////////////////////////////////////////////////

void SgGlState::popProjectionMatrix()
{
  SL_ASSERT ( this );
  SL_ASSERT ( false == _projectionStack->empty() );
  SG_GL_ERROR_CHECKER;

  // Make sure we're in the right matrix mode.
  this->setMatrixMode ( PROJECTION );

  // Load the matrix that is saved on top of the stack.
  const SlMatrix44f &P = _projectionStack->top();
  ::glLoadMatrixf ( P );
  SG_GL_ERROR_CHECK_NOW;

  // Pop our matrix.
  _projectionStack->pop();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the modelview matrix the identity.
//
///////////////////////////////////////////////////////////////////////////////

void SgGlState::makeModelviewMatrixIdentity()
{
  SL_ASSERT ( this );
  SL_ASSERT ( false == _modelviewStack->empty() );
  SG_GL_ERROR_CHECKER;

  // Make sure we're in the right matrix mode.
  this->setMatrixMode ( MODELVIEW );

  // Make OpenGL's identity (not ours).
  ::glLoadIdentity();
  SG_GL_ERROR_CHECK_NOW;

  // Make our cached copy identity.
  _modelViewMatrix.identity();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the projection matrix the identity.
//
///////////////////////////////////////////////////////////////////////////////

void SgGlState::makeProjectionMatrixIdentity()
{
  SL_ASSERT ( this );
  SL_ASSERT ( false == _projectionStack->empty() );
  SG_GL_ERROR_CHECKER;

  // Make sure we're in the right matrix mode.
  this->setMatrixMode ( PROJECTION );

  // Make OpenGL's identity (not ours).
  ::glLoadIdentity();
  SG_GL_ERROR_CHECK_NOW;

  // Make our cached copy identity.
  _projectionMatrix.identity();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the modelview matrix.
//
///////////////////////////////////////////////////////////////////////////////

void SgGlState::setModelviewMatrix ( const SlMatrix44f &M )
{
  SL_ASSERT ( this );
  SL_ASSERT ( false == _modelviewStack->empty() );
  SG_GL_ERROR_CHECKER;

  // Make sure we're in the right matrix mode.
  this->setMatrixMode ( MODELVIEW );

  // Set OpenGL's matrix (not ours).
  ::glLoadMatrixf ( M );
  SG_GL_ERROR_CHECK_NOW;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the projection matrix.
//
///////////////////////////////////////////////////////////////////////////////

void SgGlState::setProjectionMatrix ( const SlMatrix44f &P )
{
  SL_ASSERT ( this );
  SL_ASSERT ( false == _projectionStack->empty() );
  SG_GL_ERROR_CHECKER;

  // Make sure we're in the right matrix mode.
  this->setMatrixMode ( PROJECTION );

  // Set OpenGL's matrix (not ours).
  ::glLoadMatrixf ( P );
  SG_GL_ERROR_CHECK_NOW;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the matrix stack.
//
///////////////////////////////////////////////////////////////////////////////

void SgGlState::initModelviewMatrixStack()
{
  SL_ASSERT ( this );

  // Make an identity matrix.
  SlMatrix44f I ( SL_MATRIX_44_IDENTITY_F );

  // Put one identity matrix on the stack.
  _modelviewStack->clear();
  _modelviewStack->push ( I );

  // Make OpenGL's matrix identity too.
  this->makeModelviewMatrixIdentity();

  // Should be true.
  SL_ASSERT ( I == this->getModelviewMatrix() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the matrix stack.
//
///////////////////////////////////////////////////////////////////////////////

void SgGlState::initProjectionMatrixStack()
{
  SL_ASSERT ( this );

  // Make an identity matrix.
  SlMatrix44f I ( SL_MATRIX_44_IDENTITY_F );

  // Put one identity matrix on the stack.
  _projectionStack->clear();
  _projectionStack->push ( SlMatrix44f ( SL_MATRIX_44_IDENTITY_F ) );

  // Make OpenGL's matrix identity too.
  this->makeProjectionMatrixIdentity();

  // Should be true.
  SL_ASSERT ( I == this->getProjectionMatrix() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the stack's depth.
//
///////////////////////////////////////////////////////////////////////////////

unsigned long SgGlState::getModelviewMatrixStackDepth() const
{
  SL_ASSERT ( this );
  return static_cast<unsigned long>(_modelviewStack->size());
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the stack's depth.
//
///////////////////////////////////////////////////////////////////////////////

unsigned long SgGlState::getProjectionMatrixStackDepth() const
{
  SL_ASSERT ( this );
  return static_cast<unsigned long>(_projectionStack->size());
}
