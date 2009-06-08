
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for adding a model.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VIRTUAL_REALITY_VIEWER_INTERFACE_MODEL_ADD_H_
#define _VIRTUAL_REALITY_VIEWER_INTERFACE_MODEL_ADD_H_

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Math/Matrix44.h"

#include <string>

namespace osg { class Node; }

namespace VRV {
namespace Interfaces {


struct IModelAdd : public Usul::Interfaces::IUnknown
{
  typedef Usul::Interfaces::IUnknown Unknown;

  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IModelAdd );

  // Id for this interface.
  enum { IID = 3922695617u };

  // Add the new model.
  virtual void addModel ( osg::Node *model, const std::string& filename ) = 0;
};


} // namespace Interfaces
} // namespace VRV


#endif // _VIRTUAL_REALITY_VIEWER_INTERFACE_MODEL_ADD_H_
