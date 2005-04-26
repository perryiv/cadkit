///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, John K. Grant
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _osgtools_sample_h_
#define _osgtools_sample_h_

#include "OsgTools/Export.h"

#include "osg/Node"


namespace OsgTools
{

  /** Sample
    * The Sample class is a tool to provide a scene by default,
    * normally used with applications testing code other than
    * scene generation.  So, enjoy it.  By the way, it uses
    * geometry functors from the OsgTools library.
    */
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
