
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Reference-counting base class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_REFERENCED_BASE_CLASS_H_
#define _USUL_REFERENCED_BASE_CLASS_H_

#include "Usul/Base/Typed.h"
#include "Usul/Strings/Format.h"

#include "Usul/Threads/Atomic.h"

#include <stdexcept>

namespace Usul { namespace Interfaces { struct IUnknown; } }


namespace Usul {
namespace Base {


class USUL_EXPORT Referenced : public Typed
{
public:

  typedef Typed BaseClass;

  USUL_DECLARE_TYPE_ID ( Referenced );

	/// Get this referenced as an IUnknown.  May return null.
	virtual Usul::Interfaces::IUnknown*   asUnknown();

  /// Reference and unreference the instance.
  void                        ref();
  void                        unref ( bool allowDeletion = true );

  /// Get the reference count.
  unsigned long               refCount() const;

protected:

  explicit Referenced();
  Referenced ( const Referenced &r );
  Referenced &operator = ( const Referenced &r );
  virtual ~Referenced();

private:

  Usul::Threads::Atomic<unsigned long> _refCount;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function for incrementing the referencers.
//
///////////////////////////////////////////////////////////////////////////////

template < class ReferencersType > inline void incrementReferencer ( ReferencersType &referencers, const void *caller )
{
  typedef ReferencersType Referencers;
  typedef typename Referencers::WriteLock WriteLock;
  typedef typename Referencers::ValueType ReferencersMap;
  typedef typename ReferencersMap::mapped_type MappedType;

  if ( 0x0 != caller )
  {
    WriteLock lock ( referencers.mutex() );
    MappedType &count ( referencers.getReference()[caller] );
    ++count;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function for decrementing the referencers.
//
///////////////////////////////////////////////////////////////////////////////

template < class ReferencersType > inline void decrementReferencer ( ReferencersType &referencers, const void *caller, const void *object )
{
  typedef ReferencersType Referencers;
  typedef typename Referencers::WriteLock WriteLock;
  typedef typename Referencers::ValueType ReferencersMap;
  typedef typename ReferencersMap::mapped_type MappedType;

  if ( 0x0 != caller )
  {
    WriteLock lock ( referencers.mutex() );
    ReferencersMap &r ( referencers.getReference() );
    typename ReferencersMap::iterator i ( r.find ( caller ) );
    if ( r.end() == i )
    {
      throw std::runtime_error ( Usul::Strings::format 
        ( "Error 1457414003: Caller ", caller, " has not referenced object ", object ) );
    }
    MappedType &count ( i->second );
    --count;
    if ( 0 == count )
    {
      r.erase ( i );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function for checking the referencers.
//
///////////////////////////////////////////////////////////////////////////////

template < class ReferencersType, class StreamType > inline void checkReferencers ( ReferencersType &referencers, const void *object, StreamType &stream )
{
  typedef ReferencersType Referencers;
  typedef typename Referencers::ReadLock ReadLock;
  typedef typename Referencers::ValueType ReferencersMap;
  typedef typename ReferencersMap::mapped_type MappedType;

  typename Referencers::ReadLock lock ( referencers.mutex() );
  ReferencersMap &r ( referencers.getReference() );
  if ( false == r.empty() )
  {
    typedef typename ReferencersMap::const_iterator Itr;
    std::ostringstream out;
    out << "Objects referencing " << object << ": ";
    for ( Itr i = r.begin(); i != r.end(); ++i )
    {
      out << i->first << " (" << i->second << ") ";
    }
    out << '\n';
    stream << out.str();
  }
}


} // namespace Base
} // namespace Usul


#endif // _USUL_REFERENCED_BASE_CLASS_H_
