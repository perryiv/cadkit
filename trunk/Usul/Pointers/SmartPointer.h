
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A smart pointer.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_POINTERS_SMART_POINTER_H_
#define _USUL_POINTERS_SMART_POINTER_H_

#include "Usul/Config/Config.h"

// We need to undefine "check" when compiling against Carbon on OS X.
#ifdef __OBJC__
#undef check
#endif

#ifdef __CARBON__
#undef check
#endif


namespace Usul {
namespace Pointers {


template
<
  class T,
  class Config_
>
struct SmartPointer
{
  /////////////////////////////////////////////////////////////////////////////
  //
  //  Useful typedefs.
  //
  /////////////////////////////////////////////////////////////////////////////

  typedef Config_ Config;
  typedef typename Config::AccessPolicy NullAccess;
  typedef typename Config::AssignmentPolicy AssignmentPolicy;
  typedef typename Config::ConstructorPolicy ConstructorPolicy;
  typedef typename Config::ReferencePolicy ReferencePolicy;
  typedef typename Config::DestructorPolicy DestructorPolicy;
  typedef T element_type;
  typedef SmartPointer < T, Config > ThisType;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Default constructor. Needed for containers.
  //
  /////////////////////////////////////////////////////////////////////////////

  SmartPointer() : _p ( 0x0 )
  {
    ConstructorPolicy::check ( _p );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  SmartPointer ( T *t ) : _p ( t )
  {
    ConstructorPolicy::check ( _p );
    if ( _p )
    {
      ReferencePolicy::ref ( _p );
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Copy constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  SmartPointer ( const ThisType &p ) : _p ( p.get() )
  {
    ConstructorPolicy::check ( _p );
    if ( _p )
    {
      ReferencePolicy::ref ( _p );
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Destructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  ~SmartPointer()
  {
    DestructorPolicy::check ( _p );
    if ( _p )
    {
      ReferencePolicy::unref ( _p );
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get internal pointer.
  //
  /////////////////////////////////////////////////////////////////////////////

  T *get() const
  {
    NullAccess::check ( _p );
    return _p;
  }

  
  /////////////////////////////////////////////////////////////////////////////
  //
  //  Typecast operators.
  //
  /////////////////////////////////////////////////////////////////////////////

  operator T *()
  {
    return this->get();
  }
  operator const T *() const
  {
    return this->get();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Dereference.
  //
  /////////////////////////////////////////////////////////////////////////////

  const T& operator * () const
  {
    return *(this->get());
  }
  T& operator * ()
  {
    return *(this->get());
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  For pointer syntax.
  //
  /////////////////////////////////////////////////////////////////////////////

  const T* operator -> () const
  {
    return this->get();
  }
  T* operator -> ()
  {
    return this->get();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Is the internal pointer valid?
  //
  /////////////////////////////////////////////////////////////////////////////

  bool valid() const
  {
    return 0x0 != _p;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Is the internal pointer null?
  //
  /////////////////////////////////////////////////////////////////////////////

  bool null() const
  {
    return 0x0 == _p;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  To permit "if ( !ptr )"
  //
  /////////////////////////////////////////////////////////////////////////////

  bool operator ! ()
  {
    return this->null();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Assignment.
  //
  /////////////////////////////////////////////////////////////////////////////

  ThisType &operator = ( const ThisType &p )
  {
    this->_set ( p.get() );
    return *this;
  }
  ThisType &operator = ( T *p )
  {
    this->_set ( p );
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Equality.
  //
  /////////////////////////////////////////////////////////////////////////////

  bool operator == ( const ThisType &p ) const
  {
    return _p == p.get();
  }
  bool operator == ( const T *p ) const
  {
    return _p == p;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Inequality.
  //
  /////////////////////////////////////////////////////////////////////////////

  bool operator != ( const ThisType &p ) const
  {
    return _p != p.get();
  }
  bool operator != ( const T *p ) const
  {
    return _p != p;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Release control of the internal pointer and return it.
  //
  /////////////////////////////////////////////////////////////////////////////

  T* release()
  {
    // Save a pointer.
    T* saved = _p;

    // If we have a valid pointer...
    if ( _p )
    {
      // Unreference it but make sure it is not deleted.
      ReferencePolicy::unrefNoDelete ( _p );

      // Make our internal pointer null;
      _p = 0x0;
    }

    // Return the saved pointer.
    return saved;
  }

  
  /////////////////////////////////////////////////////////////////////////////
  //
  //  Predicate to determine if another smart pointer is equal to this
  //
  /////////////////////////////////////////////////////////////////////////////

  struct IsEqual
  {
    IsEqual ( const element_type *p ) : _p ( p )
    {
    }
    template < class PointerType > bool operator() ( const PointerType &t ) const
    {
      return _p == t.get();
    }
  private:
    IsEqual();
    const element_type *_p;
  };


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Predicate to determine if two smart pointers are equal.
  //
  /////////////////////////////////////////////////////////////////////////////

  struct IsEqualBinary
  {
    IsEqualBinary ( )
    {
    }
    template < class PointerType > bool operator() ( const PointerType &lhs, const PointerType &rhs ) const
    {
      return lhs.get() == rhs.get();
    }
  };

protected:


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set the internal pointer.
  //
  /////////////////////////////////////////////////////////////////////////////

  void _set ( T *p )
  {
    // Check the policy.
    AssignmentPolicy::check ( p );

    // If the given pointer is the same address then just return.
    if ( _p == p )
    {
      return;
    }

    // Make a copy of the current one.
    T *old ( _p );

    // Copy the given pointer.
    _p = p;

    // If the given pointer is not null then reference it.
    if ( _p )
    {
      ReferencePolicy::ref ( _p );
    }

    // If the old one is not null then release it. Make sure we do this last 
    // because the given pointer "p" could be a child (indirect or direct) of 
    // "old". If we release "old" before we reference "p" then "p" may get 
    // deleted when it should not have.
    if ( old )
    {
      ReferencePolicy::unref ( old );
    }
  }


private:

  T *_p;
};


} // namespace Pointers
} // namespace Usul


///////////////////////////////////////////////////////////////////////////////
//
//  Additional operators. These are not members of the class because compilers
//  vary too much in the proper syntax for friend functions in templates. 
//  See http://gcc.gnu.org/faq.html#friend and http://www.bero.org/gcc296.html
//
///////////////////////////////////////////////////////////////////////////////

template < class T, class C > bool operator == ( const T *p1, const Usul::Pointers::SmartPointer<T,C> &p2 )
{
  return p1 == p2.get();
}
template < class T, class C > bool operator == ( T *p1, const Usul::Pointers::SmartPointer<T,C> &p2 )
{
  return p1 == p2.get();
}
template < class T, class C > bool operator != ( const T *p1, const Usul::Pointers::SmartPointer<T,C> &p2 )
{
  return p1 != p2.get();
}
template < class T, class C > bool operator != ( T *p1, const Usul::Pointers::SmartPointer<T,C> &p2 )
{
  return p1 != p2.get();
}


#endif // _USUL_POINTERS_SMART_POINTER_H_
