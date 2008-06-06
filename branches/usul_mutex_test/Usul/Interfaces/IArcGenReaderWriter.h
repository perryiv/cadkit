
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Jeff Conner
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IARCGENREADERWRITER_H__
#define __USUL_INTERFACES_IARCGENREADERWRITER_H__

#include "Usul/Interfaces/IUnknown.h"

namespace osg { class Vec3f; typedef Vec3f Vec3; }

#include <vector>

namespace Usul {
namespace Interfaces {


struct IArcGenReaderWriter : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IArcGenReaderWriter );

  /// Id for this interface.
  enum { IID = 1605155574u };

  typedef std::vector< osg::Vec3 > Positions;

  // set the positions vector
  virtual void             setPolyLineVertices ( Positions p ) = 0;

  // set the recorded measurement value
  virtual void             measurement( double m ) = 0;


}; //struct IArcGenReaderWriter



}
}

#endif // __USUL_INTERFACES_IARCGENREADERWRITER_H__
