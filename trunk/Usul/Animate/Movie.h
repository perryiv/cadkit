///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_ANIMATE_MOVIE_H__
#define __USUL_ANIMATE_MOVIE_H__


namespace Usul
{
  namespace Animate
  {
    template < class Player > class Movie
    {
    public:
      typedef typename Player::Frames Sequence;
      typedef typename Player::Frame Frame;

      Movie () : _frames() { }

      void setPlayer ( Player *player ) { _player = player; }

      void append ( Frame &frame ) { _frames.push_back ( frame ); }

      void play () { (*_player) ( _frames ); }

    private:
      Sequence _frames;
      Player *_player;
    }; // Movie

  }; // namespace Animate

}; // namespace Usul

#endif //__USUL_ANIMATE_MOVIE_H__

