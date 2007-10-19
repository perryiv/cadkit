
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Jeff Conner
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IANIMATEPATH_H__
#define __USUL_INTERFACES_IANIMATEPATH_H__

#include "Usul/Interfaces/IUnknown.h"

namespace osg 
{  
  class Matrixf; 
}

namespace Usul {
namespace Interfaces {


struct IAnimatePath : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IAnimatePath );

  /// Id for this interface.
  enum { IID = 1614564854u };

  // Set an animation path from a vector of matrix keypoints
  virtual void                animatePath( std::vector< osg::Matrixf > ) = 0;

}; //struct IVertices



}
}

#endif // __USUL_INTERFACES_IANIMATEPATH_H__
