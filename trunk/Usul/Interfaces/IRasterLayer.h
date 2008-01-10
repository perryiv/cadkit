
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

namespace osg { class Image; class Vec2d; }
namespace Usul { namespace Jobs { class Job; } }
namespace StarSystem { template < class VertexType> class Extents; }

namespace Usul {
namespace Interfaces {
    
    
struct IRasterLayer : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IRasterLayer );
      
  /// Id for this interface.
  enum { IID = 3791329730u };
  
  typedef StarSystem::Extents < osg::Vec2d > Extents;
  
  /// Get the texture.
  virtual osg::Image*      texture ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job * ) = 0;
  
}; // struct IRasterLayer
    
    
} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_IRASTER_LAYER_H__ */

