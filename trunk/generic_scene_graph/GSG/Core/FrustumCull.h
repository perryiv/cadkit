
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The frustum culling class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_FRUSTUM_CULL_VISITOR_H_
#define _GENERIC_SCENE_GRAPH_CORE_FRUSTUM_CULL_VISITOR_H_

#include "GSG/Core/BinBuilder.h"


namespace GSG {


class GSG_CORE_EXPORT FrustumCull : public BinBuilder
{
public:

  GSG_DECLARE_CLONE ( FrustumCull );
  GSG_DECLARE_LOCAL_TYPEDEFS ( FrustumCull, BinBuilder );

  explicit FrustumCull ( RenderBin *bin = 0x0 );
  FrustumCull ( const FrustumCull &c );

protected:

  virtual ~FrustumCull();

  void                    _cull ( Node &n );
  void                    _cull ( Shape &s );
  void                    _cull ( Group &g );

  GSG_DECLARE_VISIT_NODE_FUNCTIONS;
};


}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_FRUSTUM_CULL_VISITOR_H_
