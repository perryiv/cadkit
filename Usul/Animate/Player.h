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
      typedef Sequence Frames;
      typedef typename Frames::value_type Frame;
      typedef typename Frames::size_type size_type;
      typedef typename Frames::iterator iterator;

      Player ( Callback *cb ) : _callback (cb), _stop(false), _started(false) {}

      virtual void play() = 0;
      virtual void forward ( Sequence &frames ) = 0;
      virtual void reverse ( Sequence &frames ) = 0;
      virtual void stop () = 0;
      virtual void pause() = 0;

    protected:
      Callback *_callback;
      bool _stop, _started;
      iterator _start;
    }; // Linear Player

  }; // namespace Animate

}; // namespace Usul

#endif //__USUL_ANIMATE_LINEAR_PLAYER_H__