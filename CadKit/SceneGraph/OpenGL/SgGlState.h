
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

#ifndef _CADKIT_SCENEGRAPH_OPENGL_STATE_H_
#define _CADKIT_SCENEGRAPH_OPENGL_STATE_H_

#include "SgGlApi.h"

#include "Standard/SlRefBase.h"
#include "Standard/SlMatrix44.h"
#include "Standard/SlStack.h"

#include <stdexcept> // VC6 wants these before SlErrorPolicy.h
#include <string>

#include "Standard/SlErrorPolicy.h"

#include <map>
#include <memory>
#include <stdexcept>
  

namespace CadKit
{
class SG_GL_API SgGlState : public SlRefBase
{
public:

  enum MatrixMode
  {
    MODELVIEW,
    PROJECTION,
  };

  SgGlState();

  // Clear the state. This will make it ask OpenGL again.
  void                  clear();

  // Enable/disable the state-flag.
  bool                  enable     ( const unsigned int &flag );
  bool                  disable    ( const unsigned int &flag );
  bool                  isEnabled  ( const unsigned int &flag );
  bool                  isDisabled ( const unsigned int &flag );

  // Modelview matrix.
  const SlMatrix44f &   getModelviewMatrix() const;
  unsigned long         getModelviewMatrixStackDepth() const;
  void                  initModelviewMatrixStack();
  void                  makeModelviewMatrixIdentity();
  void                  popModelviewMatrix();
  void                  pushModelviewMatrix();
  void                  pushModelviewMatrix ( const SlMatrix44f &M );
  void                  setModelviewMatrix ( const SlMatrix44f &M );

  // Projection matrix.
  const SlMatrix44f &   getProjectionMatrix() const;
  unsigned long         getProjectionMatrixStackDepth() const;
  void                  initProjectionMatrixStack();
  void                  makeProjectionMatrixIdentity();
  void                  popProjectionMatrix();
  void                  pushProjectionMatrix();
  void                  pushProjectionMatrix ( const SlMatrix44f &P );
  void                  setProjectionMatrix ( const SlMatrix44f &P );

  // Set the matrix mode.
  void                  setMatrixMode ( const MatrixMode &mode );

protected:

  typedef std::map<unsigned int,bool> StateMap;
  typedef CadKit::ErrorPolicy::Throw < std::out_of_range > StackErrorPolicy;
  typedef SlStack < SlMatrix44f, StackErrorPolicy > MatrixStack;

  MatrixMode _matrixMode;
  std::auto_ptr<MatrixStack> _modelviewStack;
  std::auto_ptr<MatrixStack> _projectionStack;
  std::auto_ptr<StateMap> _stateMap;
  mutable SlMatrix44f _modelViewMatrix;
  mutable SlMatrix44f _projectionMatrix;

  virtual ~SgGlState();

  // Possible states.
  enum State
  {
    _ENABLED,
    _DISABLED,
    _UNDETERMINED
  };

  State                 _getState ( const unsigned int &flag ) const;

  SL_DECLARE_DYNAMIC_CLASS(SgGlState,1032912059);
  SL_DECLARE_REFERENCE_POINTER(SgGlState);
};

}; // namespace CadKit

#endif // _CADKIT_SCENEGRAPH_OPENGL_STATE_H_
