
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A reference-counting smart pointer.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_POINTER_H_
#define _GENERIC_SCENE_GRAPH_CORE_POINTER_H_


namespace GSG {


///////////////////////////////////////////////////////////////////////////////
//
//  Policy class that permits null pointers.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  struct NoNullChecker
  {
    template < class T > static void check ( const T *t ){}
  };
};


///////////////////////////////////////////////////////////////////////////////
//
//  The smart-pointer class.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class T,
  class NullPolicy_ = Detail::NoNullChecker
>
struct Pointer
{
  /////////////////////////////////////////////////////////////////////////////
  //
  //  Useful typedefs.
  //
  /////////////////////////////////////////////////////////////////////////////

  typedef NullPolicy_ NullPolicy;
  typedef T element_type;
  typedef Pointer < T, NullPolicy > ThisType;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Default constructor. Needed for containers.
  //
  /////////////////////////////////////////////////////////////////////////////

  Pointer() : _p ( 0x0 )
  {
    NullPolicy::check ( _p );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  Pointer ( T *t ) : _p ( t )
  {
    NullPolicy::check ( _p );
    if ( _p )
      Detail::intrusive_ptr_add_ref ( _p );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Copy constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  Pointer ( const ThisType &p ) : _p ( p._p )
  {
    NullPolicy::check ( _p );
    if ( _p )
      Detail::intrusive_ptr_add_ref ( _p );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Destructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  ~Pointer()
  {
    if ( _p )
      Detail::intrusive_ptr_release ( _p );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get internal pointer.
  //
  /////////////////////////////////////////////////////////////////////////////

  T *get() const
  {
    NullPolicy::check ( _p );
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
    this->_set ( p._p );
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
    return _p == p._p;
  }
  bool operator == ( const T *p ) const
  {
    return _p == p;
  }
  bool operator == ( T *p ) const
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
    return _p != p._p;
  }
  bool operator != ( const T *p ) const
  {
    return _p != p;
  }
  bool operator != ( T *p ) const
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
      Detail::ptr_release_no_delete ( _p );

      // Make our internal pointer null;
      _p = 0x0;
    }

    // Return the saved pointer.
    return saved;
  }


protected:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set the internal pointer.
  //
  /////////////////////////////////////////////////////////////////////////////

  void _set ( T *p )
  {
    // Check the policy.
    NullPolicy::check ( p );

    // If the given pointer is the same address then just return.
    if ( _p == p )
      return;

    // Make a copy of the current one.
    T *old = _p;

    // Copy the given pointer.
    _p = p;

    // If the given pointer is not null then reference it.
    if ( _p )
      Detail::intrusive_ptr_add_ref ( _p );

    // If the old one is not null then release it. Make sure we do this last 
    // because the given pointer "p" could be a child (indirect or direct) of 
    // "old". If we release "old" before we reference "p" then "p" may get 
    // deleted when it should not have.
    if ( old )
      Detail::intrusive_ptr_release ( old );
  }


private:

  T *_p;
};


}; // namespace GSG


///////////////////////////////////////////////////////////////////////////////
//
//  Additional operators. These are not members of the class because compilers
//  vary too much in the proper syntax for friend functions in templates. 
//  See http://gcc.gnu.org/faq.html#friend and http://www.bero.org/gcc296.html
//
///////////////////////////////////////////////////////////////////////////////

template < class T, class N > bool operator == ( const T *p1, const GSG::Pointer<T,N> &p2 )
{
  return p1 == p2.get();
}
template < class T, class N > bool operator == ( T *p1, const GSG::Pointer<T,N> &p2 )
{
  return p1 == p2.get();
}
template < class T, class N > bool operator != ( const T *p1, const GSG::Pointer<T,N> &p2 )
{
  return p1 != p2.get();
}
template < class T, class N > bool operator != ( T *p1, const GSG::Pointer<T,N> &p2 )
{
  return p1 != p2.get();
}


#endif // _GENERIC_SCENE_GRAPH_CORE_POINTER_H_
