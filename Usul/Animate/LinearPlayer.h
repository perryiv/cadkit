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

      LinearPlayer ( Callback cb ) : Player( cb ) { }

      virtual void operator() ( Sequence &frames )
      {
        for( Sequence::const_iterator i = frames.begin(); i < frames.end() - 1; ++i)
          _callback ( *i, *(i + 1));
      }

    }; // Linear Player

  }; // namespace Animate

}; // namespace Usul

#endif //__USUL_ANIMATE_LINEAR_PLAYER_H__

