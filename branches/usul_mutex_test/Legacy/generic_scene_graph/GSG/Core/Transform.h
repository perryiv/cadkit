
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A group with a transformation matrix.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_TRANSFORM_H_
#define _GENERIC_SCENE_GRAPH_CORE_TRANSFORM_H_

#include "GSG/Core/Group.h"


namespace GSG {

class Visitor;


class GSG_CORE_EXPORT Transform : public Group
{
public:

  GSG_DECLARE_REFERENCED ( Transform );
  GSG_DECLARE_LOCAL_TYPEDEFS ( Transform, Group );

  explicit Transform();
  Transform ( const Transform &t );

  // Get/set the matrix.
  const Matrix &          matrix() const { return _matrix; }
  void                    matrix ( const Matrix & );

protected:

  virtual ~Transform();

  GSG_DECLARE_ACCEPT ( Transform );

private:

  Matrix _matrix;
};


}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_TRANSFORM_H_
