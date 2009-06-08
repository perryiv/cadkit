
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Helper functions to clear the container.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MODFLOW_MODEL_CLEAR_CONTAINER_H_
#define _MODFLOW_MODEL_CLEAR_CONTAINER_H_


namespace Modflow {
namespace Tools {


template < class Sequence > struct Clear
{
  typedef typename Sequence::value_type ValueType;
  typedef typename Sequence::iterator Itr;

  static void pointers1D ( Sequence &s )
  {
    for ( Itr i = s.begin(); i != s.end(); ++i )
    {
      ValueType value ( *i );
      if ( true == value.valid() )
      {
        value->clear();
      }
    }
    s.clear();
  }

  static void pointers2D ( Sequence &s )
  {
    for ( Itr i = s.begin(); i != s.end(); ++i )
    {
      ValueType &value ( *i );
      Modflow::Tools::Clear<ValueType>::pointers1D ( value );
    }
    s.clear();
  }

  static void values1D ( Sequence &s )
  {
    for ( Itr i = s.begin(); i != s.end(); ++i )
    {
      ValueType &value ( *i );
      value.clear();
    }
    s.clear();
  }

  static void values2D ( Sequence &s )
  {
    for ( Itr i = s.begin(); i != s.end(); ++i )
    {
      ValueType &value ( *i );
      Modflow::Tools::Clear<ValueType>::values1D ( value );
    }
    s.clear();
  }
};


} // namespace Tools
} // namespace Modflow


#endif // _MODFLOW_MODEL_CLEAR_CONTAINER_H_
