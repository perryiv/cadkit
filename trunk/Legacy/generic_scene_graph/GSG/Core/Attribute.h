
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for a single attribute.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_STATE_ATTRIBUTE_H_
#define _GENERIC_SCENE_GRAPH_CORE_STATE_ATTRIBUTE_H_

#include "GSG/Core/Referenced.h"


namespace GSG {


class GSG_CORE_EXPORT Attribute : public Referenced
{
public:

  GSG_DECLARE_REFERENCED ( Attribute );
  GSG_DECLARE_LOCAL_TYPEDEFS ( Attribute, Referenced );

protected:

  explicit Attribute();
  Attribute ( const Attribute &a );
  virtual ~Attribute();
};


}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_STATE_ATTRIBUTE_H_
