
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

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlRefBase.h"
# include "Standard/SlMatrix4.h"
# include "Standard/SlStack.h"
# include <map>
# include <memory>
#endif
  

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
  const SlMatrix4f &    getModelviewMatrix() const;
  unsigned long         getModelviewMatrixStackDepth() const;
  void                  initModelviewMatrixStack();
  void                  makeModelviewMatrixIdentity();
  void                  popModelviewMatrix();
  void                  pushModelviewMatrix();
  void                  pushModelviewMatrix ( const SlMatrix4f &M );
  void                  setModelviewMatrix ( const SlMatrix4f &M );

  // Projection matrix.
  const SlMatrix4f &    getProjectionMatrix() const;
  unsigned long         getProjectionMatrixStackDepth() const;
  void                  initProjectionMatrixStack();
  void                  makeProjectionMatrixIdentity();
  void                  popProjectionMatrix();
  void                  pushProjectionMatrix();
  void                  pushProjectionMatrix ( const SlMatrix4f &P );
  void                  setProjectionMatrix ( const SlMatrix4f &P );

  // Set the matrix mode.
  void                  setMatrixMode ( const MatrixMode &mode );

protected:

  typedef std::map<unsigned int,bool> StateMap;
  typedef SlStack<SlMatrix4f> MatrixStack;

  MatrixMode _matrixMode;
  std::auto_ptr<MatrixStack> _modelviewStack;
  std::auto_ptr<MatrixStack> _projectionStack;
  std::auto_ptr<StateMap> _stateMap;
  mutable SlMatrix4f _M;
  mutable SlMatrix4f _P;

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
