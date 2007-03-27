
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for direct access to OpenSceneGraph base class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SAL_INTERFACE_OSG_H_
#define _SAL_INTERFACE_OSG_H_

#include "Usul/Interfaces/IUnknown.h"

namespace osg { class Referenced; };


namespace SAL {
namespace Interfaces {


struct IOSG : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IOSG );

  /// Id for this interface.
  enum { IID = 1937449464 };

  /// Get the pointer to the base class for all OSG objects.
  virtual const osg::Referenced *       osgReferenced() const = 0;
  virtual osg::Referenced *             osgReferenced() = 0;
};


}; // namespace Interfaces
}; // namespace SAL


#endif // _SAL_INTERFACE_OSG_H_
