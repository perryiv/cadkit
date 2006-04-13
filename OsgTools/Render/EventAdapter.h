
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
#include "osg/ref_ptr"

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

#if 0
  // Get the properties.
  virtual EventType       getEventType()  const { return _type; }
  virtual int             getKey()        const { return _key; }
  virtual int             getButton()     const { return _button; }
  virtual float           getXmin()       const { return 0.0f; }
  virtual float           getXmax()       const { return _size[0]; }
  virtual float           getYmin()       const { return 0.0f; }
  virtual float           getYmax()       const { return _size[1]; }
  virtual float           getX()          const { return _mouse[0]; }
  virtual float           getY()          const { return _mouse[1]; }
  virtual unsigned int    getButtonMask() const { return _buttons; }
  virtual unsigned int    getModKeyMask() const { return 0; }

  // Get the time of the event in seconds.
  virtual double          time() const { return _seconds; }
#endif

  // Set the properties.
  void                    setEventType  ( const EventType &type );
  void                    setWindowSize ( const Usul::Math::Vec2ui &size );
  void                    setMouse      ( const Usul::Math::Vec2f &xy );
  void                    setSeconds    ( double seconds );
  void                    setButtonMask ( bool left, bool middle, bool right );

protected:

  // Use reference counting.
  virtual ~EventAdapter();

#if 0
  EventType _type;
  int _key;
  int _button;
  Usul::Math::Vec2f _size;
  Usul::Math::Vec2f _mouse;
  unsigned int _buttons;
  double _seconds;
#endif
};


} // namespace Render
} // namespace OsgTools


#endif // _OPEN_SCENE_GRAPH_TOOLS_EVENT_ADAPTER_H_
