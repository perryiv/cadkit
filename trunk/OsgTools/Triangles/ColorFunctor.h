
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functors for setting the color.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OPEN_SCENE_GRAPH_TOOLS_COLOR_FUNCTOR_CLASS_H_
#define _OPEN_SCENE_GRAPH_TOOLS_COLOR_FUNCTOR_CLASS_H_

#include "OsgTools/Export.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include "osg/Vec4f"

#include <string>


namespace OsgTools {
namespace Triangles {

class TriangleSet;
class Triangle;
class SharedVertex;


class OSG_TOOLS_EXPORT ColorFunctor : public Usul::Base::Referenced
{
public:

  // Useful typedefs.
  typedef Usul::Base::Referenced BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ColorFunctor );

  // Construction.
  ColorFunctor();

  // Get the name of this functor.
  virtual std::string   name() const;

  // Calculate the triangle color.
  virtual osg::Vec4f    color ( const TriangleSet *ts, const Triangle *t ) const;

  // Calculate the vertex color.
  virtual osg::Vec4f    color ( const TriangleSet *ts, const SharedVertex *sv ) const;

protected:

  // Do not copy.
  ColorFunctor ( const ColorFunctor & );
  ColorFunctor &operator = ( const ColorFunctor & );

  // Use reference counting.
  virtual ~ColorFunctor();
};


} // namespace Triangles
} // namespace OsgTools


#endif // _OPEN_SCENE_GRAPH_TOOLS_COLOR_FUNCTOR_CLASS_H_
