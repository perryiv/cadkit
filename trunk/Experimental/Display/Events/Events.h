
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Event classes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _DISPLAY_LIBRARY_EVENT_CLASSES_H_
#define _DISPLAY_LIBRARY_EVENT_CLASSES_H_

#include "Display/Export/Export.h"

#include "Usul/Types/Types.h"

#include <set>


namespace Display {
namespace Events {


///////////////////////////////////////////////////////////////////////////////
//
//  Base event class.
//
///////////////////////////////////////////////////////////////////////////////

struct DISPLAY_LIBRARY_EXPORT Event
{
  typedef Usul::Types::Uint64 TypeID;
  typedef std::set<int> KeysDown;
  typedef std::set<unsigned int> ButtonsDown;
  ~Event(){}
  static TypeID classType() { return 2571790917u; }
  virtual TypeID type() = 0;
  KeysDown keysDown() const { return _keys; }
  ButtonsDown buttonsDown() const { return _buttons; }
protected:
  Event ( const KeysDown &k, const ButtonsDown &b ) : _keys ( k ), _buttons ( b ){}
private:
  Event ( const Event & );
  Event &operator = ( const Event & );
  KeysDown _keys;
  ButtonsDown _buttons;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Macro to help with defining event classes.
//
///////////////////////////////////////////////////////////////////////////////

#define DISPLAY_EVENTS_EVENT_CLASS(type_id) \
  static TypeID classType() { return type_id; } \
  virtual TypeID type() { return type_id; }


///////////////////////////////////////////////////////////////////////////////
//
//  Mouse move event class.
//
///////////////////////////////////////////////////////////////////////////////

struct DISPLAY_LIBRARY_EXPORT MouseMove : public Event
{
  typedef Event BaseClass;
  MouseMove ( double x, double y, const KeysDown &k, const ButtonsDown &b ) : BaseClass ( k, b ), _x ( x ), _y ( y ){}
  ~MouseMove(){}
  double x() const { return _x; }
  double y() const { return _y; }
private:
  double _x;
  double _y;
  DISPLAY_EVENTS_EVENT_CLASS ( 1213457440u );
};


///////////////////////////////////////////////////////////////////////////////
//
//  Button press event class.
//
///////////////////////////////////////////////////////////////////////////////

struct DISPLAY_LIBRARY_EXPORT MousePress : public Event
{
  typedef Event BaseClass;
  MousePress ( unsigned int p, const KeysDown &k, const ButtonsDown &b ) : BaseClass ( k, b ), _button ( p ){}
  ~MousePress(){}
  unsigned int button() const { return _button; }
private:
  unsigned int _button;
  DISPLAY_EVENTS_EVENT_CLASS ( 7567653180u );
};


///////////////////////////////////////////////////////////////////////////////
//
//  Mouse release event class.
//
///////////////////////////////////////////////////////////////////////////////

struct DISPLAY_LIBRARY_EXPORT MouseRelease : public Event
{
  typedef Event BaseClass;
  MouseRelease ( unsigned int r, const KeysDown &k, const ButtonsDown &b ) : BaseClass ( k, b ), _button ( r ){}
  ~MouseRelease(){}
  unsigned int button() const { return _button; }
private:
  unsigned int _button;
  DISPLAY_EVENTS_EVENT_CLASS ( 1650497863u );
};


}; // namespace Events
}; // namespace Display


#endif // _DISPLAY_LIBRARY_EVENT_CLASSES_H_
