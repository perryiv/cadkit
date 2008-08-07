
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for time varying data
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_TIME_VARYING_DATA_H__
#define __USUL_INTERFACES_TIME_VARYING_DATA_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct ITimeVaryingData : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ITimeVaryingData );

  /// Id for this interface.
  enum { IID = 1282837521u };

  virtual void             setCurrentTimeStep ( unsigned int current ) = 0;
  virtual unsigned int     getCurrentTimeStep() const = 0;

  virtual unsigned int     getNumberOfTimeSteps() const = 0;
};


}
}

#endif // __USUL_INTERFACES_TIME_VARYING_DATA_H__
