
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functor that are adaptors.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_ADAPTOR_CLASSES_H_
#define _GENERIC_SCENE_GRAPH_CORE_ADAPTOR_CLASSES_H_


namespace GSG {
namespace Detail {


/////////////////////////////////////////////////////////////////////////////
//
//  Unfortunately, std::mem_fun and boost::mem_fun both rely on partial 
//  template specialization to accomodate member functions that return void. 
//  This is a work-around.
//
/////////////////////////////////////////////////////////////////////////////

template < class T > struct mem_fun_void_t : public std::unary_function < T *, void >
{
  explicit mem_fun_void_t ( void (T::*p)() ) : _ptr ( p ){}
  void operator() ( T *p ) const
  {
    (p->*_ptr)();
  }
private:
  void (T::*_ptr)();
};
template < class T > inline mem_fun_void_t<T> mem_fun_void ( void (T::*f)() )
{
  return mem_fun_void_t<T>(f);
}


/////////////////////////////////////////////////////////////////////////////
//
//  Same as above but for const member functions.
//
/////////////////////////////////////////////////////////////////////////////

template < class T > struct const_mem_fun_void_t : public std::unary_function < const T *, void >
{
  explicit const_mem_fun_void_t ( void (T::*p)() ) : _ptr ( p ){}
  void operator() ( const T *p ) const
  {
    (p->*_ptr)();
  }
private:
  void (T::*_ptr)() const;
};
template < class T > inline const_mem_fun_void_t<T> mem_fun_void ( void (T::*f)() const )
{
  return const_mem_fun_void_t<T>(f);
}


/////////////////////////////////////////////////////////////////////////////
//
//  Same as above but for member functions that take a single argument.
//
/////////////////////////////////////////////////////////////////////////////

template < class T, class A > struct mem_fun1_void_t : public std::binary_function < T *, A, void >
{
  explicit mem_fun1_void_t ( void (T::*p) ( A ) ) : _ptr ( p ){}
  void operator() ( T *p, typename boost::call_traits<A>::param_type x ) const
  {
    (p->*_ptr) ( x );
  }
private:
  void (T::*_ptr) ( A );
};
template < class T, class A > inline mem_fun1_void_t<T,A> mem_fun1_void ( void (T::*f) ( A ) )
{
  return mem_fun1_void_t<T,A>(f);
}


/////////////////////////////////////////////////////////////////////////////
//
//  Same as above but for const member functions that take a single argument.
//
/////////////////////////////////////////////////////////////////////////////

template < class T, class A > struct const_mem_fun1_void_t : public std::binary_function < const T *, A, void >
{
  explicit const_mem_fun1_void_t ( void (T::*p) ( A ) ) : _ptr ( p ){}
  void operator() ( const T *p, typename boost::call_traits<A>::param_type x ) const
  {
    (p->*_ptr) ( x );
  }
private:
  void (T::*_ptr)( A ) const;
};
template < class T, class A > inline const_mem_fun1_void_t<T,A> mem_fun_void ( void (T::*f) ( A ) const )
{
  return const_mem_fun1_void_t<T,A>(f);
}


}; // namespace Detail
}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_ADAPTOR_CLASSES_H_
