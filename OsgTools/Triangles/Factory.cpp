
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV & Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Factory class.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#pragma warning ( disable : 4800 ) // Forcing value to bool 'true' or 'false'
#define WIN32_LEAN_AND_MEAN        // Exclude rarely-used stuff from Windows headers.
#define NOMINMAX                   // Do not define min and max as macros.
#endif

#include "OsgTools/Triangles/Factory.h"
#include "OsgTools/Triangles/SharedVertex.h"
#include "OsgTools/Triangles/Triangle.h"

#include "boost/pool/object_pool.hpp"


///////////////////////////////////////////////////////////////////////////////
//
//  Internal implementation class.
//
///////////////////////////////////////////////////////////////////////////////

namespace OsgTools
{
  namespace Triangles
  {
    class FactoryImpl
    {
    public:

      typedef boost::object_pool < SharedVertex > SharedVertexMemoryPool;
      typedef boost::object_pool < Triangle >     TriangleMemoryPool;

      FactoryImpl ( unsigned int sizeOfFirstChunk = 1024 ) : 
        _memorySV ( sizeOfFirstChunk ),
        _memoryT  ( sizeOfFirstChunk )
      {
      }

      ~FactoryImpl()
      {
      }

      SharedVertex *newSharedVertex ( unsigned int index, unsigned int numTrianglesToReserve )
      {
        SharedVertex *raw ( _memorySV.malloc() );
        SharedVertex::ValidRefPtr sv ( ( raw ) ? ( new ( raw ) SharedVertex ( index, numTrianglesToReserve, SharedVertex::MEMORY_POOL ) ) : 0x0 );
        return sv.release();
      }

      Triangle *newTriangle ( SharedVertex *v0, SharedVertex *v1, SharedVertex *v2, unsigned int index )
      {
        // Allocate raw memory.
        Triangle *raw ( _memoryT.malloc() );

        // Construct the triangle.
        Triangle::ValidRefPtr t ( ( raw ) ? ( new ( raw ) Triangle ( v0, v1, v2, index, Triangle::MEMORY_POOL ) ) : 0x0 );

        // Add new triangle to the shared vertices.
        v0->addTriangle ( t.get() );
        v1->addTriangle ( t.get() );
        v2->addTriangle ( t.get() );

        // Return new triangle.
        return t.release();
      }

    private:

      SharedVertexMemoryPool  _memorySV;
      TriangleMemoryPool      _memoryT;
    };
  }
}


using namespace OsgTools::Triangles;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Factory::Factory ( unsigned int sizeOfFirstChunk ) : _factory ( new FactoryImpl ( sizeOfFirstChunk ) )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Factory::~Factory()
{
  delete _factory;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return a new shared vertex.
//
///////////////////////////////////////////////////////////////////////////////

SharedVertex *Factory::newSharedVertex ( unsigned int index, unsigned int numTrianglesToReserve )
{
  return _factory->newSharedVertex ( index, numTrianglesToReserve );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return a new triangle.
//
///////////////////////////////////////////////////////////////////////////////

Triangle *Factory::newTriangle ( SharedVertex *v0, SharedVertex *v1, SharedVertex *v2, unsigned int index )
{
  return _factory->newTriangle ( v0, v1, v2, index );
}
