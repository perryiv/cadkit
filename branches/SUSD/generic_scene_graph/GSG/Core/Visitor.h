
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all scene graph visitors.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_VISITOR_H_
#define _GENERIC_SCENE_GRAPH_CORE_VISITOR_H_

#include "GSG/Core/Referenced.h"


/////////////////////////////////////////////////////////////////////////////
//
//  Macro to declare visit-node functions.
//
/////////////////////////////////////////////////////////////////////////////

#define GSG_DECLARE_VISIT_NODE_FUNCTIONS \
  virtual void    _visit ( Node & ); \
  virtual void    _visit ( Group & ); \
  virtual void    _visit ( Camera & ); \
  virtual void    _visit ( Transform & ); \
  virtual void    _visit ( Lod & ); \
  virtual void    _visit ( Shape & )


/////////////////////////////////////////////////////////////////////////////
//
//  Macro to declare visit-element functions.
//
/////////////////////////////////////////////////////////////////////////////

#define GSG_DECLARE_VISIT_ELEMENT_FUNCTIONS \
  virtual void    _visit ( PrimitiveSetElement & ); \
  virtual void    _visit ( MatrixLoadElement & ); \
  virtual void    _visit ( MatrixModeElement & ); \
  virtual void    _visit ( MaterialElement & ); \
  virtual void    _visit ( ColorElement & )


/////////////////////////////////////////////////////////////////////////////
//
//  Forward declarations.
//
/////////////////////////////////////////////////////////////////////////////

namespace GSG
{
  class Node;
  class Group;
  class Camera;
  class Transform;
  class Lod;
  class Shape;
  class PrimitiveSetElement;
  class MatrixLoadElement;
  class MatrixModeElement;
  class MaterialElement;
  class ColorElement;
  class Path;
  namespace Detail { struct PushPop; };
};


/////////////////////////////////////////////////////////////////////////////
//
//  Visitor class.
//
/////////////////////////////////////////////////////////////////////////////

namespace GSG {


class GSG_CORE_EXPORT Visitor : public Referenced
{
public:

  GSG_DECLARE_LOCAL_TYPEDEFS ( Visitor, Referenced );

  // Callback class.
  struct GSG_CORE_EXPORT Callback : public Referenced
  {
    GSG_DECLARE_LOCAL_TYPEDEFS ( Callback, Referenced );
    virtual void operator () ( Visitor &v, Node &n ) = 0;
  protected:
    explicit Callback() : Referenced(){}
    Callback ( const Callback &cb ) : Referenced ( cb ){}
    virtual ~Callback();
  };

  // Set/get the visited-callbacks.
  const Callback *        preVisitCallback() const;
  Callback *              preVisitCallback();
  void                    preVisitCallback ( Callback *cb );
  const Callback *        postVisitCallback() const;
  Callback *              postVisitCallback();
  void                    postVisitCallback ( Callback *cb );

  // Reset the internal state.
  virtual void            reset();

  // Traverse the scene.
  void                    traverse ( Node & );

  // Get/set the viewport.
  const Viewport &        viewport() const;
  virtual void            viewport ( const Viewport &viewport );

protected:

  explicit Visitor();
  Visitor ( const Visitor &v );
  virtual ~Visitor();

  GSG_DECLARE_VISIT_NODE_FUNCTIONS;
  GSG_DECLARE_VISIT_ELEMENT_FUNCTIONS;

  // Call the callbacks if there are any.
  virtual void            _preVisit ( Node &n );
  virtual void            _postVisit ( Node &n );

  // Push/pop the node. These are called from each node's accept() function.
  void                    _pushNode ( Node *n );
  void                    _popNode();

  // Traverse the scene.
  virtual void            _preTraverse  ( Node & );
  virtual void            _traverse     ( Node & );
  virtual void            _traverse     ( Group & );
  virtual void            _postTraverse ( Node & );

  // Friends.
  friend struct Detail::PushPop;
  friend class Node;
  friend class Group;
  friend class Camera;
  friend class Transform;
  friend class Lod;
  friend class Shape;
  friend class PrimitiveSetElement;
  friend class MatrixLoadElement;
  friend class MatrixModeElement;
  friend class MaterialElement;
  friend class ColorElement;
  friend class Path;

private:

  Viewport _viewport;
  Path *_path;
  Callback::Ptr _preVisitCB;
  Callback::Ptr _postVisitCB;
};


}; // namespace GSG


/////////////////////////////////////////////////////////////////////////////
//
//  Macro to define empty visit functions.
//
/////////////////////////////////////////////////////////////////////////////

#define GSG_DEFINE_EMPTY_VISIT_FUNCTION(class_name,node_name) \
  void class_name::_visit ( node_name & ){}


/////////////////////////////////////////////////////////////////////////////
//
//  Macro to define all visit-node functions as empty.
//
/////////////////////////////////////////////////////////////////////////////

#define GSG_DEFINE_EMPTY_VISIT_NODE_FUNCTIONS(class_name) \
  GSG_DEFINE_EMPTY_VISIT_FUNCTION ( class_name, Node ); \
  GSG_DEFINE_EMPTY_VISIT_FUNCTION ( class_name, Group ); \
  GSG_DEFINE_EMPTY_VISIT_FUNCTION ( class_name, Camera ); \
  GSG_DEFINE_EMPTY_VISIT_FUNCTION ( class_name, Transform ); \
  GSG_DEFINE_EMPTY_VISIT_FUNCTION ( class_name, Lod ); \
  GSG_DEFINE_EMPTY_VISIT_FUNCTION ( class_name, Shape )


/////////////////////////////////////////////////////////////////////////////
//
//  Macro to define all visit-element functions as empty.
//
/////////////////////////////////////////////////////////////////////////////

#define GSG_DEFINE_EMPTY_VISIT_ELEMENT_FUNCTIONS(class_name) \
  GSG_DEFINE_EMPTY_VISIT_FUNCTION ( class_name, PrimitiveSetElement ); \
  GSG_DEFINE_EMPTY_VISIT_FUNCTION ( class_name, MatrixLoadElement ); \
  GSG_DEFINE_EMPTY_VISIT_FUNCTION ( class_name, MatrixModeElement ); \
  GSG_DEFINE_EMPTY_VISIT_FUNCTION ( class_name, MaterialElement ); \
  GSG_DEFINE_EMPTY_VISIT_FUNCTION ( class_name, ColorElement )


/////////////////////////////////////////////////////////////////////////////
//
//  Macro to define delegating visit functions.
//
/////////////////////////////////////////////////////////////////////////////

#define GSG_DEFINE_DELEGATING_VISIT_FUNCTION(class_name,function_name,node_name) \
  void class_name::_visit ( node_name &n ) { function_name ( n ); }


/////////////////////////////////////////////////////////////////////////////
//
//  Macro to define all visit-node functions as delegating.
//
/////////////////////////////////////////////////////////////////////////////

#define GSG_DEFINE_DELEGATING_VISIT_NODE_FUNCTIONS(class_name,function_name) \
  GSG_DEFINE_DELEGATING_VISIT_FUNCTION ( class_name, function_name, Node ); \
  GSG_DEFINE_DELEGATING_VISIT_FUNCTION ( class_name, function_name, Group ); \
  GSG_DEFINE_DELEGATING_VISIT_FUNCTION ( class_name, function_name, Camera ); \
  GSG_DEFINE_DELEGATING_VISIT_FUNCTION ( class_name, function_name, Transform ); \
  GSG_DEFINE_DELEGATING_VISIT_FUNCTION ( class_name, function_name, Lod ); \
  GSG_DEFINE_DELEGATING_VISIT_FUNCTION ( class_name, function_name, Shape )


/////////////////////////////////////////////////////////////////////////////
//
//  Macro to define all visit-element functions as delegating.
//
/////////////////////////////////////////////////////////////////////////////

#define GSG_DEFINE_DELEGATING_VISIT_ELEMENT_FUNCTIONS(class_name,function_name) \
  GSG_DEFINE_DELEGATING_VISIT_FUNCTION ( class_name, function_name, PrimitiveSetElement ); \
  GSG_DEFINE_DELEGATING_VISIT_FUNCTION ( class_name, function_name, MatrixLoadElement ); \
  GSG_DEFINE_DELEGATING_VISIT_FUNCTION ( class_name, function_name, MatrixModeElement ); \
  GSG_DEFINE_DELEGATING_VISIT_FUNCTION ( class_name, function_name, MaterialElement ); \
  GSG_DEFINE_DELEGATING_VISIT_FUNCTION ( class_name, function_name, ColorElement )

#endif // _GENERIC_SCENE_GRAPH_CORE_VISITOR_H_
