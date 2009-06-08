
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for a component that builds a scene.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SAL_INTERFACE_BUILDER_H_
#define _SAL_INTERFACE_BUILDER_H_

#include "Usul/Interfaces/IUnknown.h"


namespace SAL {
namespace Interfaces {

struct INode;


struct IBuilder : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IBuilder );

  /// Id for this interface.
  enum { IID = 1282381398 };

  // Build the scene.
  virtual INode *         buildScene() const = 0;
};


}; // namespace Interfaces
}; // namespace SAL


#endif // _SAL_INTERFACE_BUILDER_H_
