
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

#ifndef _USUL_INTERFACE_OPEN_SCENE_GRAPH_H_
#define _USUL_INTERFACE_OPEN_SCENE_GRAPH_H_

#include "Usul/Interfaces/IUnknown.h"

namespace osg { class Referenced; };


namespace Usul {
namespace Interfaces {


struct IOpenSceneGraph : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IOpenSceneGraph );

  /// Id for this interface.
  enum { IID = 2851842570u };

  /// Get the pointer to the base class for all OSG objects.
  //virtual const osg::Referenced *       osgReferenced() const = 0;
  virtual osg::Referenced *             osgReferenced() = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_OPEN_SCENE_GRAPH_H_
