
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Adapts events for OSG.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OPEN_SCENE_GRAPH_TOOLS_EVENT_ADAPTER_H_
#define _OPEN_SCENE_GRAPH_TOOLS_EVENT_ADAPTER_H_

#include "OsgTools/Export.h"

#include "Usul/Pointers/Pointers.h"
#include "Usul/Math/Vector2.h"

#include "osgGA/GUIEventAdapter"

#include "osg/Version"
#include "osg/ref_ptr"
#include "osg/Version"

namespace OsgTools {
namespace Render {


class OSG_TOOLS_EXPORT EventAdapter : public osgGA::GUIEventAdapter
{
public:

  // Useful typedefs.
  typedef osgGA::GUIEventAdapter BaseClass;
  typedef osg::ref_ptr< EventAdapter > Ptr;

  // Constructor.
  EventAdapter();

  void                    setButton     ( bool left, bool middle, bool right );

  // Set the properties.
  void                    setEventType  ( const EventType &type );
  void                    setWindowSize ( const Usul::Math::Vec2ui &size );
  void                    setMouse      ( const Usul::Math::Vec2f &xy );
  void                    setSeconds    ( double seconds );
  void                    setButtonMask ( bool left, bool middle, bool right );

protected:

  // Use reference counting.
  virtual ~EventAdapter();

};


} // namespace Render
} // namespace OsgTools


#endif // _OPEN_SCENE_GRAPH_TOOLS_EVENT_ADAPTER_H_
