
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
#include <string>


namespace XML {


/////////////////////////////////////////////////////////////////////////////
//
//  Predicate to test for an equal name.
//
/////////////////////////////////////////////////////////////////////////////

struct EqualName : public std::binary_function < const std::string &, const std::string &, bool >
{
  EqualName ( const std::string &name ) : _name ( name ){}
  template < class PointerType_ > bool operator() ( const PointerType_ &p ) { return ( _name == p->name() ); }
private:
  std::string _name;
};


/////////////////////////////////////////////////////////////////////////////
//
//  Predicate to test for the i'th equal name.
//
/////////////////////////////////////////////////////////////////////////////

struct IthEqualName : public std::binary_function < const std::string &, const std::string &, bool >
{
  IthEqualName ( const std::string &name, unsigned int i ) : _name ( name ), _i ( i ), _count ( 0 ){}
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
  std::string _name;
  unsigned int _i;
  unsigned int _count;
};


}; // namespace XML


#endif // _XML_DOM_PREDICATE_CLASSES_H_
