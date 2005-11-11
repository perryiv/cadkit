
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all windows.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_WINDOW_CLASS_H_
#define _APP_FRAME_WORK_WINDOW_CLASS_H_

#include "AppFrameWork/Core/Icon.h"
#include "AppFrameWork/Core/Constants.h"
#include "AppFrameWork/Actions/CommandAction.h"
#include "AppFrameWork/Actions/UpdateAction.h"
#include "AppFrameWork/Conditions/Condition.h"

#include "Usul/Math/Vector2.h"
#include "Usul/Devices/BaseDevice.h"
#include "Usul/Interfaces/IGUIServer.h"

#include <vector>
#include <list>
#include <sstream>

namespace AFW { namespace Core { class Group; class BaseVisitor; } }


namespace AFW {
namespace Core {


class APP_FRAME_WORK_EXPORT Window : public Object
{
public:

  // Typedefs.
  typedef Object BaseClass;
  typedef AFW::Actions::CommandAction CommandAction;
  typedef std::vector < CommandAction::RefPtr > CommandActions;
  typedef CommandActions::iterator CommandActionsItr;
  typedef CommandActions::const_iterator CommandActionsConstItr;
  typedef AFW::Conditions::Condition Condition;
  typedef AFW::Actions::UpdateAction UpdateAction;
  typedef std::pair < Condition::RefPtr, UpdateAction::RefPtr > UpdatePair;
  typedef std::vector < UpdatePair > UpdatePairs;
  typedef UpdatePairs::iterator UpdatePairsItr;
  typedef UpdatePairs::const_iterator UpdatePairsConstItr;
  typedef std::list < Window * > WindowList; // Not reference-counted by design!
  typedef WindowList::iterator WindowListItr;
  typedef Usul::Interfaces::IGUIServer IGUIServer;
  typedef Usul::Base::Referenced GuiObject;
  typedef USUL_REF_POINTER ( GuiObject ) GuiObjectPtr;
  typedef std::pair < AFW::Core::DockSite::Type, unsigned int > DockState;
  typedef Usul::Devices::BaseDevice BaseDevice;
  typedef std::vector < BaseDevice::RefPtr > BaseDevices;
  typedef BaseDevices::iterator DevicesItr;
  typedef BaseDevices::const_iterator DevicesConstItr;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Window );

  // Iterators to the list of all windows.
  static WindowListItr                allWindowsBegin();
  static WindowListItr                allWindowsEnd();

  // Accept the visitor.
  virtual void                        accept ( AFW::Core::BaseVisitor * );

  // Append actions and devices.
  virtual void                        append ( AFW::Actions::CommandAction * );
  virtual void                        append ( AFW::Conditions::Condition *, AFW::Actions::UpdateAction * );
  virtual void                        append ( BaseDevice * );

  // Call all the actions.
  void                                callCommandActions();
  void                                callUpdateActions();

  // Iterators to the command-actions.
  CommandActionsConstItr              commandsBegin() const;
  CommandActionsItr                   commandsBegin();
  CommandActionsConstItr              commandsEnd() const;
  CommandActionsItr                   commandsEnd();

  // Iterators to the devices.
  DevicesConstItr                     devicesBegin() const;
  DevicesItr                          devicesBegin();
  DevicesConstItr                     devicesEnd() const;
  DevicesItr                          devicesEnd();

  // Set/get the dirty flag.
  virtual void                        dirty ( bool );
  virtual bool                        dirty() const;

  // Set/get the docked site.
  virtual void                        dockState ( DockState );
  virtual DockState                   dockState() const;

  // Set/get the handle to the graphical object. The gui-server uses these.
  void                                guiObject ( GuiObject * );
  const GuiObject *                   guiObject() const;
  GuiObject *                         guiObject();

  // Set/get the icon.
  void                                icon ( Icon * );
  const Icon *                        icon() const;
  Icon *                              icon();

  // Return the number of actions.
  unsigned int                        numCommandActions() const;
  unsigned int                        numUpdateActions() const;

  // Get the parent.
  const Group *                       parent() const;
  Group *                             parent();

  // Set/get the percentage of the parent this window should take up.
  // This is a hint, and thus, not always used.
  void                                percent ( float x, float y );
  void                                percent ( const Usul::Math::Vec2f & );
  Usul::Math::Vec2f                   percent() const;

  // Set/get/append the text.
  virtual void                        textAppend ( const std::string &t );
  virtual void                        textAppend ( const char *t, unsigned int length );
  template < class T > void           textAppend ( T value );
  virtual std::string                 textGet() const;
  virtual void                        textGet ( std::string & ) const;
  virtual void                        textSet ( const std::string &t );
  virtual void                        textSet ( const char *t, unsigned int length );

  // Set/get the title.
  virtual void                        title ( const std::string & );
  virtual const std::string &         title() const;

  // Iterators to the update-actions.
  UpdatePairsConstItr                 updatesBegin() const;
  UpdatePairsItr                      updatesBegin();
  UpdatePairsConstItr                 updatesEnd() const;
  UpdatePairsItr                      updatesEnd();

protected:

  // Constructor
  Window ( const std::string &text = "", Icon *icon = 0x0 );

  // Use reference counting.
  virtual ~Window();

  void                                _setParent ( Group * );

  virtual void                        _traverse ( AFW::Core::BaseVisitor * );

private:

  // No copying.
  Window ( const Window & );
  Window &operator = ( const Window & );

  friend class Group;       // Calls _setParent()
  friend class BaseVisitor; // Calls _traverse()

  static WindowList _allWindows;
  WindowListItr _whichWindow;
  AFW::Core::State::Type _flags;
  Group *_parent;
  Icon::RefPtr _icon;
  std::string _text;
  std::string _title;
  CommandActions _commands;
  UpdatePairs _updates;
  Usul::Math::Vec2f _percent;
  GuiObjectPtr _guiObject;
  DockState _dockState;
  BaseDevices _devices;
};


// Append generic type to text.
template < class T > inline void Window::textAppend ( T value )
{
  std::ostringstream s;
  s << value;
  this->textAppend ( s.str() );
  return *this;
}


// Output operator to add text.
template < class T > inline Window &operator << ( Window &window, T value )
{
  window.textAppend ( value );
  return window;
}


} // namespace Core
} // namespace AFW


#endif //_APP_FRAME_WORK_WINDOW_CLASS_H_
