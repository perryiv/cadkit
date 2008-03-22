
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Callback filters.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _XML_DOM_CALLBACK_FILTERS_H_
#define _XML_DOM_CALLBACK_FILTERS_H_


namespace XML {
namespace Filter {


///////////////////////////////////////////////////////////////////////////////
//
//  Select the proper argument.
//
///////////////////////////////////////////////////////////////////////////////

struct Use1
{
  template < class F, class T1, class T2, class T3, class T4, class T5 >
  static void call ( F &f, const T1 &t1, const T2 &, const T3 &, const T4 &, const T5 & )
  {
    f ( t1 );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Select the proper argument.
//
///////////////////////////////////////////////////////////////////////////////

struct Use2
{
  template < class F, class T1, class T2, class T3, class T4, class T5 >
  static void call ( F &f, const T1 &, const T2 &t2, const T3 &, const T4 &, const T5 & )
  {
    f ( t2 );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Select the proper argument.
//
///////////////////////////////////////////////////////////////////////////////

struct Use12
{
  template < class F, class T1, class T2, class T3, class T4, class T5 >
  static void call ( F &f, const T1 &t1, const T2 &t2, const T3 &, const T4 &, const T5 & )
  {
    f ( t1, t2 );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Select the proper argument.
//
///////////////////////////////////////////////////////////////////////////////

struct Use13
{
  template < class F, class T1, class T2, class T3, class T4, class T5 >
  static void call ( F &f, const T1 &t1, const T2 &, const T3 &t3, const T4 &, const T5 & )
  {
    f ( t1, t3 );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Select the proper argument.
//
///////////////////////////////////////////////////////////////////////////////

struct Use4
{
  template < class F, class T1, class T2, class T3, class T4, class T5 >
  static void call ( F &f, const T1 &t1, const T2 &, const T3 &, const T4 &t4, const T5 & )
  {
    f ( t1, t4 );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Select the proper argument.
//
///////////////////////////////////////////////////////////////////////////////

struct Use123
{
  template < class F, class T1, class T2, class T3, class T4, class T5 >
  static void call ( F &f, const T1 &t1, const T2 &t2, const T3 &t3, const T4 &, const T5 & )
  {
    f ( t1, t2, t3 );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Select the proper argument.
//
///////////////////////////////////////////////////////////////////////////////

struct Use1234
{
  template < class F, class T1, class T2, class T3, class T4, class T5 >
  static void call ( F &f, const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 & )
  {
    f ( t1, t2, t3, t4 );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Select the proper argument.
//
///////////////////////////////////////////////////////////////////////////////

struct Use12345
{
  template < class F, class T1, class T2, class T3, class T4, class T5 >
  static void call ( F &f, const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5 )
  {
    f ( t1, t2, t3, t4, t5 );
  }
};


}; // namespace Filter
}; // namespace XML


#endif // _XML_DOM_CALLBACK_FILTERS_H_