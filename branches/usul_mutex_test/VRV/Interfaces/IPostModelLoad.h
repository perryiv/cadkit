
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for post-processing a model load.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VRV_INTERFACE_MODEL_LOAD_POST_PROCESS_H_APPLICATION_H_
#define _VRV_INTERFACE_MODEL_LOAD_POST_PROCESS_H_APPLICATION_H_

#include "Usul/Interfaces/IUnknown.h"

#include <string>

namespace osg { class Node; };

namespace VRV {
namespace Interfaces {


struct IPostModelLoad : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IPostModelLoad );

  /// Id for this interface.
  enum { IID = 1075429803 };

  /// Post-process the model. This behavior is implementation defined.
  virtual void            postProcessModelLoad ( const std::string &filename, osg::Node *model ) = 0;
};


} // namespace Interfaces
} // namespace VRV


#endif // _VRV_INTERFACE_MODEL_LOAD_POST_PROCESS_H_APPLICATION_H_
