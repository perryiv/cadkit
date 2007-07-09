
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_ISNAP_SHOT_H__
#define __USUL_INTERFACES_ISNAP_SHOT_H__

#include "Usul/Interfaces/IUnknown.h"

#include <string>

namespace Usul {
namespace Interfaces {


struct ISnapShot : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ISnapShot );

  /// Id for this interface.
  enum { IID = 2481220240u };

  virtual void                  takePicture ( const std::string& filename, float frameSizeScale, unsigned int numSamples ) = 0;
};

}
}

#endif // __USUL_INTERFACES_ISNAP_SHOT_H__
