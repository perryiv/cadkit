
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SgVisitor: The base visitor class.
//
///////////////////////////////////////////////////////////////////////////////

#include "SgPrecompiled.h"
#include "SgVisitor.h"
#include "SgCoordinate3.h"
#include "SgCoordinate4.h"
#include "SgNormal.h"
#include "SgMaterial.h"
#include "SgBaseColor.h"
#include "SgDrawStyle.h"
#include "SgSeparator.h"

#include "Standard/SlPrint.h"
#include "Standard/SlAssert.h"
#include "Standard/SlStack.h"
#include "Standard/SlErrorPolicy.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <memory>
# include <stdexcept>
#endif

using namespace CadKit;

SG_IMPLEMENT_VISITOR(SgVisitor,SlRefBase);


///////////////////////////////////////////////////////////////////////////////
//
//  Can't have these in the header file.
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit
{
  typedef CadKit::ErrorPolicy::Throw < std::out_of_range > StackErrorPolicy;

  typedef SlStack < SgNode::Ptr,        StackErrorPolicy > NodeStack;
  typedef SlStack < SgCoordinate::Ptr,  StackErrorPolicy > CoordinateStack;
  typedef SlStack < SgCoordinate3::Ptr, StackErrorPolicy > Coordinate3Stack;
  typedef SlStack < SgCoordinate4::Ptr, StackErrorPolicy > Coordinate4Stack;
  typedef SlStack < SgNormal::Ptr,      StackErrorPolicy > NormalStack;
  typedef SlStack < SgMaterial::Ptr,    StackErrorPolicy > MaterialStack;
  typedef SlStack < SgBaseColor::Ptr,   StackErrorPolicy > BaseColorStack;
  typedef SlStack < SgDrawStyle::Ptr,   StackErrorPolicy > DrawStyleStack;
};
 

///////////////////////////////////////////////////////////////////////////////
//
//  For convenience.
//
///////////////////////////////////////////////////////////////////////////////

#define NODE_STACK        ((NodeStack *)        _nodeStack)
#define COORDINATE_STACK  ((CoordinateStack *)  _coordinateStack)
#define COORDINATE3_STACK ((Coordinate3Stack *) _coordinate3Stack)
#define COORDINATE4_STACK ((Coordinate4Stack *) _coordinate4Stack)
#define NORMAL_STACK      ((NormalStack *)      _normalStack)
#define MATERIAL_STACK    ((MaterialStack *)    _materialStack)
#define BASE_COLOR_STACK  ((BaseColorStack *)   _baseColorStack)
#define DRAW_STYLE_STACK  ((DrawStyleStack *)   _drawStyleStack)


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SgVisitor::SgVisitor() : SlRefBase ( 0 ),
  _nodeStack          ( new NodeStack ),
  _coordinateStack    ( new CoordinateStack ),
  _coordinate3Stack   ( new Coordinate3Stack ),
  _coordinate4Stack   ( new Coordinate4Stack ),
  _normalStack        ( new NormalStack ),
  _materialStack      ( new MaterialStack ),
  _baseColorStack     ( new BaseColorStack ),
  _drawStyleStack     ( new DrawStyleStack ),
  _currentCoordinate  ( NULL ),
  _currentCoordinate3 ( NULL ),
  _currentCoordinate4 ( NULL ),
  _currentNormal      ( NULL ),
  _currentMaterial    ( NULL ),
  _currentBaseColor   ( NULL ),
  _currentDrawStyle   ( NULL )
{
  SL_PRINT2 ( "SgVisitor::SgVisitor(), this = %X\n", this );
  SL_ASSERT ( _nodeStack );
  SL_ASSERT ( _coordinateStack );
  SL_ASSERT ( _coordinate3Stack );
  SL_ASSERT ( _coordinate4Stack );
  SL_ASSERT ( _normalStack );
  SL_ASSERT ( _materialStack );
  SL_ASSERT ( _baseColorStack );
  SL_ASSERT ( _drawStyleStack );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SgVisitor::~SgVisitor()
{
  SL_PRINT2 ( "SgVisitor::~SgVisitor(), this = %X\n", this );
  
  SL_ASSERT ( NODE_STACK->empty() );
  SL_ASSERT ( COORDINATE_STACK->empty() );
  SL_ASSERT ( COORDINATE3_STACK->empty() );
  SL_ASSERT ( COORDINATE4_STACK->empty() );
  SL_ASSERT ( NORMAL_STACK->empty() );
  SL_ASSERT ( MATERIAL_STACK->empty() );
  SL_ASSERT ( BASE_COLOR_STACK->empty() );
  SL_ASSERT ( DRAW_STYLE_STACK->empty() );

  delete NODE_STACK;
  delete COORDINATE_STACK;
  delete COORDINATE3_STACK;
  delete COORDINATE4_STACK;
  delete NORMAL_STACK;
  delete MATERIAL_STACK;
  delete BASE_COLOR_STACK;
  delete DRAW_STYLE_STACK;

  // These are not ref-pointers. You manually referenced them in 
  // _setCurrentNode() below. You have to safely release them.
  CadKit::safeUnref ( _currentCoordinate );
  CadKit::safeUnref ( _currentCoordinate3 );
  CadKit::safeUnref ( _currentCoordinate4 );
  CadKit::safeUnref ( _currentNormal );
  CadKit::safeUnref ( _currentMaterial );
  CadKit::safeUnref ( _currentBaseColor );
  CadKit::safeUnref ( _currentDrawStyle );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reset the visitor. Call this in between scene traversals to ensure that
//  any accumulated states are reset.
//
///////////////////////////////////////////////////////////////////////////////

bool SgVisitor::reset()
{
  SL_ASSERT ( this );
  NODE_STACK->clear();
  COORDINATE_STACK->clear();
  COORDINATE3_STACK->clear();
  COORDINATE4_STACK->clear();
  NORMAL_STACK->clear();
  MATERIAL_STACK->clear();
  BASE_COLOR_STACK->clear();
  DRAW_STYLE_STACK->clear();
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Push the node.
//
///////////////////////////////////////////////////////////////////////////////

void SgVisitor::pushNode ( SgNode *node )
{
  SL_ASSERT ( this );

  // This will reference the node.
  NODE_STACK->push ( node );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pop the top node.
//
///////////////////////////////////////////////////////////////////////////////

void SgVisitor::popNode()
{
  SL_ASSERT ( this );
  SL_ASSERT ( false == NODE_STACK->empty() );

  // This will unreference the node.
  NODE_STACK->pop();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Template function to implement the "set" functions. This is exactly what
//  SlRefPtr::setValue() does, but we can't use that because of the way we 
//  are hiding the class types.
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit
{
template <class T> inline void _setCurrentNode ( T *node, T *&current )
{
  // If the two pointers are the same address then just return.
  if ( node == current )
    return;

  // Make a copy of the current one.
  T *old = current;

  // Copy the given pointer.
  current = node;

  // If the given pointer is not null then reference it.
  CadKit::safeRef ( current );

  // If the old one is not null then release it. Make sure we do this last 
  // because the given pointer "p" could be a child (indirect or direct) of 
  // "old". If we release "old" before we reference "p" then "p" may get 
  // deleted when it should not have.
  CadKit::safeUnref ( old );
}
}; // namespace CadKit


///////////////////////////////////////////////////////////////////////////////
//
//  Set the current node.
//
///////////////////////////////////////////////////////////////////////////////

void SgVisitor::_setCurrentCoordinate   ( SgCoordinate *node )  { CadKit::_setCurrentNode ( node, _currentCoordinate ); }
void SgVisitor::_setCurrentCoordinate3  ( SgCoordinate3 *node ) { CadKit::_setCurrentNode ( node, _currentCoordinate3 ); }
void SgVisitor::_setCurrentCoordinate4  ( SgCoordinate4 *node ) { CadKit::_setCurrentNode ( node, _currentCoordinate4 ); }
void SgVisitor::_setCurrentNormal       ( SgNormal *node )      { CadKit::_setCurrentNode ( node, _currentNormal ); }
void SgVisitor::_setCurrentMaterial     ( SgMaterial *node )    { CadKit::_setCurrentNode ( node, _currentMaterial ); }
void SgVisitor::_setCurrentBaseColor    ( SgBaseColor *node )   { CadKit::_setCurrentNode ( node, _currentBaseColor ); }
void SgVisitor::_setCurrentDrawStyle    ( SgDrawStyle *node )   { CadKit::_setCurrentNode ( node, _currentDrawStyle ); }


///////////////////////////////////////////////////////////////////////////////
//
//  Template function to implement the "push" functions.
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit
{
template <class Stack, class Node> inline void _pushNode ( Stack &stack, Node *node )
{
  // Push the node onto the stack. This will reference it.
  stack.push ( node );
}
}; // namespace CadKit


///////////////////////////////////////////////////////////////////////////////
//
//  Push the current node.
//
///////////////////////////////////////////////////////////////////////////////

void SgVisitor::_pushCoordinate()  { CadKit::_pushNode ( *COORDINATE_STACK,  _currentCoordinate ); }
void SgVisitor::_pushCoordinate3() { CadKit::_pushNode ( *COORDINATE3_STACK, _currentCoordinate3 ); }
void SgVisitor::_pushCoordinate4() { CadKit::_pushNode ( *COORDINATE4_STACK, _currentCoordinate4 ); }
void SgVisitor::_pushNormal()      { CadKit::_pushNode ( *NORMAL_STACK,      _currentNormal ); }
void SgVisitor::_pushMaterial()    { CadKit::_pushNode ( *MATERIAL_STACK,    _currentMaterial ); }
void SgVisitor::_pushBaseColor()   { CadKit::_pushNode ( *BASE_COLOR_STACK,  _currentBaseColor ); }
void SgVisitor::_pushDrawStyle()   { CadKit::_pushNode ( *DRAW_STYLE_STACK,  _currentDrawStyle ); }


namespace CadKit {


///////////////////////////////////////////////////////////////////////////////
//
//  Template function to implement the "pop" functions.
//
///////////////////////////////////////////////////////////////////////////////

template <class Stack, class Node> inline void _popNode ( SgVisitor &visitor, Stack &stack, Node *&current )
{
  // Should be true.
  SL_ASSERT ( false == stack.empty() );

  // Get the node on the top of the stack. We have to catch this with a 
  // ref-pointer or else it crashes when the same group is added twice to 
  // the scene (at least that's when I noticed). I think the node gets 
  // deleted prematurely without the ref-pointer catch.
  SlRefPtr<Node> topNode = stack.top();

  // Pop the top node. This will unreference it. Since we have already 
  // referenced the node above with the ref-pointer catch, this will not 
  // delete the node.
  stack.pop();

  // If the top node is the same as the given one then return. This takes 
  // care of the case when they are both null.
  if ( topNode.getValue() == current )
    return;

  // If the top node is null...
  if ( topNode.isNull() )
  {
    // They should all be null. The only time a null is pushed onto the stack 
    // is when that node has not been encountered yet. Therefore, every other
    // node on the stack should also be null.
    SL_ASSERT ( stack.end() == std::find_if ( stack.begin(), stack.end(), std::bind2nd ( std::not_equal_to<Node *>(), (Node *) NULL ) ) );

    // The current node is now null.
    CadKit::_setCurrentNode ( (Node *) NULL, current );
    SL_ASSERT ( NULL == current );

    // We have to restore default values so make a new node and visit it.
    SlRefPtr<Node> temp = static_cast<Node *> ( Node::getClassType()->createObject() );

    // Make sure we have a new node. If the class is concrete then we should.
    if ( temp.isValid() )
      temp->accept ( visitor );

    // Done with this case.
    return;
  }

  // Should be true when we get to here. We should never have a null current
  // node if we have a non-null top node.
  SL_ASSERT ( topNode.isValid() && current );

  // If neither one of them are null then check their respective values 
  // and return if they are the same.
  if ( topNode->isEqualVisibleProperties ( *current ) )
    return;

  // Set the new "current" node.
  CadKit::_setCurrentNode ( topNode.getValue(), current );

  // Visit the new "current" node. Is this a potential infinite loop?
  current->accept ( visitor );
}


}; // namespace CadKit


///////////////////////////////////////////////////////////////////////////////
//
//  Pop the top node.
//
///////////////////////////////////////////////////////////////////////////////

void SgVisitor::_popCoordinate()  { CadKit::_popNode ( *this, *COORDINATE_STACK,  _currentCoordinate ); }
void SgVisitor::_popCoordinate3() { CadKit::_popNode ( *this, *COORDINATE3_STACK, _currentCoordinate3 ); }
void SgVisitor::_popCoordinate4() { CadKit::_popNode ( *this, *COORDINATE4_STACK, _currentCoordinate4 ); }
void SgVisitor::_popNormal()      { CadKit::_popNode ( *this, *NORMAL_STACK,      _currentNormal ); }
void SgVisitor::_popMaterial()    { CadKit::_popNode ( *this, *MATERIAL_STACK,    _currentMaterial ); }
void SgVisitor::_popBaseColor()   { CadKit::_popNode ( *this, *BASE_COLOR_STACK,  _currentBaseColor ); }
void SgVisitor::_popDrawStyle()   { CadKit::_popNode ( *this, *DRAW_STYLE_STACK,  _currentDrawStyle ); }


///////////////////////////////////////////////////////////////////////////////
//
//  Push the appropriate nodes.
//
///////////////////////////////////////////////////////////////////////////////

void SgVisitor::_pushStateNodes ( const SgSeparator &separator )
{
  SL_ASSERT ( this );

  if ( separator.hasPushPopFlags ( SgSeparator::COORDINATES ) )
  {
    this->_pushCoordinate();
    this->_pushCoordinate3();
    this->_pushCoordinate4();
  }

  if ( separator.hasPushPopFlags ( SgSeparator::NORMALS ) )
    this->_pushNormal();

  if ( separator.hasPushPopFlags ( SgSeparator::MATERIAL ) )
    this->_pushMaterial();
  
  if ( separator.hasPushPopFlags ( SgSeparator::BASE_COLOR ) )
    this->_pushBaseColor();

  if ( separator.hasPushPopFlags ( SgSeparator::DRAW_STYLE ) )
    this->_pushDrawStyle();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pop the appropriate nodes.
//
///////////////////////////////////////////////////////////////////////////////

void SgVisitor::_popStateNodes ( const SgSeparator &separator )
{
  SL_ASSERT ( this );

  if ( separator.hasPushPopFlags ( SgSeparator::DRAW_STYLE ) )
    this->_popDrawStyle();

  if ( separator.hasPushPopFlags ( SgSeparator::BASE_COLOR ) )
    this->_popBaseColor();

  if ( separator.hasPushPopFlags ( SgSeparator::MATERIAL ) )
    this->_popMaterial();

  if ( separator.hasPushPopFlags ( SgSeparator::NORMALS ) )
    this->_popNormal();

  if ( separator.hasPushPopFlags ( SgSeparator::COORDINATES ) )
  {
    this->_popCoordinate();
    this->_popCoordinate3();
    this->_popCoordinate4();
  }
}
