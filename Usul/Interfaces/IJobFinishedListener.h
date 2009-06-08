
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IJOB_FINISHED_LISTENER_H__
#define __USUL_INTERFACES_IJOB_FINISHED_LISTENER_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul { namespace Jobs { class Job; } }

namespace Usul {
namespace Interfaces {
    
    
struct IJobFinishedListener : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IJobFinishedListener );
  
  /// Id for this interface.
  enum { IID = 3155290797u };
  
  /// The job has finished.
  virtual void                          jobFinished ( Usul::Jobs::Job *job ) = 0;
  
}; // struct IJobFinishedListener
    
    
} // End namespace Interfaces
} // End namespace Usul


#endif // __USUL_INTERFACES_IJOB_FINISHED_LISTENER_H__
