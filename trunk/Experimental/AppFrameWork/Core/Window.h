
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
#include "AppFrameWork/Actions/CommandAction.h"
#include "AppFrameWork/Actions/UpdateAction.h"
#include "AppFrameWork/Conditions/Condition.h"

#include <vector>


namespace AFW {
namespace Core {

class Group;

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

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Window );

  // Possible flags
  enum
  {
    DIRTY = 0x01
  };

  // Append an action.
  void                                append ( AFW::Actions::CommandAction * );
  void                                append ( AFW::Conditions::Condition *, AFW::Actions::UpdateAction * );

  // Call all the actions.
  void                                callCommandActions ( Window *, Usul::Base::Referenced *data );
  void                                callUpdateActions  ( Window *, Usul::Base::Referenced *data );

  // Iterators to the command-actions.
  CommandActionsConstItr              commandsBegin() const;
  CommandActionsItr                   commandsBegin();
  CommandActionsConstItr              commandsEnd() const;
  CommandActionsItr                   commandsEnd();
 
  // Set/get the dirty flag.
  virtual void                        dirty ( bool );
  virtual bool                        dirty() const;

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

  // Set/get the text.
  const std::string &                 text() const;
  void                                text ( const std::string &t );

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

private:

  // No copying.
  Window ( const Window & );
  Window &operator = ( const Window & );

  // Calls _setParent()
  friend class Group;

  unsigned int _flags;
  Group *_parent;
  Icon::RefPtr _icon;
  std::string _text;
  CommandActions _commands;
  UpdatePairs _updates;
};


} // namespace Core
} // namespace AFW


#endif //_APP_FRAME_WORK_WINDOW_CLASS_H_
