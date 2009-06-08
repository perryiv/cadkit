
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

class DISPLAY_LIBRARY_EXPORT Event
{
public:
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
  Event ( const Event &e ) : _keys ( e._keys ), _buttons ( e._buttons ){}
  Event &operator = ( const Event &e ) { _keys = e._keys; _buttons = e._buttons; return *this; }
private:
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

class DISPLAY_LIBRARY_EXPORT MouseMove : public Event
{
public:
  typedef Event BaseClass;
  MouseMove ( double x, double y, const KeysDown &k, const ButtonsDown &b ) : BaseClass ( k, b ), _x ( x ), _y ( y ){}
  MouseMove ( const MouseMove &e ) : BaseClass ( e ), _x ( e._x ), _y ( e._y ){}
  MouseMove &operator = ( const MouseMove &e ) { _x = e._x; _y = e._y; BaseClass::operator = ( e ); return *this; }
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

class DISPLAY_LIBRARY_EXPORT MousePress : public Event
{
public:
  typedef Event BaseClass;
  MousePress ( unsigned int p, const KeysDown &k, const ButtonsDown &b ) : BaseClass ( k, b ), _button ( p ){}
  MousePress ( const MousePress &e ) : BaseClass ( e ), _button ( e._button ){}
  MousePress &operator = ( const MousePress &e ) { _button = e._button; BaseClass::operator = ( e ); return *this; }
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

class DISPLAY_LIBRARY_EXPORT MouseRelease : public Event
{
public:
  typedef Event BaseClass;
  MouseRelease ( unsigned int r, const KeysDown &k, const ButtonsDown &b ) : BaseClass ( k, b ), _button ( r ){}
  MouseRelease ( const MouseRelease &e ) : BaseClass ( e ), _button ( e._button ){}
  MouseRelease &operator = ( const MouseRelease &e ) { _button = e._button; BaseClass::operator = ( e ); return *this; }
  ~MouseRelease(){}
  unsigned int button() const { return _button; }
private:
  unsigned int _button;
  DISPLAY_EVENTS_EVENT_CLASS ( 1650497863u );
};


///////////////////////////////////////////////////////////////////////////////
//
//  Mouse double-click event class.
//
///////////////////////////////////////////////////////////////////////////////

class DISPLAY_LIBRARY_EXPORT MouseDoubleClick : public Event
{
public:
  typedef Event BaseClass;
  MouseDoubleClick ( unsigned int c, const KeysDown &k, const ButtonsDown &b ) : BaseClass ( k, b ), _button ( c ){}
  MouseDoubleClick ( const MouseDoubleClick &e ) : BaseClass ( e ), _button ( e._button ){}
  MouseDoubleClick &operator = ( const MouseDoubleClick &e ) { _button = e._button; BaseClass::operator = ( e ); return *this; }
  ~MouseDoubleClick(){}
  unsigned int button() const { return _button; }
private:
  unsigned int _button;
  DISPLAY_EVENTS_EVENT_CLASS ( 4031057500u );
};


///////////////////////////////////////////////////////////////////////////////
//
//  Mouse wheel event class.
//
///////////////////////////////////////////////////////////////////////////////

class DISPLAY_LIBRARY_EXPORT MouseWheel : public Event
{
public:
  typedef Event BaseClass;
  MouseWheel ( double d, const KeysDown &k, const ButtonsDown &b ) : BaseClass ( k, b ), _delta ( d ){}
  MouseWheel ( const MouseWheel &e ) : BaseClass ( e ), _delta ( e._delta ){}
  MouseWheel &operator = ( const MouseWheel &e ) { _delta = e._delta; BaseClass::operator = ( e ); return *this; }
  ~MouseWheel(){}
  double delta() const { return _delta; }
private:
  double _delta;
  DISPLAY_EVENTS_EVENT_CLASS ( 2805090724u );
};


///////////////////////////////////////////////////////////////////////////////
//
//  Key press event class.
//
///////////////////////////////////////////////////////////////////////////////

class DISPLAY_LIBRARY_EXPORT KeyPress : public Event
{
public:
  typedef Event BaseClass;
  KeyPress ( int c, const KeysDown &k, const ButtonsDown &b ) : BaseClass ( k, b ), _key ( c ){}
  KeyPress ( const KeyPress &e ) : BaseClass ( e ), _key ( e._key ){}
  KeyPress &operator = ( const KeyPress &e ) { _key = e._key; BaseClass::operator = ( e ); return *this; }
  ~KeyPress(){}
  int key() const { return _key; }
private:
  int _key;
  DISPLAY_EVENTS_EVENT_CLASS ( 3272704351u );
};


///////////////////////////////////////////////////////////////////////////////
//
//  Key release event class.
//
///////////////////////////////////////////////////////////////////////////////

class DISPLAY_LIBRARY_EXPORT KeyRelease : public Event
{
public:
  typedef Event BaseClass;
  KeyRelease ( int c, const KeysDown &k, const ButtonsDown &b ) : BaseClass ( k, b ), _key ( c ){}
  KeyRelease ( const KeyRelease &e ) : BaseClass ( e ), _key ( e._key ){}
  KeyRelease &operator = ( const KeyRelease &e ) { _key = e._key; BaseClass::operator = ( e ); return *this; }
  ~KeyRelease(){}
  int key() const { return _key; }
private:
  int _key;
  DISPLAY_EVENTS_EVENT_CLASS ( 3952639119u );
};


}; // namespace Events
}; // namespace Display


#endif // _DISPLAY_LIBRARY_EVENT_CLASSES_H_
