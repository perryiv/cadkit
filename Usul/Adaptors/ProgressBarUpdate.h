
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_PROGRESS_BAR_UPDATE_H__
#define __USUL_PROGRESS_BAR_UPDATE_H__

#include "Usul/Interfaces/IProgressBar.h" 
#include "Usul/Export/Export.h"

namespace Usul {

namespace Adaptors {

class USUL_EXPORT ProgressBarUpdate
{ 
public:
  ProgressBarUpdate( Usul::Interfaces::IUnknown *caller, unsigned int init = 0, unsigned int total = 100 );
  ~ProgressBarUpdate();

  void operator() ( unsigned int value ) const;
private:
  mutable Usul::Interfaces::IProgressBar::QueryPtr _progress;
};

}

}

#endif // __USUL_PROGRESS_BAR_UPDATE_H__