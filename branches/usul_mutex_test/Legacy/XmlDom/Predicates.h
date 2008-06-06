
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Predicate classes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _XML_DOM_PREDICATE_CLASSES_H_
#define _XML_DOM_PREDICATE_CLASSES_H_

#include <functional>


namespace XML {
namespace Predicates {


/////////////////////////////////////////////////////////////////////////////
//
//  Predicate to test for an equal name.
//
/////////////////////////////////////////////////////////////////////////////

template < class StringType > struct EqualName : public std::binary_function < const StringType &, const StringType &, bool >
{
  EqualName ( const StringType &name ) : _name ( name ){}
  template < class PointerType_ > bool operator() ( const PointerType_ &p ) { return ( _name == p->name() ); }
private:
  StringType _name;
};


/////////////////////////////////////////////////////////////////////////////
//
//  Predicate to test for the i'th equal name.
//
/////////////////////////////////////////////////////////////////////////////

template < class StringType > struct IthEqualName : public std::binary_function < const StringType &, const StringType &, bool >
{
  IthEqualName ( const StringType &name, unsigned int i ) : _name ( name ), _i ( i ), _count ( 0 ){}
  template < class PointerType_ > bool operator() ( const PointerType_ &p )
  {
    if ( p->name() == _name )
    {
      if ( _i == _count++ )
      {
        return true;
      }
    }
    return false;
  }
private:
  StringType _name;
  unsigned int _i;
  unsigned int _count;
};


}; // namespace Predicates
}; // namespace XML


#endif // _XML_DOM_PREDICATE_CLASSES_H_
