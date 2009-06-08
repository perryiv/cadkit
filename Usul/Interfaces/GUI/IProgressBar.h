
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for updating the progress bar
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_PROGRESS_BAR_H_
#define _USUL_INTERFACE_PROGRESS_BAR_H_

#include "Usul/Interfaces/IUnknown.h"

#include "Usul/Interfaces/GUI/IFlushEvents.h"

namespace Usul {
namespace Interfaces {


struct IProgressBar : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IProgressBar );

  // Id for this interface.
  enum { IID = 1100747838u };

  // Show the progress bar
  virtual void showProgressBar() = 0;

  // Set the total of progress bar
  virtual void setTotalProgressBar ( unsigned int value ) = 0;

  // Update the progress bar
  virtual void updateProgressBar ( unsigned int value ) = 0;

  // Hide the progress bar
  virtual void hideProgressBar() = 0;

  // Helper struct to make sure we show/hide.
  struct ShowHide
  {
    template < class T > ShowHide ( T *t, unsigned int maximum = 100 ) : _progress ( t )
    {
      if ( _progress.valid() )
      {
        _progress->updateProgressBar ( 0 );
        _progress->setTotalProgressBar ( maximum );
        _progress->showProgressBar();
      }
    }
    ~ShowHide()
    {
      if ( _progress.valid() )
        _progress->hideProgressBar();
    }

  private:
    IProgressBar::QueryPtr _progress;
  };


  //  Helper to update the progress bar.
  struct UpdateProgressBar
  {
    template < class T > UpdateProgressBar( double start, double finish, T *t, bool hideWhenFinished = true ) :
    _start( start ),
    _finish ( finish ),
    _progressBar ( t ),
    _flush( t ),
    _hideWhenFinished ( hideWhenFinished )
    {
      //Show the progress bar if we have one
      if( _progressBar.valid() )
      {
        _progressBar->updateProgressBar ( 0 );
        _progressBar->setTotalProgressBar ( 100 );
        _progressBar->showProgressBar();
      }
    }

   ~UpdateProgressBar()
    {
      //Hide the progress bar if we have one
      if( _progressBar.valid() && _hideWhenFinished )
        _progressBar->hideProgressBar();
    }

    //Send an update to the progress bar
    void operator () ( unsigned int current, unsigned int total ) const
    {
      double percent ( (double) current / total );
      percent *= _finish - _start;
      percent += _start;

      if( _progressBar.valid() )
      {
        //Update the progress bar
        _progressBar->updateProgressBar ( static_cast < unsigned int > ( percent * 100 ) );

        //Flush the event queue.  Makes application responsive
        if ( _flush.valid() )
          _flush->flushEventQueue();
      }
    }

  private:
    double _start;
    double _finish;

    mutable IProgressBar::QueryPtr                   _progressBar;
    mutable Usul::Interfaces::IFlushEvents::QueryPtr _flush;
    bool _hideWhenFinished;
  };
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_PROGRESS_BAR_H_
