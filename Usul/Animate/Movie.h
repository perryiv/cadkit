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
      typedef typename Player::Frames Frames;
      typedef typename Frames::size_type size_type;
      typedef typename Frames::iterator iterator;
      typedef typename Frames::const_iterator const_iterator;
      typedef typename Player::Frame Frame;

      Movie () : _frames() { }

      bool empty() const { return _frames.empty(); }

      iterator        begin()       { return _frames.begin(); }
      const_iterator  begin() const { return _frames.begin(); }

      iterator        end()       { return _frames.end(); }
      const_iterator  end() const { return _frames.end(); }

      void setPlayer ( Player *player ) { _player = player; }

      void prepend ( Frame &frame ) { _frames.push_front ( frame ); }

      void append ( Frame &frame ) { _frames.push_back ( frame ); }

      void playForward () { _player->play(); _player->forward ( _frames ); }

      void playReverse () { _player->play(); _player->reverse ( _frames ); }

      void stop() { _player->stop(); }

      void clear () { _frames.clear(); }

      size_type size() const { return _frames.size(); }

      Frame remove ( unsigned int index )
      {
        if(index < 0 || index >= _frames.size())
        {
          //TODO throw an exception
        }
        for( Frames::iterator i = _frames.begin(), unsigned int j = 0; j != index; ++i, ++j);
        Frame frame = *i;
        _frames.erase(i);
        return frame;
      }

    private:
      Frames _frames;
      Player *_player;
    }; // Movie

  }; // namespace Animate

}; // namespace Usul

#endif //__USUL_ANIMATE_MOVIE_H__

