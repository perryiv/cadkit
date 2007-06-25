
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Authors: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_WIDGETS_HELPER_UPDATE_PROGRESS_H__
#define __OSG_TOOLS_WIDGETS_HELPER_UPDATE_PROGRESS_H__

#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/Guard.h"

#include "osg/Geometry"

namespace OsgTools {
namespace Widgets {
namespace Helper {

class UpdateProgressCallback : public osg::Drawable::UpdateCallback
{
};

class UpdateProgress : public osg::Geometry
{
public:
  typedef osg::Geometry BaseClass;
  typedef Usul::Threads::Mutex Mutex;
  typedef Usul::Threads::Guard< Mutex > Guard;

  UpdateProgress ();
  virtual ~UpdateProgress();
    
  void setBounds ( const osg::Vec2f& ul,  const osg::Vec2f& lr );

  virtual void drawImplementation ( osg::RenderInfo& info ) const;

private:
  Mutex        *_mutex;
  osg::Vec2f    _ul;
  osg::Vec2f    _lr;
};

}
}
}

#endif // __OSG_TOOLS_WIDGETS_HELPER_UPDATE_PROGRESS_H__
