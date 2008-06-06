
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The base drawing class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_BASE_DRAW_VISITOR_H_
#define _GENERIC_SCENE_GRAPH_CORE_BASE_DRAW_VISITOR_H_

#include "GSG/Core/BinBuilder.h"
#include "GSG/Core/Color.h"


namespace GSG {


class GSG_CORE_EXPORT Draw : public BinBuilder
{
public:

  GSG_DECLARE_LOCAL_TYPEDEFS ( Draw, BinBuilder );

  // Set/get the background color.
  const Color &         backgroundColor() const;
  virtual void          backgroundColor ( const Color &background ) = 0;

protected:

  explicit Draw ( RenderBin *bin = 0x0 );
  Draw ( const Draw &r );
  virtual ~Draw();

private:

  Color _background;
};


}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_BASE_DRAW_VISITOR_H_
