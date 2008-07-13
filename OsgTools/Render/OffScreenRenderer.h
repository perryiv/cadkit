
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class to render to off screen target, without needed a window.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_RENDER_OFF_SCREEN_RENDERER_H__
#define __OSG_TOOLS_RENDER_OFF_SCREEN_RENDERER_H__

#include "OsgTools/Export.h"
#include "OsgTools/Render/FrameDump.h"
#include "OsgTools/Render/SceneManager.h"

#include "Usul/Base/Object.h"
#include "Usul/Math/Vector2.h"

#include "osg/ref_ptr"
#include "osgViewer/Viewer"

#include "boost/shared_ptr.hpp"

namespace OsgTools {
namespace Render {


class OSG_TOOLS_EXPORT OffScreenRenderer : public Usul::Base::Object
{
public:
  typedef Usul::Base::Object BaseClass;
  typedef OsgTools::Render::FrameDump FrameDump;
  typedef boost::shared_ptr<FrameDump> FrameDumpPtr;

  USUL_DECLARE_REF_POINTERS ( OffScreenRenderer );

  OffScreenRenderer();

  // Initialize.
  void           init ( unsigned int width, unsigned int height );

  // Set/get the frame dump class.
  void           frameDump ( FrameDumpPtr frameDump );
  FrameDumpPtr   frameDump() const;
  
  // Set look at.
  void           lookAt ( double ex, double ey, double ez, double cx, double cy, double cz, double ux, double uy, double uz );

  // Set the model.
  void           model ( osg::Node* );

  // Render.
  void           render() const;

protected:
  
  virtual ~OffScreenRenderer();

private:

  osg::ref_ptr<osgViewer::Viewer> _viewer;
  OsgTools::Render::SceneManager::RefPtr _sceneManager;
  FrameDumpPtr _frameDump;
  Usul::Math::Vec2ui _size;
  osg::ref_ptr<osg::Image> _image;
};


}
}

#endif // __OSG_TOOLS_RENDER_OFF_SCREEN_RENDERER_H__
