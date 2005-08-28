

///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Get and set pointer to the progress bar.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Resources/ProgressBar.h"
#include "Usul/Interfaces/IProgressBar.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Pointer to the progress bar.
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul
{
  namespace Resources
  {
    namespace Detail
    {
      Usul::Interfaces::IProgressBar::QueryPtr _progressBar;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get pointer to the progress bar. It may be null.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IProgressBar *Usul::Resources::progressBar()
{
  return Detail::_progressBar.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set pointer to the progress bar.
//
///////////////////////////////////////////////////////////////////////////////

void Usul::Resources::progressBar ( Usul::Interfaces::IProgressBar *progressBar )
{
  Detail::_progressBar = progressBar;
} 
