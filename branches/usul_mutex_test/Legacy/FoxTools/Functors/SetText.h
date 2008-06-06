
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functor for setting the text.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FOX_TOOLS_FUNCTORS_SET_TEXT_H_
#define _FOX_TOOLS_FUNCTORS_SET_TEXT_H_

#include <functional>
#include <string>

namespace FX { class FXObject; }


namespace FoxTools {
namespace Functors {


template < class T > struct SetText : public std::unary_function < const T *, void >
{
  SetText ( const std::string &t ) : _text ( t )
  {
  }
  SetText ( const SetText &t ) : _text ( t._text )
  {
  }
  ~SetText()
  {
  }
  SetText &operator = ( const SetText &t )
  {
    _text = t._text;
    return *this;
  }
  void operator () ( T *t ) const
  {
    if ( t )
      t->setText ( _text.c_str() );
  }
private:
  std::string _text;
};


} // namespace Functors
} // namespace FoxTools


#endif // _FOX_TOOLS_FUNCTORS_SET_TEXT_H_
