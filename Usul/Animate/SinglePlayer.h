///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_ANIMATE_SINGLE_PLAYER_H__
#define __USUL_ANIMATE_SINGLE_PLAYER_H__

#include "Usul/Animate/Player.h"

namespace Usul
{
  namespace Animate
  {
    template < class Sequence, class Callback > class SinglePlayer : public Player < Sequence, Callback >
    {
    public:
      typedef typename Sequence::value_type Frame;
      typedef typename Player< Sequence, Callback > BaseClass;

      SinglePlayer ( Callback cb ) : BaseClass( cb ) { }

      virtual void play()  { }
      virtual void forward ( Sequence &frames ) { }
      virtual void reverse ( Sequence &frames ) { }
      virtual void stop () { }
      virtual void pause() { }

    }; // Single Player

  }; // namespace Animate

}; // namespace Usul

#endif //__USUL_ANIMATE_LINEAR_PLAYER_H__