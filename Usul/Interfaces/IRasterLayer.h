
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IRASTER_LAYER_H__
#define __USUL_INTERFACES_IRASTER_LAYER_H__

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Math/Vector2.h"

#include "osg/Image"
#include "osg/ref_ptr"
#include "osg/Vec2d"


namespace Usul { namespace Jobs { class Job; } }
namespace Minerva { namespace Core { template < class VertexType> class Extents; } }

namespace Usul {
namespace Interfaces {
    
    
struct IRasterLayer : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IRasterLayer );
      
  /// Id for this interface.
  enum { IID = 3791329730u };
  
  typedef Minerva::Core::Extents < osg::Vec2d > Extents;
  typedef osg::ref_ptr<osg::Image> ImagePtr;
  
  /// Get the texture.
  virtual ImagePtr        texture ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job *, Usul::Interfaces::IUnknown *caller ) = 0;
};
    
    
} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_IRASTER_LAYER_H__ */

