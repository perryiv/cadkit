
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgVisitor: The base visitor class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_BASE_VISITOR_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_BASE_VISITOR_H_

#include "SgApi.h"
#include "SgTypedefs.h"
#include "SgVisitorMacros.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlRefBase.h"
#endif

// Declare all the nodes for this class.
SG_FORWARD_DECLARE_ALL_NODES;


namespace CadKit
{
class SG_API SgVisitor : public SlRefBase
{
public:

  // The visit functions.
  virtual bool          visit ( SgBackground & ) = 0;
  virtual bool          visit ( SgBaseColor & ) = 0;
  virtual bool          visit ( SgBillboard & ) = 0;
  virtual bool          visit ( SgCamera & ) = 0;
  virtual bool          visit ( SgCircle & ) = 0;
  virtual bool          visit ( SgColorWheel & ) = 0;
  virtual bool          visit ( SgCone & ) = 0;
  virtual bool          visit ( SgCoordinate3 & ) = 0;
  virtual bool          visit ( SgCoordinate4 & ) = 0;
  virtual bool          visit ( SgCube & ) = 0;
  virtual bool          visit ( SgCylinder & ) = 0;
  virtual bool          visit ( SgDirectionalLight & ) = 0;
  virtual bool          visit ( SgDrawStyle & ) = 0;
  virtual bool          visit ( SgEllipse & ) = 0;
  virtual bool          visit ( SgGroup & ) = 0;
  virtual bool          visit ( SgIdentity & ) = 0;
  virtual bool          visit ( SgLineSet & ) = 0;
  virtual bool          visit ( SgMaterial & ) = 0;
  virtual bool          visit ( SgNode & ) = 0;
  virtual bool          visit ( SgNormal & ) = 0;
  virtual bool          visit ( SgOrthographicCamera & ) = 0;
  virtual bool          visit ( SgPerspectiveCamera & ) = 0;
  virtual bool          visit ( SgPointSet & ) = 0;
  virtual bool          visit ( SgQuadMesh & ) = 0;
  virtual bool          visit ( SgRectangle & ) = 0;
  virtual bool          visit ( SgRotation & ) = 0;
  virtual bool          visit ( SgScale & ) = 0;
  virtual bool          visit ( SgSelection & ) = 0;
  virtual bool          visit ( SgSeparator & ) = 0;
  virtual bool          visit ( SgShading & ) = 0;
  virtual bool          visit ( SgSphere & ) = 0;
  virtual bool          visit ( SgSquare & ) = 0;
  virtual bool          visit ( SgSwitch & ) = 0;
  virtual bool          visit ( SgTransform & ) = 0;
  virtual bool          visit ( SgTranslation & ) = 0;
  virtual bool          visit ( SgTriangle & ) = 0;
  virtual bool          visit ( SgTriStrip & ) = 0;
  virtual bool          visit ( SgUnScale & ) = 0;
  virtual bool          visit ( SgShape & ) = 0;
  virtual bool          visit ( SgCoordinate & ) = 0;
  virtual bool          visit ( SgLight & ) = 0;
  virtual bool          visit ( SgVertexSet & ) = 0;
  virtual bool          visit ( SgPrimitive & ) = 0;
  virtual bool          visit ( SgText & ) = 0;

  // Push/pop the node. These are called from each node's accept() function.
  // Normal usage does not require calling these, but they have to be public.
  void                  pushNode ( SgNode * );
  void                  popNode();

  // Reset the visitor. Call this in between scene visits to ensure that
  // any accumulated states are reset.
  virtual bool          reset();

protected:

  SgVisitor();
  virtual ~SgVisitor();

  const SgCoordinate *  getCurrentCoordinate()  const { return _currentCoordinate; }
  const SgCoordinate3 * getCurrentCoordinate3() const { return _currentCoordinate3; }
  const SgCoordinate4 * getCurrentCoordinate4() const { return _currentCoordinate4; }
  const SgNormal *      getCurrentNormal()      const { return _currentNormal; }
  const SgMaterial *    getCurrentMaterial()    const { return _currentMaterial; }
  const SgBaseColor *   getCurrentBaseColor()   const { return _currentBaseColor; }
  const SgDrawStyle *   getCurrentDrawStyle()   const { return _currentDrawStyle; }

  SgCoordinate *        getCurrentCoordinate()  { return _currentCoordinate; }
  SgCoordinate3 *       getCurrentCoordinate3() { return _currentCoordinate3; }
  SgCoordinate4 *       getCurrentCoordinate4() { return _currentCoordinate4; }
  SgNormal *            getCurrentNormal()      { return _currentNormal; }
  SgMaterial *          getCurrentMaterial()    { return _currentMaterial; }
  SgBaseColor *         getCurrentBaseColor()   { return _currentBaseColor; }
  SgDrawStyle *         getCurrentDrawStyle()   { return _currentDrawStyle; }

  void                  _pushStateNodes ( const SgSeparator &separator );
  void                  _pushCoordinate();
  void                  _pushCoordinate3();
  void                  _pushCoordinate4();
  void                  _pushNormal();
  void                  _pushMaterial();
  void                  _pushBaseColor();
  void                  _pushDrawStyle();

  void                  _popStateNodes ( const SgSeparator &separator );
  void                  _popCoordinate();
  void                  _popCoordinate3();
  void                  _popCoordinate4();
  void                  _popNormal();
  void                  _popMaterial();
  void                  _popBaseColor();
  void                  _popDrawStyle();

  void                  _setCurrentCoordinate  ( SgCoordinate *coordinate );
  void                  _setCurrentCoordinate3 ( SgCoordinate3 *coordinate3 );
  void                  _setCurrentCoordinate4 ( SgCoordinate4 *coordinate4 );
  void                  _setCurrentNormal      ( SgNormal *normal );
  void                  _setCurrentMaterial    ( SgMaterial *material );
  void                  _setCurrentBaseColor   ( SgBaseColor *baseColor );
  void                  _setCurrentDrawStyle   ( SgDrawStyle *drawStyle );

private:

  // Have to hide the actual types because this header is included in SgNode.h,
  // and in order to define the stacks, we need the definition of the classes.
  void *_nodeStack;
  void *_coordinateStack;
  void *_coordinate3Stack;
  void *_coordinate4Stack;
  void *_normalStack;
  void *_materialStack;
  void *_baseColorStack;
  void *_drawStyleStack;

  SgCoordinate *_currentCoordinate;
  SgCoordinate3 *_currentCoordinate3;
  SgCoordinate4 *_currentCoordinate4;
  SgNormal *_currentNormal;
  SgMaterial *_currentMaterial;
  SgBaseColor *_currentBaseColor;
  SgDrawStyle *_currentDrawStyle;

  SG_DECLARE_VISITOR ( SgVisitor, 0x00001039 );
};

}; // namespace CadKit

#endif // _CADKIT_SCENEGRAPH_CORE_LIBRARY_BASE_VISITOR_H_
