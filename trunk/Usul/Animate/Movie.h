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

      void prepend ( Frame &frame ) { _frames.push_front ( frame ); }

      void append ( Frame &frame ) { _frames.push_back ( frame ); }

      void playForward () { _player->play(); _player->forward ( _frames ); }

      void playReverse () { _player->play(); _player->reverse ( _frames ); }

      void stop() { _player->stop(); }

      void clear () { _frames.clear(); }

      unsigned int size() { return _frames.size(); }

      Frame remove ( unsigned int index )
      {
        if(index < 0 || index >= _frames.size())
        {
          //TODO throw an exception
        }
        for( Sequence::iterator i = _frames.begin(), unsigned int j = 0; j != index; ++i, ++j);
        Frame frame = *i;
        _frames.erase(i);
        return frame;
      }

    private:
      Sequence _frames;
      Player *_player;
    }; // Movie

  }; // namespace Animate

}; // namespace Usul

#endif //__USUL_ANIMATE_MOVIE_H__

