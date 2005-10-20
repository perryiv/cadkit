
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Memory pool.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_LIBRARY_MEMORY_POOL_H_
#define _USUL_LIBRARY_MEMORY_POOL_H_

#include "Usul/Memory/Block.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Math/MinMax.h"

#include <list>
#include <new> // For std::bad_alloc


namespace Usul {
namespace Memory {


///////////////////////////////////////////////////////////////////////////////
//
//  Memory pool.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > class Pool
{
public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Useful typedefs.
  //
  /////////////////////////////////////////////////////////////////////////////

  typedef T ValueType;
  typedef Usul::Memory::Block < ValueType > Block;
  typedef typename Block::ValidRefPtr BlockPtr;
  typedef std::list < BlockPtr > Blocks;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Default constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  Pool ( unsigned int firstNum, unsigned int maxNum, float growthFactor ) :
    _firstNum     ( firstNum ),
    _nextNum      ( firstNum ),
    _maxNum       ( maxNum ),
    _growthFactor ( growthFactor ),
    _blocks       ()
  {
    // Should be true.
    USUL_ASSERT ( _growthFactor > 1.0f );
    USUL_ASSERT ( _firstNum <= _maxNum );

    // Allocate the first block.
    this->_addBlock();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Destructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  ~Pool()
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Allocate an object.
  //
  /////////////////////////////////////////////////////////////////////////////

  ValueType *malloc()
  {
    // Should be true.
    USUL_ASSERT ( false == _blocks.empty() );

    // Try to allocate on the current block.
    void *address ( _blocks.back()->malloc() );

    // If it failed then add a block and try again.
    if ( 0x0 == address )
    {
      this->_addBlock();
      address = _blocks.back()->malloc();
    }

    // Return memory.
    return ( reinterpret_cast < ValueType * > ( address ) );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get next size.
  //
  /////////////////////////////////////////////////////////////////////////////

  unsigned int nextSize() const
  {
    return ( _nextNum * sizeof ( ValueType ) );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get next number.
  //
  /////////////////////////////////////////////////////////////////////////////

  unsigned int nextNum() const
  {
    return _nextNum;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set next number.
  //
  /////////////////////////////////////////////////////////////////////////////

  void nextNum ( unsigned int num )
  {
    _nextNum = num;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set the next number to the appropriate size.
  //
  /////////////////////////////////////////////////////////////////////////////

  void reserve ( unsigned int num )
  {
    _nextNum = num - _blocks.back()->numObjectsAvailable();
  }

protected:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Make another block.
  //
  /////////////////////////////////////////////////////////////////////////////

  void _addBlock()
  {
    while ( true )
    {
      // Safely...
      try
      {
        // Allocate the block.
        _blocks.push_back ( new Block ( _nextNum ) );

        // Set next number of blocks.
        _nextNum = ( Usul::Math::minimum ( _maxNum, static_cast < unsigned int > ( static_cast < float > ( _nextNum ) * _growthFactor ) ) );

        // We're done.
        return;
      }

      // Catch bad allocations.
      catch ( const std::bad_alloc & )
      {
        // If the smallest possible size failed then punt.
        if ( _nextNum == _firstNum )
        {
          throw;
        }

        // If we are not allowed to change size then punt.
        if ( _firstNum == _maxNum )
        {
          throw;
        }

        // Otherwise, try something smaller.
        _nextNum = _firstNum;
      }
    }
  }

private:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  No copying.
  //
  /////////////////////////////////////////////////////////////////////////////

  Pool ( const Pool & );
  Pool &operator = ( const Pool & );


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Data members.
  //
  /////////////////////////////////////////////////////////////////////////////

  unsigned int _firstNum;
  unsigned int _nextNum;
  unsigned int _maxNum;
  float _growthFactor;
  Blocks _blocks;
};


}; // namespace Memory
}; // namespace Usul


#endif // _USUL_LIBRARY_MEMORY_POOL_H_
