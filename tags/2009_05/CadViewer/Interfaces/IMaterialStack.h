
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for maintaining a material stack within the node's user-data.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CV_INTERFACE_MATERIAL_STACK_H_
#define _CV_INTERFACE_MATERIAL_STACK_H_

#include "Usul/Interfaces/IUnknown.h"

namespace osg { class Node; class Material; };


namespace CV {
namespace Interfaces {


struct IMaterialStack : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IMaterialStack );

  /// Id for this interface.
  enum { IID = 1075829504 };

  /// Pop the material.
  virtual void            popMaterial ( osg::Node *node ) = 0;

  /// Push the material.
  virtual void            pushMaterial ( const osg::Material *mat, osg::Node *node ) = 0;
};


}; // namespace Interfaces
}; // namespace CV


#endif // _CV_INTERFACE_MATERIAL_STACK_H_
