///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_ANIMATE_PLAYER_H__
#define __USUL_ANIMATE_PLAYER_H__

namespace Usul
{
  namespace Animate
  {
    template < class Sequence, class Callback > class Player
    {
    public:
      typedef typename Sequence::value_type Frame;
      typedef Sequence Frames;

      Player ( Callback cb ) : _callback (cb) {}

      virtual void operator() ( Sequence &frames ) = 0;

    protected:
      Callback _callback;
    }; // Linear Player

  }; // namespace Animate

}; // namespace Usul

#endif //__USUL_ANIMATE_LINEAR_PLAYER_H__