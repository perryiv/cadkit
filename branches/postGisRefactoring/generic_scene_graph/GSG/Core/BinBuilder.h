
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The base class for visitors that operate on a render bin.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_RENDER_BIN_BUILDER_VISITOR_H_
#define _GENERIC_SCENE_GRAPH_CORE_RENDER_BIN_BUILDER_VISITOR_H_

#include "GSG/Core/Visitor.h"
#include "GSG/Core/RenderBin.h"


namespace GSG {


class GSG_CORE_EXPORT BinBuilder : public Visitor
{
public:

  GSG_DECLARE_LOCAL_TYPEDEFS ( BinBuilder, Visitor );

  // Get/set the render-bin.
  const RenderBin *       renderBin() const;
  RenderBin *             renderBin();
  void                    renderBin ( RenderBin *bin );

  // Reset the internal state.
  virtual void            reset();

protected:

  explicit BinBuilder ( RenderBin *bin = 0x0 );
  BinBuilder ( const BinBuilder &r );
  virtual ~BinBuilder();

private:

  RenderBin::Ptr _bin;
};


}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_RENDER_BIN_BUILDER_VISITOR_H_
