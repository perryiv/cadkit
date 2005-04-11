
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_POLYGONS_SHARED_VERTEX_H__
#define __USUL_POLYGONS_SHARED_VERTEX_H__

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Bits/Bits.h"

#include <list>


namespace Usul {
namespace Polygons {


///////////////////////////////////////////////////////////////////////////////
//
//  Shared Vertex Class
//
///////////////////////////////////////////////////////////////////////////////

template < class Polygon, class ValueType_ >
class SharedVertex : public Usul::Base::Referenced
{
public:

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef typename std::list < Polygon * > PolygonList;
  typedef typename PolygonList::const_iterator ConstIterator;
  typedef typename PolygonList::iterator Iterator;
  typedef ValueType_ ValueType;

  enum
  {
    VISITED  = 0x01,
    ON_EDGE  = 0x02,
    DELETED  = 0x04,
    SELECTED = 0x08
  };

  // Smart pointers.
  USUL_DECLARE_REF_POINTERS ( SharedVertex );

  SharedVertex ( const ValueType &v ) : BaseClass(),
    _polygons(), 
    _value ( v ), 
    _flags ( 0x0 )
  {
  }

  // Append a polygon
  void append ( Polygon *p )
  {
    _polygons.push_back ( p );
  }

  //Iterator to the begining of the polygon list
  Iterator begin()
  {
    return _polygons.begin();
  }

  //Iterator to the end of the polygon list
  Iterator end()
  {
    return _polygons.end();
  }

  unsigned char flags () const
  {
    return _flags;
  }

  void flags ( unsigned char f )
  {
    _flags = f;
  }

  //Has this polygon been visited?
  bool visited() const
  {
    return Usul::Bits::has ( _flags, VISITED );
  }

  //Set the visited flag
  void visited ( bool v )
  {
    if( v )
      _flags = Usul::Bits::add ( _flags, VISITED );
    else
      _flags = Usul::Bits::remove ( _flags, VISITED );
  }

  bool onEdge() const
  {
    return Usul::Bits::has( _flags, ON_EDGE );
  }

  void onEdge( bool e )
  {
    if( e )
      _flags = Usul::Bits::add ( _flags, ON_EDGE );
    else
      _flags = Usul::Bits::remove ( _flags, ON_EDGE );
  }

  //Get the polygon list
  const PolygonList &polygons() const
  {
    return _polygons;
  }

  //Get the value of this shared vertex
  const ValueType &value() const
  {
    return _value;
  }

  //Set the value of this shared vertex
  void value ( const ValueType &v )
  {
    _value = v;
  }

  //Calculate the normal.  Not normalized
  ValueType normal() const
  {
    ValueType n ( 0, 0, 0 );
    for ( ConstIterator i = _polygons.begin(); i != _polygons.end(); ++i )
      n += (*i)->normal();
    return n;
  }

protected:

  // No default construction.
  SharedVertex();

  // No copying.
  SharedVertex ( const SharedVertex & );
  SharedVertex &operator = ( const SharedVertex & );

  // Use reference counting
  virtual ~SharedVertex()
  {
  } 

private:
  PolygonList _polygons;
  ValueType _value;
  unsigned char _flags;

}; //class SharedVertex



} // namespace Polygons
} // namespace Usul


#endif // __USUL_POLYGONS_SHARED_VERTEX_H__
