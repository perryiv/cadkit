
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
#include "osg/Version"

namespace OsgTools {
namespace Widgets {
namespace Helper {

///////////////////////////////////////////////////////////////////////////////
//
//  Callback to set the geometry of an osg::Geometry during an update traversal.
//
///////////////////////////////////////////////////////////////////////////////


class UpdateProgressCallback : public osg::Drawable::UpdateCallback
{
  public:
  typedef osg::Drawable::UpdateCallback   BaseClass;
  typedef Usul::Threads::Mutex            Mutex;
  typedef Usul::Threads::Guard < Mutex >  Guard;

  UpdateProgressCallback ( );

  virtual void update ( osg::NodeVisitor *nv, osg::Drawable* );

  // Set the bounds of the geometry
  void setBounds ( const osg::Vec2f& ul,  const osg::Vec2f& lr );

  // Set the geometry
  void setGeometry ( osg::Geometry * geometry );

  // Set the mutex.
  void mutex ( Mutex* mutex );

private:
  Mutex       *_mutex;
  osg::Vec2f    _ul;
  osg::Vec2f    _lr;
  bool         _dirty;

  osg::Geometry * _geometry;
};

///////////////////////////////////////////////////////////////////////////////
//
//  Class to update the geometry in a thread safe way.
//
///////////////////////////////////////////////////////////////////////////////

class UpdateProgress : public osg::Geometry
{
public:
  typedef osg::Geometry BaseClass;
  typedef Usul::Threads::Mutex Mutex;
  typedef Usul::Threads::Guard< Mutex > Guard;

  UpdateProgress ();
  virtual ~UpdateProgress();
    
  void setBounds ( const osg::Vec2f& ul,  const osg::Vec2f& lr );

#if OSG_VERSION_MAJOR <= 1 && OSG_VERSION_MINOR <= 2
  virtual void drawImplementation ( osg::State& state ) const;
#else
  virtual void drawImplementation ( osg::RenderInfo& info ) const;
#endif

private:
  Mutex        *_mutex;
  osg::ref_ptr < UpdateProgressCallback > _updateCallback;
};

}
}
}

#endif // __OSG_TOOLS_WIDGETS_HELPER_UPDATE_PROGRESS_H__
