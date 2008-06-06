
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Internal class used for adapting actions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OPEN_SCENE_GRAPH_ACTION_ADAPTER_H_
#define _OPEN_SCENE_GRAPH_ACTION_ADAPTER_H_

#include "OsgTools/Export.h"

#include "osgGA/GUIActionAdapter"

#include "Usul/Interfaces/ISpin.h"

namespace OsgTools {
namespace Render {


struct OSG_TOOLS_EXPORT ActionAdapter : public osgGA::GUIActionAdapter
{
  // Useful typedefs.
  typedef osgGA::GUIActionAdapter BaseClass;

  // Constructor/destructor.
  ActionAdapter ( Usul::Interfaces::IUnknown *caller );
  virtual ~ActionAdapter();

  // Request actions.
  virtual void requestRedraw();
  virtual void requestContinuousUpdate ( bool needed );
  virtual void requestWarpPointer ( float x, float y );

private:

  Usul::Interfaces::ISpin::QueryPtr _spin;
};


} // namespace Views
} // namespace Helios


#endif // _OPEN_SCENE_GRAPH_FOX_BINDING_ACTION_ADAPTER_H_
