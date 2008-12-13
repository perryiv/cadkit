
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

#include "Usul/Interfaces/IInputListener.h"

#include <map>
#include <vector>


namespace Display {
namespace View {


class DISPLAY_LIBRARY_EXPORT Viewer : public Canvas
{
public:

  // Typedefs.
  typedef Canvas BaseClass;
  typedef Usul::Interfaces::IInputListener IInputListener;
  typedef IInputListener::KeysDown KeysDown;
  typedef IInputListener::ButtonsDown ButtonsDown;
  typedef std::pair<KeysDown,ButtonsDown> InputState;
  typedef std::vector<IUnknown::RefPtr> ListenerSequence;
  typedef std::map<InputState,ListenerSequence> ListenerMap;

  // Type information.
  USUL_DECLARE_TYPE_ID ( Viewer );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Viewer );

  // Construction.
  Viewer ( IUnknown::RefPtr doc = IUnknown::RefPtr ( 0x0 ) );

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

  void                                _destroy();
};


}; // namespace View
}; // namespace Display


#endif // _DISPLAY_LIBRARY_VIEWER_CLASS_H_
