
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Traits.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_TRAITS_H_
#define _GENERIC_SCENE_GRAPH_CORE_TRAITS_H_


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for declaring iterator traits for pointer types. VC6 doesn't do this
//  for std::vector iterator types because it requires partial template
//  specialization.
//
///////////////////////////////////////////////////////////////////////////////

#define GSG_EXPLICIT_ITERATOR_TRAIT(T) \
namespace std \
{ \
  template <> struct iterator_traits < T * > \
  { \
    typedef random_access_iterator_tag iterator_category; \
    typedef T value_type; \
    typedef ptrdiff_t distance_type; \
    typedef ptrdiff_t difference_type; \
    typedef T* pointer; \
    typedef T& reference; \
  }; \
  template <> struct iterator_traits < const T * > \
  { \
    typedef random_access_iterator_tag iterator_category; \
    typedef T value_type; \
    typedef ptrdiff_t distance_type; \
    typedef ptrdiff_t difference_type; \
    typedef const T* pointer; \
    typedef const T& reference; \
  }; \
}


///////////////////////////////////////////////////////////////////////////////
//
//  Declare some iterator traits for pointer types. Only required on VC6 and 
//  when using std::vector. If not using std::vector then these will not hurt.
//
///////////////////////////////////////////////////////////////////////////////

#if ( _MSC_VER && _MSC_VER <= 1200 )
  GSG_EXPLICIT_ITERATOR_TRAIT ( GSG::Indices::value_type );
  GSG_EXPLICIT_ITERATOR_TRAIT ( GSG::SeqVec4::value_type );
  GSG_EXPLICIT_ITERATOR_TRAIT ( GSG::SeqVec3::value_type );
  GSG_EXPLICIT_ITERATOR_TRAIT ( GSG::SeqVec2::value_type );
#endif


#endif // _GENERIC_SCENE_GRAPH_CORE_TRAITS_H_
