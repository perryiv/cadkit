///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_ANIMATE_LINEAR_PLAYER_H__
#define __USUL_ANIMATE_LINEAR_PLAYER_H__

#include "Usul/Animate/Player.h"

namespace Usul
{
  namespace Animate
  {
    template < class Sequence, class Callback > class LinearPlayer : public Player< Sequence, Callback >
    {
    public:
      typedef typename Sequence::value_type Frame;
      typedef typename Player< Sequence, Callback > BaseClass;

      LinearPlayer ( Callback cb ) : BaseClass( cb ) { }

      template < class Itr > _play ( Itr first, Itr second, Itr end)
      {
        while ( second != end )
        {
          if(_stop)
            break;
          _callback ( *first, *second );
          ++first;
          ++second;
        }
      }

      virtual void play() { _stop = false; }

      virtual void forward ( Sequence &frames )
      {
        if (frames.size() < 2 )
          return;
        _play ( frames.begin(), ++(frames.begin()), frames.end() );
      }

      virtual void reverse ( Sequence &frames )
      {
        if (frames.size() < 2 )
          return;
        _play ( frames.rbegin(), ++(frames.rbegin()), frames.rend() );
      }

      virtual void stop ( ) { _stop = true; }

      virtual void pause ( ) { }

    }; // Linear Player

  }; // namespace Animate

}; // namespace Usul

#endif //__USUL_ANIMATE_LINEAR_PLAYER_H__

