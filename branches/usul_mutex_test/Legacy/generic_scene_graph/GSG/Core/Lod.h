
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A group that displays one child.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_LEVEL_OF_DETAIL_H_
#define _GENERIC_SCENE_GRAPH_CORE_LEVEL_OF_DETAIL_H_

#include "GSG/Core/Group.h"


namespace GSG {

class Visitor;


class GSG_CORE_EXPORT Lod : public Group
{
public:

  GSG_DECLARE_REFERENCED ( Lod );
  GSG_DECLARE_LOCAL_TYPEDEFS ( Lod, Group );

  explicit Lod();
  Lod ( const Lod &l );

protected:

  virtual ~Lod();

  GSG_DECLARE_ACCEPT ( Lod );
};


}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_LEVEL_OF_DETAIL_H_
