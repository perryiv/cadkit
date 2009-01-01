
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Viewer class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _DISPLAY_LIBRARY_VIEWER_CLASS_H_
#define _DISPLAY_LIBRARY_VIEWER_CLASS_H_

#include "Display/View/Canvas.h"
#include "Display/Events/Events.h"

#include <map>
#include <vector>


namespace Display {
namespace View {


class DISPLAY_LIBRARY_EXPORT Viewer : public Canvas
{
public:

  // Typedefs.
  typedef Canvas BaseClass;
  typedef Display::Events::Event::TypeID EventType;
  typedef std::vector<IUnknown::RefPtr> ListenerSequence;
  typedef std::map<EventType,ListenerSequence> EventListeners;

  // Type information.
  USUL_DECLARE_TYPE_ID ( Viewer );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Viewer );

  // Construction.
  Viewer ( IUnknown::RefPtr doc = IUnknown::RefPtr ( 0x0 ) );

  // Add and remove listeners.
  void                  listenerAppend  ( EventType type, IUnknown::RefPtr );
  void                  listenerRemove  ( EventType type, IUnknown::RefPtr );
  void                  listenerPrepend ( EventType type, IUnknown::RefPtr );

  // Notification of an event.
  void                  notify ( Display::Events::Event &e );

  // Save/load the state.
  void                  stateLoad();
  void                  stateSave() const;

protected:

  // Use reference counting.
  virtual ~Viewer();

private:

  // No copying or assignment.
  Viewer ( const Viewer & );
  Viewer &operator = ( const Viewer & );

  void                  _destroy();

  void                  _notify ( Display::Events::Event &e );

  EventListeners _eventListeners;
};


}; // namespace View
}; // namespace Display


#endif // _DISPLAY_LIBRARY_VIEWER_CLASS_H_
