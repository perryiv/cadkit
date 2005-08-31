
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Internal class used for adapting actions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OPEN_SCENE_GRAPH_TOOLS_ACTION_ADAPTER_H_
#define _OPEN_SCENE_GRAPH_TOOLS_ACTION_ADAPTER_H_

#include "OsgTools/Export.h"

#include "Usul/Pointers/Pointers.h"

#include "osgGA/GUIActionAdapter"


namespace OsgTools {
namespace Render {

class Viewer;

struct OSG_TOOLS_EXPORT ActionAdapter : public osgGA::GUIActionAdapter
{
  // Useful typedefs.
  typedef osgGA::GUIActionAdapter BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ActionAdapter );

  // Constructor/destructor.
  ActionAdapter ( Viewer & );
  ~ActionAdapter();

  // Request actions.
  virtual void requestRedraw();
  virtual void requestContinuousUpdate ( bool needed );
  virtual void requestWarpPointer ( float x, float y );

private:

  Viewer &_viewer;
};


} // namespace Render
} // namespace OsgTools


#endif // _OPEN_SCENE_GRAPH_TOOLS_ACTION_ADAPTER_H_
