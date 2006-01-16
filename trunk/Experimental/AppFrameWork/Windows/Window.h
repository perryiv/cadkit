
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
#include "AppFrameWork/Core/Types.h"
#include "AppFrameWork/Actions/CommandAction.h"
#include "AppFrameWork/Actions/UpdateAction.h"
#include "AppFrameWork/Conditions/Condition.h"

#include "Usul/Devices/BaseDevice.h"

#include <vector>
#include <list>
#include <sstream>

namespace AFW { namespace Core { class BaseVisitor; } }
namespace AFW { namespace Windows { class Group; } }


namespace AFW {
namespace Windows {


  class APP_FRAME_WORK_EXPORT Window : public AFW::Core::Object
{
public:

  // Typedefs.
  typedef AFW::Core::Object BaseClass;
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
  typedef Usul::Threads::Variable < WindowList > WindowListVar;
  typedef WindowList::iterator WindowListItr;
  typedef std::pair < AFW::Core::DockSite::Type, unsigned int > DockState;
  typedef Usul::Devices::BaseDevice BaseDevice;
  typedef std::vector < BaseDevice::RefPtr > BaseDevices;
  typedef BaseDevices::iterator DevicesItr;
  typedef BaseDevices::const_iterator DevicesConstItr;
  typedef AFW::Core::Types::Position Position;
  typedef AFW::Core::Types::Size Size;

  // Iterators to the list of all windows.
  static WindowListItr                allWindowsBegin();
  static WindowListItr                allWindowsEnd();

  // Accept the visitor.
  virtual void                        accept ( AFW::Core::BaseVisitor * );

  // Append actions and devices.
  virtual void                        append ( AFW::Actions::CommandAction * );
  virtual void                        append ( AFW::Conditions::Condition *, AFW::Actions::UpdateAction * );
  virtual void                        append ( AFW::Actions::UpdateAction * );
  virtual void                        append ( BaseDevice * );

  // Call all the actions. Pass false to execute during idle processing.
  void                                callCommandActions ( bool immediate = true );
  void                                callUpdateActions  ( bool immediate = true );

  // Write configuration to disk.
  virtual void                        configWrite() const;

  // Called automatically when this window is added to another.
  virtual bool                        create ( Window * );

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

  // Enable/disable the window, or get that state.
  virtual void                        enable ( bool );
  virtual bool                        enabled() const;

  // Set/get the flags.
  void                                flags ( AFW::Core::State::Type );
  AFW::Core::State::Type              flags() const;

  // Set/get the icon.
  void                                icon ( const AFW::Core::Icon & );
  AFW::Core::Icon                     icon() const;

  // Return the number of actions.
  unsigned int                        numCommandActions() const;
  unsigned int                        numUpdateActions() const;

  // Get the parent.
  const Group *                       parent() const;
  Group *                             parent();

  // Set/get the persistent name. This string is used for saving and 
  // restoring state, for example, in the registry.
  void                                persistentName ( const std::string & );
  std::string                         persistentName() const;

  // Set/get the position.
  virtual void                        position ( const Position & );
  virtual Position                    position() const;

  // Scroll to the end.
  virtual void                        scrollToEnd();

  // Set/get window size.
  virtual void                        size ( const Size & );
  virtual Size                        size() const;

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
  virtual std::string                 title() const;

  // Iterators to the update-actions.
  UpdatePairsConstItr                 updatesBegin() const;
  UpdatePairsItr                      updatesBegin();
  UpdatePairsConstItr                 updatesEnd() const;
  UpdatePairsItr                      updatesEnd();

  // Set/get the visible state.
  virtual void                        visible ( bool );
  virtual bool                        visible() const;

protected:

  // Constructor
  Window();

  // Use reference counting.
  virtual ~Window();

  void                                _setParent ( Group * );

  virtual void                        _traverse ( AFW::Core::BaseVisitor * );

private:

  // No copying.
  Window ( const Window & );
  Window &operator = ( const Window & );

  friend class AFW::Windows::Group;    // Calls _setParent()
  friend class AFW::Core::BaseVisitor; // Calls _traverse()

  static WindowListVar _allWindows;
  WindowListItr _whichWindow;
  AFW::Core::State::Type _flags;
  Group *_parent;
  AFW::Core::Icon _icon;
  std::string _text;
  std::string _title;
  std::string _regName;
  CommandActions _commands;
  UpdatePairs _updates;
  DockState _dockState;
  BaseDevices _devices;

  AFW_DECLARE_OBJECT ( Window );
};


// Append generic type to text.
template < class T > inline void Window::textAppend ( T value )
{
  std::ostringstream s;
  s << value;
  this->textAppend ( s.str() );
}


} // namespace Windows
} // namespace AFW


#endif //_APP_FRAME_WORK_WINDOW_CLASS_H_
