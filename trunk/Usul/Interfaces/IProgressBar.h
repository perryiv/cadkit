
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
};


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_PROGRESS_BAR_H_
