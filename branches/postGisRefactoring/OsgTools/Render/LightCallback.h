
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_RENDER_LIGHT_CALLBACK_H__
#define __OSG_TOOLS_RENDER_LIGHT_CALLBACK_H__

#include "OsgTools/Draggers/Callback.h"

#include "osg/ref_ptr"
#include "osg/Vec3"
#include "osg/Vec4"

namespace osg { class Light; }

namespace OsgTools {
namespace Render {

class LightCallback : public OsgTools::Draggers::Callback
{
public:
  typedef OsgTools::Draggers::Callback BaseClass;
  typedef OsgTools::Draggers::Dragger Dragger;

  LightCallback ( osg::Light *light );
  LightCallback ( const LightCallback & );

  // Execute the callback.
  virtual void operator() ( Dragger &d );
  
protected:

  // Use reference counting.
  virtual ~LightCallback();

private:

  osg::Vec4 _direction;
  osg::ref_ptr< osg::Light > _light;
};

}
}


#endif // __OSG_TOOLS_RENDER_LIGHT_CALLBACK_H__
