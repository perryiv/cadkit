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

      virtual void operator() ( Sequence &frames )
      {
        if (frames.size() < 2 )
          return;
        Sequence::const_iterator frameA = frames.begin();
        Sequence::const_iterator frameB = frames.begin();
        ++frameB;
        while ( frameB != frames.end() )
        {
          _callback ( *frameA, *frameB );
          ++frameA;
          ++frameB;
        }
      }

    }; // Linear Player

  }; // namespace Animate

}; // namespace Usul

#endif //__USUL_ANIMATE_LINEAR_PLAYER_H__

