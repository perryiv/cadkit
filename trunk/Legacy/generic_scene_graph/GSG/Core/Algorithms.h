
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Algorithms.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_ALGORITHMS_H_
#define _GENERIC_SCENE_GRAPH_CORE_ALGORITHMS_H_


namespace GSG {


/////////////////////////////////////////////////////////////////////////////
//
//  Select the parent type given the child type.
//
/////////////////////////////////////////////////////////////////////////////

template < class Child_ > struct ParentType;
template <> struct ParentType < Node >      { typedef Group Type; };
template <> struct ParentType < Primitive > { typedef Shape Type; };


/////////////////////////////////////////////////////////////////////////////
//
//  Select the type from the pair.
//
/////////////////////////////////////////////////////////////////////////////

template < class Pair_ > struct SelectFirst
{
  typedef typename Pair_::first_type Type;
  Type &operator () ( Pair_ &p )
  {
    return p.first;
  }
  const Type &operator () ( const Pair_ &p ) const
  {
    return p.first;
  }
};
template < class Pair_ > struct SelectSecond
{
  typedef typename Pair_::second_type Type;
  Type &operator () ( Pair_ &p )
  {
    return p.second;
  }
  const Type &operator () ( const Pair_ &p ) const
  {
    return p.second;
  }
};


/////////////////////////////////////////////////////////////////////////////
//
//  Compare the properties of the container's values.
//
/////////////////////////////////////////////////////////////////////////////

template
<
  class ValueType_, 
  class SelectProperty_, 
  class Comparison_
>
struct Compare : public std::unary_function < const ValueType_ &, bool >
{
  typedef typename ValueType_ ValueType;
  typedef typename SelectProperty_ SelectProperty;
  typedef typename SelectProperty::Type Property;
  typedef typename Comparison_ Comparison;

  Compare ( const Property &prop ) : _prop ( prop ){}

  bool operator () ( const ValueType &value ) const
  {
    const Property &p = SelectProperty () ( value );
    return ( Comparison () ( p, _prop ) );
  }

protected:

  Property _prop;
};


/////////////////////////////////////////////////////////////////////////////
//
//  Find the contained value given the predicate.
//
/////////////////////////////////////////////////////////////////////////////

template < class Iterator_, class Predicate_ > 
Iterator_ recursive_find_first ( Iterator_ first, Iterator_ last, const Predicate_ &pred )
{
  typedef Iterator_ Iterator;
  typedef typename std::iterator_traits<Iterator>::value_type pointer_type;
  typedef typename pointer_type::element_type object_type;
  typedef typename ParentType<object_type>::Type parent_type;

  while ( first != last )
  {
    pointer_type &ptr = *first;
    object_type &object = *ptr;

    // See if this is the object.
    if ( pred ( object ) )
    {
      // Return the iterator to this object.
      return first;
    }

    // See if this object is also the same type as its parent 
    // (e.g., Group or Shape).
    parent_type *composite = dynamic_cast < parent_type * > ( &object );
    if ( composite )
    {
      // Call this function for the children of this composite object.
      Iterator itr = recursive_find_first ( composite->begin(), composite->end(), pred );

      // Return the iterator if we found it.
      if ( composite->end() != itr )
      {
        return itr;
      }
    }

    // Try the next element.
    ++first;
  }

  // If we get to here then we did not find anything.
  return last;
}


}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_ALGORITHMS_H_
