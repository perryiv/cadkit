
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
# ifndef NOMINMAX
#  define NOMINMAX                 // Do not define min and max as macros.
# endif
#endif

#include "OsgTools/Triangles/Factory.h"
#include "OsgTools/Triangles/SharedVertex.h"
#include "OsgTools/Triangles/Triangle.h"

#include "Usul/System/Memory.h"
#include "Usul/Resources/TextWindow.h"
#include "Usul/Memory/Pool.h"

#include <stdexcept>
#include <iostream>

using namespace OsgTools::Triangles;


///////////////////////////////////////////////////////////////////////////////
//
//  Functor to allocate a shared-vertex.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  struct NewSharedVertex
  {
    NewSharedVertex ( unsigned int index, unsigned int reserve ) :
      _index   ( index ),
      _reserve ( reserve )
    {
    }

    SharedVertex *operator () ( SharedVertex *raw ) const
    {
      return ( new ( raw ) SharedVertex ( _index, _reserve, SharedVertex::MEMORY_POOL ) );
    }

    SharedVertex * operator()() const
    {
      return ( new SharedVertex ( _index, _reserve ) );
    }

    unsigned int index() const { return _index; }

  private:

    unsigned int _index;
    unsigned int _reserve;
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Functor to allocate a triangle.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  struct NewTriangle
  {
    NewTriangle ( SharedVertex *v0, SharedVertex *v1, SharedVertex *v2, unsigned int index ) :
      _v0    ( v0 ),
      _v1    ( v1 ),
      _v2    ( v2 ),
      _index ( index )
    {
    }

    Triangle *operator () ( Triangle *raw ) const
    {
      return ( new ( raw ) Triangle ( _v0, _v1, _v2, _index, Triangle::MEMORY_POOL ) );
    }

    Triangle *operator()() const
    {
      return ( new Triangle ( _v0, _v1, _v2, _index ) );
    }

    unsigned int index() const { return _index; }

  private:

    SharedVertex *_v0;
    SharedVertex *_v1;
    SharedVertex *_v2;
    unsigned int _index;
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Generic pool allocator.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class ObjectType > struct PoolAllocator
  {
    template < class NewFunctor, class MemoryPool > static ObjectType *newObject ( const NewFunctor &nf, const char *name, MemoryPool &pool )
    {
      // Safely try to allocate from the pool.
      try
      {
        // Allocate some raw memory.
        ObjectType *raw ( pool.malloc() );

        // If it worked ...
        if ( raw )
        {
          // Allocate with placement new.
          ObjectType *object ( nf ( raw ) );

          // If that worked then return it.
          if ( object )
          {
            return object;
          }
        }
      }

      // Catch standard exceptions.
      catch ( const std::exception &e )
      {
        std::cout << "Error 2304217158: Standard exception caught when allocating " << name << ' ' << nf.index() << std::endl;
        if ( e.what() )
          std::cout << e.what() << std::endl;
      }

      // Catch all other exceptions.
      catch ( ... )
      {
        std::cout << "Error 3167899103: Unknown exception caught when allocating " << name << ' ' << nf.index() << std::endl;
      }

      // Print this every time.
      std::cout << "Error 1457727156: Allocation of " << name << ' ' << nf.index() << " on the memory pool failed.\n"
                << "Memory pool probably tried to allocate a continuous block of size " 
                << pool.nextSize() 
                << " bytes, but there are only " 
                << Usul::System::Memory::availablePhysical() 
                << " bytes available from the system's physical memory.\n"
                << "Will allocate from regular heap from now on."
                << Usul::Resources::TextWindow::endl;

      // Do not throw because the heap is used next if we return null.
      return 0x0;
    }
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Generic heap allocator.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class ObjectType > struct HeapAllocator
  {
    template < class NewFunctor > static ObjectType *newObject ( const NewFunctor &nf, const char *name )
    {
      // Safely try to allocate from the pool.
      try
      {
        // Allocate from the heap.
        ObjectType *object ( nf() );

        // If that worked then return it.
        if ( object )
        {
          return object;
        }
      }

      // Catch standard exceptions.
      catch ( const std::exception &e )
      {
        std::cout << "Error 2766578267: Standard exception caught when allocating " << name << ' ' << nf.index() << std::endl;
        if ( e.what() )
          std::cout << e.what() << std::endl;
      }

      // Catch all other exceptions.
      catch ( ... )
      {
        std::cout << "Error 2161268871: Unknown exception caught when allocating " << name << ' ' << nf.index() << std::endl;
      }

      // Print this every time.
      std::cout << "Error 2701762960: Allocation of " << name << ' ' << nf.index() << " on the heap failed.\n"
                << "There are " 
                << Usul::System::Memory::availablePhysical() 
                << " bytes available from the system's physical memory." << Usul::Resources::TextWindow::endl;

      // Memory allocation failed.
      std::ostringstream message;
      message << "Error 2727406310: Allocation of " << name << " failed";
      throw std::runtime_error ( message.str().c_str() );
    }
  };
}


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

      typedef Helper::PoolAllocator < SharedVertex > PoolAllocatorSV;
      typedef Helper::PoolAllocator < Triangle >     PoolAllocatorT;
      typedef Helper::HeapAllocator < SharedVertex > HeapAllocatorSV;
      typedef Helper::HeapAllocator < Triangle >     HeapAllocatorT;
      typedef Usul::Memory::Pool < SharedVertex > MemoryPoolSV;
      typedef Usul::Memory::Pool < Triangle >     MemoryPoolT;

      FactoryImpl ( unsigned int firstNum, unsigned int maxNum, float growthFactor ) : 
        _memorySV  ( firstNum, maxNum, growthFactor ),
        _memoryT   ( firstNum, maxNum, growthFactor ),
        _usePoolSV ( true ),
        _usePoolT  ( true )
      {
      }

      ~FactoryImpl()
      {
      }


      /////////////////////////////////////////////////////////////////////////
      //
      //  Turn on/off use of the memory pool.
      //
      /////////////////////////////////////////////////////////////////////////

      void usePool ( bool use )
      {
        _usePoolSV = use;
        _usePoolT  = use;
      }


      /////////////////////////////////////////////////////////////////////////
      //
      //  Return new shared-vertex.
      //
      /////////////////////////////////////////////////////////////////////////

      SharedVertex *newSharedVertex ( unsigned int index, unsigned int reserve )
      {
        // Initialize.
        SharedVertex *sv ( 0x0 );

        // Allocate from the pool if we should. This should return null if it fails.
        if ( _usePoolSV )
        {
          sv = PoolAllocatorSV::newObject ( Helper::NewSharedVertex ( index, reserve ), "shared vertex", _memorySV );
        }

        // Allocate from the heap if we should, or if the above failed.
        // This should throw if it fails.
        if ( 0x0 == sv )
        {
          _usePoolSV = false;
          sv = HeapAllocatorSV::newObject ( Helper::NewSharedVertex ( index, reserve ), "shared vertex" );
        }

        // Return new shared-vertex.
        return sv;
      }


      /////////////////////////////////////////////////////////////////////////
      //
      //  Return new triangle.
      //
      /////////////////////////////////////////////////////////////////////////

      Triangle *newTriangle ( SharedVertex *v0, SharedVertex *v1, SharedVertex *v2, unsigned int index )
      {
        // Initialize.
        Triangle *t ( 0x0 );

        // Allocate from the pool if we should. This should return null if it fails.
        if ( _usePoolT )
        {
          t = PoolAllocatorT::newObject ( Helper::NewTriangle ( v0, v1, v2, index ), "triangle", _memoryT );
        }

        // Allocate from the heap if we should, or if the above failed.
        // This should throw if it fails.
        if ( 0x0 == t )
        {
          _usePoolT = false;
          t = HeapAllocatorT::newObject ( Helper::NewTriangle ( v0, v1, v2, index ), "triangle" );
        }

        // Add new triangle to the shared vertices.
        v0->addTriangle ( t );
        v1->addTriangle ( t );
        v2->addTriangle ( t );

        // Return new triangle.
        return t;
      }


      /////////////////////////////////////////////////////////////////////////
      //
      //  Make space in the memory pool.
      //
      /////////////////////////////////////////////////////////////////////////

      void reserveTriangles ( unsigned int num )
      {
        if ( _usePoolT )
          _memoryT.reserve ( num );
      }


      /////////////////////////////////////////////////////////////////////////
      //
      //  Make space in the memory pool.
      //
      /////////////////////////////////////////////////////////////////////////

      void reserveSharedVertices ( unsigned int num )
      {
        if ( _usePoolSV )
          _memorySV.reserve ( num );
      }


      /////////////////////////////////////////////////////////////////////////
      //
      //  Return the next number to be allocated.
      //
      /////////////////////////////////////////////////////////////////////////

      unsigned int nextNumTriangles() const
      {
        return _memoryT.nextNum();
      }


      /////////////////////////////////////////////////////////////////////////
      //
      //  Set the next number to be allocated.
      //
      /////////////////////////////////////////////////////////////////////////

      void nextNumTriangles ( unsigned int num )
      {
        _memoryT.nextNum ( num );
      }


      /////////////////////////////////////////////////////////////////////////
      //
      //  Return the next number to be allocated.
      //
      /////////////////////////////////////////////////////////////////////////

      unsigned int nextNumSharedVertices() const
      {
        return _memorySV.nextNum();
      }


      /////////////////////////////////////////////////////////////////////////
      //
      //  Set the next number to be allocated.
      //
      /////////////////////////////////////////////////////////////////////////

      void nextNumSharedVertices ( unsigned int num )
      {
        _memorySV.nextNum ( num );
      }

    private:

      MemoryPoolSV _memorySV;
      MemoryPoolT _memoryT;
      bool _usePoolSV;
      bool _usePoolT;
    };
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Factory::Factory ( unsigned int firstNum, unsigned int maxNum, float growthFactor ) : BaseClass(),
  _factory ( new FactoryImpl ( firstNum, maxNum, growthFactor ) )
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


///////////////////////////////////////////////////////////////////////////////
//
//  Make space in the memory pool.
//
///////////////////////////////////////////////////////////////////////////////

void Factory::reserveTriangles ( unsigned int num )
{
  _factory->reserveTriangles ( num );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make space in the memory pool.
//
///////////////////////////////////////////////////////////////////////////////

void Factory::reserveSharedVertices ( unsigned int num )
{
  _factory->reserveSharedVertices ( num );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return next number.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Factory::nextNumTriangles() const
{
  return _factory->nextNumTriangles();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the next number.
//
///////////////////////////////////////////////////////////////////////////////

void Factory::nextNumTriangles ( unsigned int num ) const
{
  _factory->nextNumTriangles ( num );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return next number.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Factory::nextNumSharedVertices() const
{
  return _factory->nextNumSharedVertices();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the next number.
//
///////////////////////////////////////////////////////////////////////////////

void Factory::nextNumSharedVertices ( unsigned int num ) const
{
  _factory->nextNumSharedVertices ( num );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Turn on/off use of the memory pool.
//
///////////////////////////////////////////////////////////////////////////////

void Factory::usePool ( bool use )
{
  _factory->usePool ( use );
}
