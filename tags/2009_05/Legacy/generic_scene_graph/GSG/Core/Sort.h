
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The sorting class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_SORT_VISITOR_H_
#define _GENERIC_SCENE_GRAPH_CORE_SORT_VISITOR_H_

#include "GSG/Core/BinBuilder.h"


namespace GSG {


class GSG_CORE_EXPORT Sort : public BinBuilder
{
public:

  GSG_DECLARE_REFERENCED ( Sort );
  GSG_DECLARE_LOCAL_TYPEDEFS ( Sort, BinBuilder );

  explicit Sort ( RenderBin *bin = 0x0 );
  Sort ( const Sort &s );

protected:

  virtual ~Sort();
};


}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_SORT_VISITOR_H_
