
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The "bin" of shapes that will be drawn.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_RENDER_BIN_H_
#define _GENERIC_SCENE_GRAPH_CORE_RENDER_BIN_H_

#include "GSG/Core/RenderElement.h"


namespace GSG {


class GSG_CORE_EXPORT RenderBin : public Referenced
{
public:

  GSG_DECLARE_REFERENCED ( RenderBin );
  GSG_DECLARE_LOCAL_TYPEDEFS ( RenderBin, Referenced );
  typedef GSG_BIDIRECTIONAL_CONTAINER < RenderElement::ValidPtr > Elements;
  typedef Elements::size_type size_type;
  typedef Elements::iterator iterator;
  typedef Elements::const_iterator const_iterator;
  typedef Elements::reference reference;
  typedef Elements::const_reference const_reference;

  explicit RenderBin();
  RenderBin ( const RenderBin &group );

  // Iterators to the elements.
  const_iterator                begin() const;
  iterator                      begin();
  const_iterator                end() const;
  iterator                      end();

  // Add an element.
  void                          add ( RenderElement *e );

  // Clear the bin.
  void                          clear();

  // Number of elements.
  size_type                     size() const { return _elements.size(); }

protected:

  virtual ~RenderBin();

  Elements _elements;
};


}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_RENDER_BIN_H_
