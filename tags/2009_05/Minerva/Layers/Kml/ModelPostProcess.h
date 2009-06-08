
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Set proper texture state.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_KML_MODEL_POST_PROCESS_H__
#define __MINERVA_LAYERS_KML_MODEL_POST_PROCESS_H__

namespace osg { class Node; }

namespace Minerva {
namespace Layers {
namespace Kml {

  
  struct ModelPostProcess
  {
    void operator () ( osg::Node * node );
  };


}
}
}

#endif // __MINERVA_LAYERS_KML_MODEL_POST_PROCESS_H__
