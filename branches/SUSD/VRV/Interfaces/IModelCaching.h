
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for turning on/of model caching. Model caching is simply the 
//  association of a filename with a previously loaded model. A read-request 
//  will first check the cache and see if the model has been loaded already, 
//  and clone it if it has.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VIRTUAL_REALITY_VIEWER_INTERFACE_MODEL_CACHING_H_
#define _VIRTUAL_REALITY_VIEWER_INTERFACE_MODEL_CACHING_H_

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Math/Matrix44.h"

#include <string>


namespace VRV {
namespace Interfaces {


struct IModelCaching : public Usul::Interfaces::IUnknown
{
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Usul::Math::Matrix44f Matrix;

  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IModelCaching );

  // Id for this interface.
  enum { IID = 2584330189u };

  // Get/set the model-caching mode.
  virtual bool getModelCachingMode() const = 0;
  virtual void setModelCachingMode ( bool mode ) = 0;
};


}; // namespace Interfaces
}; // namespace VRV


#endif // _VIRTUAL_REALITY_VIEWER_INTERFACE_MODEL_CACHING_H_
