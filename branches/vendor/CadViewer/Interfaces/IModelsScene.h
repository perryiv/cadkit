
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Access to the model's scene interface.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CV_INTERFACE_MODELS_SCENE_H_
#define _CV_INTERFACE_MODELS_SCENE_H_

#include "Usul/Interfaces/IUnknown.h"

namespace osg { class Group; };


namespace CV {
namespace Interfaces {


struct IModelsScene : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IModelsScene );

  /// Id for this interface.
  enum { IID = 1076038112 };

  // Get the model's scene.
  virtual const osg::Group *    modelsScene() const = 0;
  virtual osg::Group *          modelsScene() = 0;
};


}; // namespace Interfaces
}; // namespace CV


#endif // _CV_INTERFACE_MODELS_SCENE_H_
