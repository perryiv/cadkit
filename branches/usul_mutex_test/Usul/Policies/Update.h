
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_POLICIES_UPDATE_H__
#define __USUL_POLICIES_UPDATE_H__

#include "Usul/System/Clock.h"
#include "Usul/Types/Types.h"

namespace Usul {
namespace Policies {

///////////////////////////////////////////////////////////////////////////////
//
//  Time based policy.
//
///////////////////////////////////////////////////////////////////////////////

struct TimeBased
{
  TimeBased ( Usul::Types::Uint64 updateTime ) : 
    _lastTime   ( Usul::System::Clock::milliseconds() ), 
    _updateTime ( updateTime )
  {
  }


  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Returns true if the time elapsed since last true is greater than given value
  //
  ///////////////////////////////////////////////////////////////////////////////

  bool operator() ()
  {
    //Get the current time
    Usul::Types::Uint64 currentTime ( Usul::System::Clock::milliseconds() );

    //Get the time elasped since true returned last
    Usul::Types::Uint64 elapsedTime ( currentTime - _lastTime );

    //Is the elapsed time greater than our update time
    if ( elapsedTime > _updateTime )
    {
      //We are going to return true, update last time
      _lastTime = currentTime;

      //Ok to update
      return true;
    }

    //If we get here return false
    return false;
  }


  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Set the time.
  //
  ///////////////////////////////////////////////////////////////////////////////

  void lastTime ( Usul::Types::Uint64 last )
  {
    _lastTime = last;
  }


  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Set the time.
  //
  ///////////////////////////////////////////////////////////////////////////////

  void updateTime ( Usul::Types::Uint64 update )
  {
    _updateTime = update;
  }

private:

  Usul::Types::Uint64 _lastTime;
  Usul::Types::Uint64 _updateTime;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Number based policy.
//
///////////////////////////////////////////////////////////////////////////////

struct NumberBased
{
  NumberBased ( unsigned int refreshRate ) :
    _refreshRate  ( refreshRate ),
    _numRefreshes ( 0 )
  {
  }

  
  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Returns true if the number of times function called is divisible by the
  //  given refresh rate
  //
  ///////////////////////////////////////////////////////////////////////////////

  bool operator () ()
  {
    ++_numRefreshes;

    //Can we update?
    if ( _numRefreshes % _refreshRate == 0 )
      return true;
    return false;
  }

private:

  unsigned int _refreshRate;
  unsigned int _numRefreshes;
};

}
}

#endif //__USUL_POLICIES_UPDATE_H__

