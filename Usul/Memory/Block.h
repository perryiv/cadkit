
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A single block of memory.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_LIBRARY_MEMORY_BLOCK_H_
#define _USUL_LIBRARY_MEMORY_BLOCK_H_

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"


namespace Usul {
namespace Memory {


///////////////////////////////////////////////////////////////////////////////
//
//  Memory block.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > class Block : public Usul::Base::Referenced
{
public:

  // Typedefs
  typedef Usul::Base::Referenced BaseClass;
  typedef T ValueType;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Block );


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Construction
  //
  /////////////////////////////////////////////////////////////////////////////

  Block ( unsigned int num ) : BaseClass(),
    _num   ( num ), 
    _one   ( sizeof ( ValueType ) ),
    _begin ( 0x0 ), 
    _next  ( 0x0 ), 
    _end   ( 0x0 )
  {
    const unsigned int total ( _num * _one );
    _begin = new unsigned char[total];
    _next = _begin;
    _end = _next + total;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return memory for an object. Increment pointer to the "next" available.
  //
  /////////////////////////////////////////////////////////////////////////////

  void *malloc()
  {
    if ( _next < _end )
    {
      void *address ( _next );
      _next += _one;
      return address;
    }
    else
    {
      return 0x0;
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return total number of objects this block can hold.
  //
  /////////////////////////////////////////////////////////////////////////////

  unsigned int numObjectsTotal() const
  {
    return _num;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return number of objects this block can still allocate.
  //
  /////////////////////////////////////////////////////////////////////////////

  unsigned int numObjectsAvailable() const
  {
    return ( _num - ( _next - _begin ) );
  }

protected:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Destruction. Use reference counting.
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual ~Block()
  {
    delete [] _begin;
  }

private:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  No copying.
  //
  /////////////////////////////////////////////////////////////////////////////

  Block ( const Block & );
  Block &operator = ( const Block & );


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Data members.
  //
  /////////////////////////////////////////////////////////////////////////////

  unsigned int _num;
  unsigned int _one;
  unsigned char *_begin;
  unsigned char *_next;
  unsigned char *_end;
};


} // namespace Memory
} // namespace Usul


#endif // _USUL_LIBRARY_MEMORY_BLOCK_H_
