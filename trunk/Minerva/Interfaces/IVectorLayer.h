
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_INTERFACES_IVECTOR_LAYER_H__
#define __MINERVA_INTERFACES_IVECTOR_LAYER_H__

#include "Usul/Interfaces/IUnknown.h"

#include "Minerva/Core/Extents.h"

#include "osg/Image"
#include "osg/ref_ptr"
#include "osg/Vec2d"

namespace osg { class Node; }

namespace Minerva {
namespace Interfaces {


struct IVectorLayer : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IVectorLayer );

  /// Id for this interface.
  enum { IID = 2501255927u };

  typedef Minerva::Core::Extents<osg::Vec2d> Extents;
  typedef osg::ref_ptr<osg::Image> ImagePtr;
  
  /// Build the scene for data that is contained by the given extents.
  virtual osg::Node* buildTiledScene ( const Extents& extents, unsigned int level, ImagePtr elevationData, Usul::Interfaces::IUnknown * caller = 0x0 ) = 0;

}; // struct IVectorLayer


} // end namespace Interfaces
} // end namespace Minerva


#endif /* __MINERVA_INTERFACES_IVECTOR_LAYER_H__ */
