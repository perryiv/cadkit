
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlFindExtreme: Functors to find extreme values in containers.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_FIND_EXTREME_H_
#define _CADKIT_STANDARD_LIBRARY_FIND_EXTREME_H_

#include <functional>
#include <limits>


namespace CadKit {


///////////////////////////////////////////////////////////////////////////////
//
//  Functor used to find the extreme scalar in a container of objects.
//
///////////////////////////////////////////////////////////////////////////////

template < typename Type, class Compare > class SlFindExtreme
{
public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  SlFindExtreme ( const Type &extreme ) : _extreme ( extreme )
  {
    // Empty.
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Operator that takes as an argument the template parameter "Type".
  //
  /////////////////////////////////////////////////////////////////////////////

  void operator () ( Type value )
  {
    if ( _compare ( value, _extreme ) )
      _extreme = value;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Operator that takes a type other than "Type".
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class T > void operator () ( const T &t )
  {
    // We assume that type "T" has operator() defined. This should generate 
    // a compile-error if it does not.
    t ( *this );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return the extreme value.
  //
  /////////////////////////////////////////////////////////////////////////////

  const Type &getExtreme() const
  {
    return _extreme;
  }


private:

  Compare _compare;
  Type _extreme;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Functor used to find the maximum scalar in a container of objects.
//
///////////////////////////////////////////////////////////////////////////////

template < typename Type > struct SlFindMax : public SlFindExtreme < Type, std::greater<Type> >
{
  typedef SlFindExtreme < Type, std::greater<Type> > BaseClass;
  SlFindMax() : BaseClass ( -(std::numeric_limits<Type>::max()) ){}
  const Type &getMax() const { return this->getExtreme(); }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Functor used to find the minimum scalar in a container of objects.
//
///////////////////////////////////////////////////////////////////////////////

template < typename Type > struct SlFindMin : public SlFindExtreme < Type, std::less<Type> >
{
  typedef SlFindExtreme < Type, std::less<Type> > BaseClass;
  SlFindMin() : BaseClass ( std::numeric_limits<Type>::max() ){}
  const Type &getMin() const { return this->getExtreme(); }
};


}; // namespace CadKit


#endif // _CADKIT_STANDARD_LIBRARY_FIND_EXTREME_H_