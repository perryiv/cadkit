
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A renderer (a sequence of visitors).
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_RENDERER_H_
#define _GENERIC_SCENE_GRAPH_CORE_RENDERER_H_

#include "GSG/Core/Visitor.h"


namespace GSG {


class GSG_CORE_EXPORT Renderer : public Visitor
{
public:

  GSG_DECLARE_CLONE ( Renderer );
  GSG_DECLARE_LOCAL_TYPEDEFS ( Renderer, Visitor );
  typedef GSG_SEQUENCE_CONTAINER_TYPE < Visitor::ValidPtr > Visitors;
  typedef Visitors::size_type size_type;
  typedef Visitors::iterator iterator;
  typedef Visitors::const_iterator const_iterator;

  explicit Renderer();
  Renderer ( const Renderer &Renderer );

  // Iterators to the visitors.
  const_iterator          begin() const;
  iterator                begin();
  const_iterator          end() const;
  iterator                end();

  // Add a visitor.
  void                    append ( Visitor *v );
  void                    prepend ( Visitor *v );
  void                    insert ( Visitors::iterator beforeMe, Visitor *v );

protected:

  virtual ~Renderer();

  // Traverse the scene.
  virtual void            _traverse ( Node & );

private:

  Visitors _visitors;
};


}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_RENDERER_H_
