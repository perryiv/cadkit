///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, John K. Grant
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _osgtools_sample_h_
#define _osgtools_sample_h_

#include "osg/Node"
#include "Export.h"

namespace OsgTools
{

  class OSG_TOOLS_EXPORT Sample
  {
  public:
    Sample(): _scene(0x0) {}
    virtual ~Sample() {}

    osg::Node* scene()
    {
      if( !_scene.valid() )
        _scene = make_scene();  // create the scene

      return _scene.get();
    }

  protected:
    osg::Node* make_scene();

  private:
    osg::ref_ptr<osg::Node> _scene;
  };

};

#endif
