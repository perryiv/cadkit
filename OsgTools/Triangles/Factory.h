
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


namespace OsgTools {
namespace Triangles {


class SharedVertex;
class Triangle;
class FactoryImpl;


class OSG_TOOLS_EXPORT Factory
{
public:

  Factory ( unsigned int sizeOfFirstChunk = 1024 );
  ~Factory();

  SharedVertex *          newSharedVertex ( unsigned int index, unsigned int numTrianglesToReserve = 0 );
  Triangle *              newTriangle ( SharedVertex *v0, SharedVertex *v1, SharedVertex *v2, unsigned int index );

private:

  FactoryImpl *_factory;
};


} // namespace Triangles
} // namespace OsgTools


#endif // _OPEN_SCENE_GRAPH_TOOLS_TRIANGLES_FACTORY_H_
