
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


namespace Usul {
namespace Interfaces {


struct IProgressBar : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IProgressBar );

  // Id for this interface.
  enum { IID = 1100747838u };

  struct NoUpdate
  {
    virtual ~NoUpdate () { }
    virtual void operator () ( unsigned int ) { }
  };

  //Functor for updating progress bar
  struct Update : public NoUpdate
  {
    Update( Usul::Interfaces::IProgressBar* progress, unsigned int init, unsigned int total ) :
    _progress ( progress )
    {
      _progress->showProgressBar();
      _progress->setTotalProgressBar( total );
      _progress->updateProgressBar ( init );
    }
    virtual ~Update ()
    {
      _progress->hideProgressBar();
    }

    virtual void operator() ( unsigned int value )
    {
      _progress->updateProgressBar( value );
    }
  private:
    Usul::Interfaces::IProgressBar* _progress;
  };

  //show the progress bar
  virtual void showProgressBar () = 0;
  //set the total of progress bar
  virtual void setTotalProgressBar ( unsigned int value ) = 0;
  // update the progress bar
  virtual void updateProgressBar ( unsigned int value ) = 0;
  //hide the progress bar
  virtual void hideProgressBar () = 0;
}; // struct IProgressBar

//TODO where is a good place for this?
namespace Helper {
//Helper function to get an update functor depending on if the caller implements IProgressBar
inline IProgressBar::NoUpdate* getUpdateFunctor ( Usul::Interfaces::IUnknown* caller )
{
  //The progress bar
  IProgressBar::QueryPtr progress ( caller );
  
  //if progressBar is valid set update functor
  if( progress.valid() )
    return new IProgressBar::Update ( progress.get(), 0, 100 );
  
  //Create a no update functor
  return new IProgressBar::NoUpdate;
}

} //namespace Helper

}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_PROGRESS_BAR_H_
