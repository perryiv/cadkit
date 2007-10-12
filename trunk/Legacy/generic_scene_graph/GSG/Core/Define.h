
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  Definitions.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_DEFINITIONS_H_
#define _GENERIC_SCENE_GRAPH_CORE_DEFINITIONS_H_


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for declaring local types.
//
///////////////////////////////////////////////////////////////////////////////

#define GSG_DECLARE_LOCAL_TYPEDEFS(class_name,base_class_name) \
  typedef Pointer < class_name, NullOkPolicy >    Ptr; \
  typedef Pointer < class_name, NullCheckPolicy > ValidPtr; \
  typedef Pointer < const class_name, NullCheckPolicy, Detail::NoReferenceCounting > ConstValidNoRefPtr; \
  typedef base_class_name BaseClass


///////////////////////////////////////////////////////////////////////////////
//
//  Convenient macros for sub-classes.
//
///////////////////////////////////////////////////////////////////////////////

#define GSG_DECLARE_ACCEPT(class_name) \
  virtual void _accept ( Visitor &v ); \
  friend class Visitor

#define GSG_IMPLEMENT_ACCEPT_NODE(class_name) \
  void class_name::_accept ( Visitor &v ) \
  { \
    Lock lock ( this ); \
    Detail::PushPop pp ( v, this ); \
    v._preVisit ( *this ); \
    v._visit ( *this ); \
    v._postVisit ( *this ); \
  }

#define GSG_IMPLEMENT_ACCEPT_ELEMENT(class_name) \
  void class_name::_accept ( Visitor &v ) \
  { \
    v._visit ( *this ); \
  }


///////////////////////////////////////////////////////////////////////////////
//
//  To shorten the lines.
//
///////////////////////////////////////////////////////////////////////////////

#define GSG_PERMUTATION_ITERATOR(element_container,indices_container) \
  boost::permutation_iterator_generator \
  < \
    element_container::iterator, \
    indices_container::iterator \
  >::type

#define GSG_CONST_PERMUTATION_ITERATOR(element_container,indices_container) \
  boost::permutation_iterator_generator \
  < \
    element_container::const_iterator, \
    indices_container::const_iterator \
  >::type


#endif //_GENERIC_SCENE_GRAPH_CORE_DEFINITIONS_H_
