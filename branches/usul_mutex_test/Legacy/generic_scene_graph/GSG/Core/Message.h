
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A message class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_MESSAGE_H_
#define _GENERIC_SCENE_GRAPH_CORE_MESSAGE_H_


namespace GSG {
namespace Detail {


template
<
  class FirstType_,
  class SecondType_,
  class ThirdType_,
  class FourthType_
>
struct Message
{
public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Useful typedefs.
  //
  /////////////////////////////////////////////////////////////////////////////

  typedef FirstType_  FirstType;
  typedef SecondType_ SecondType;
  typedef ThirdType_  ThirdType;
  typedef FourthType_ FourthType;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  explicit Message() :
    _first(),
    _second(),
    _third(),
    _fourth()
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Copy constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  Message ( const Message &m ) :
    _first  ( m._first ),
    _second ( m._second ),
    _third  ( m._third ),
    _fourth ( m._fourth )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Assignment.
  //
  /////////////////////////////////////////////////////////////////////////////

  Message &operator = ( const Message &m )
  {
    _first  = m._first;
    _second = m._second;
    _third  = m._third;
    _fourth = m._fourth;
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Access.
  //
  /////////////////////////////////////////////////////////////////////////////

  const FirstType &first() const
  {
    return _first;
  }
  FirstType &first()
  {
    return _first;
  }
  const SecondType &second() const
  {
    return _second;
  }
  SecondType &second()
  {
    return _second;
  }
  const ThirdType &third() const
  {
    return _third;
  }
  ThirdType &third()
  {
    return _third;
  }
  const FourthType &fourth() const
  {
    return _fourth;
  }
  FourthType &fourth()
  {
    return _fourth;
  }

protected:

  FirstType  _first;
  SecondType _second;
  ThirdType  _third;
  FourthType _fourth;
};


}; // namespace Detail
}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_MESSAGE_H_
