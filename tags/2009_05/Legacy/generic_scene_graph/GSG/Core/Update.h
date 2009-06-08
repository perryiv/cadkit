
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The base updating class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_UPDATE_VISITOR_H_
#define _GENERIC_SCENE_GRAPH_CORE_UPDATE_VISITOR_H_

#include "GSG/Core/Visitor.h"


namespace GSG {


class GSG_CORE_EXPORT Update : public Visitor
{
public:

  GSG_DECLARE_REFERENCED ( Update );
  GSG_DECLARE_LOCAL_TYPEDEFS ( Update, Visitor );

  explicit Update();
  Update ( const Update &r );

protected:

  virtual ~Update();

  void                    _update ( Node &n );
  void                    _update ( Group &g );

  GSG_DECLARE_VISIT_NODE_FUNCTIONS;
};


}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_UPDATE_VISITOR_H_
