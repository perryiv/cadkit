
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A smart-pointer class
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _XML_POINTER_CLASS_H_
#define _XML_POINTER_CLASS_H_


namespace XML {


template < class ObjectType > class Pointer
{
public:

  Pointer() : _p ( 0x0 )
  {
  }
  Pointer ( ObjectType *p ) : _p ( p )
  {
    if ( _p )
      _p->ref();
  }
  Pointer ( const Pointer &p ) : _p ( p._p )
  {
    if ( _p )
      _p->ref();
  }
  Pointer &operator = ( const Pointer &p )
  {
    _p = p._p;
    if ( _p )
      _p->ref();
    return *this;
  }
  ~Pointer()
  {
    if ( _p )
      _p->unref();
  }
  const ObjectType *operator -> () const
  {
    return _p;
  }
  ObjectType *operator -> ()
  {
    return _p;
  }
  ObjectType *get()
  {
    return _p;
  }
  const ObjectType *get() const
  {
    return _p;
  }
  bool valid() const
  {
    return ( 0x0 != _p );
  }
private:
  ObjectType *_p;
};


}; // namespace XML


#endif // _XML_POINTER_CLASS_H_
