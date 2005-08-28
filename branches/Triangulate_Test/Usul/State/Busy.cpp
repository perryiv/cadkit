
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/State/Busy.h"

#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/Guard.h"

namespace Usul
{
  namespace State
  {
    typedef Usul::Threads::Mutex Mutex;
    typedef Usul::Threads::Guard<Mutex> Guard;

    namespace Detail
    {
      bool _busyState ( false );

      Mutex *_m ( 0x0 );

      Mutex& mutex()
      {
        if( 0x0 == _m )
          _m = Mutex::create();
        return *_m;
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the busy state.
//
///////////////////////////////////////////////////////////////////////////////

bool Usul::State::busy()
{
  Guard guard ( Usul::State::Detail::mutex() );
  bool result ( Usul::State::Detail::_busyState );
  return result;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set busy state to true.
//
///////////////////////////////////////////////////////////////////////////////

Usul::State::Busy::Busy ()
{
  Guard guard ( Usul::State::Detail::mutex() );
  Usul::State::Detail::_busyState = true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set busy state to false.
//
///////////////////////////////////////////////////////////////////////////////

Usul::State::Busy::~Busy (  )
{
  Guard guard ( Usul::State::Detail::mutex() );
  Usul::State::Detail::_busyState = false;
}

