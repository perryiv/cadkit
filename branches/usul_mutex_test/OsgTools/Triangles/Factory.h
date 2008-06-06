
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Factory class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OPEN_SCENE_GRAPH_TOOLS_TRIANGLES_FACTORY_H_
#define _OPEN_SCENE_GRAPH_TOOLS_TRIANGLES_FACTORY_H_

#include "OsgTools/Export.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include <climits>


namespace OsgTools {
namespace Triangles {


class SharedVertex;
class Triangle;
class FactoryImpl;


class OSG_TOOLS_EXPORT Factory : public Usul::Base::Referenced
{
public:

  typedef Usul::Base::Referenced BaseClass;

  USUL_DECLARE_REF_POINTERS ( Factory );

  Factory ( unsigned int firstNum = 128, unsigned int maxNum = UINT_MAX, float growthFactor = 2.0f );

  SharedVertex *          newSharedVertex ( unsigned int index, unsigned int numTrianglesToReserve = 0 );
  Triangle *              newTriangle ( SharedVertex *v0, SharedVertex *v1, SharedVertex *v2, unsigned int index );

  void                    nextNumTriangles ( unsigned int ) const;
  unsigned int            nextNumTriangles() const;
  void                    nextNumSharedVertices ( unsigned int ) const;
  unsigned int            nextNumSharedVertices() const;

  void                    reserveTriangles ( unsigned int num );
  void                    reserveSharedVertices ( unsigned int num );

  void                    usePool ( bool );

protected:

  virtual ~Factory();

private:

  FactoryImpl *_factory;
};


} // namespace Triangles
} // namespace OsgTools


#endif // _OPEN_SCENE_GRAPH_TOOLS_TRIANGLES_FACTORY_H_
