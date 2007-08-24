
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Jeff Conner
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IMPDNAVIGATOR_H__
#define __USUL_INTERFACES_IMPDNAVIGATOR_H__

#include "Usul/Interfaces/IUnknown.h"

//namespace osg { class Vec4; }

namespace Usul {
namespace Interfaces {


struct IMpdNavigator : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IMpdNavigator );

  /// Id for this interface.
  enum { IID = 2186138810u };

  // go to the next group in the set
  virtual void              nextGroup ( unsigned int index ) = 0;

  


}; //struct IVertices



}
}

#endif // __USUL_INTERFACES_IMPDNAVIGATOR_H__
