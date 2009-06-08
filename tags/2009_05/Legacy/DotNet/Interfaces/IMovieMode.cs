using System;
using System.Collections.Generic;
using System.Text;

namespace CadKit.Interfaces
{
  public interface IMovieMode : IReferenced
  {
    void setMovieMode( bool val );

    void play();

    void pause();

    void restart();

    bool isPlaying();

    bool isPaused();
  }
}
