
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all groups.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_GROUP_CLASS_H_
#define _APP_FRAME_WORK_GROUP_CLASS_H_

#include "AppFrameWork/Core/Window.h"

#include <vector>


namespace AFW {
namespace Core {


class APP_FRAME_WORK_EXPORT Group : public Window
{
public:

  // Typedefs.
  typedef Window BaseClass;
  typedef std::vector < Window::ValidRefPtr > Windows;
  typedef Windows::iterator Itr;
  typedef Windows::const_iterator ConstItr;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Group );

  // Constructor
  Group ( const std::string &text = "", Icon *icon = 0x0 );

  // Accept the visitor.
  virtual void                        accept ( AFW::Core::BaseVisitor * );

  // Append a window.
  void                                append ( Window * );

  // Append an action.
  void                                append ( AFW::Actions::CommandAction * );
  void                                append ( AFW::Conditions::Condition *, AFW::Actions::UpdateAction * );

  // Iterators to the contained windows.
  Itr                                 begin();
  ConstItr                            begin() const;

  // Iterators to the contained windows.
  Itr                                 end();
  ConstItr                            end() const;

  // Find the child window.
  ConstItr                            find ( Window *w ) const;
  Itr                                 find ( Window *w );

  // Insert a window.
  void                                insert ( Itr where, Window * );

  // Return the number of children.
  unsigned int                        numChildren() const;

  // Remove a window.
  void                                remove ( Window * );

  // Prepend a window.
  void                                prepend ( Window * );

protected:

  // Use reference counting.
  virtual ~Group();

  virtual void                        _traverse ( AFW::Core::BaseVisitor * );

private:

  // No copying.
  Group ( const Group & );
  Group &operator = ( const Group & );

  friend class BaseVisitor; // Calls _traverse()

  Windows _windows;
};


} // namespace Core
} // namespace AFW


#endif //_APP_FRAME_WORK_GROUP_CLASS_H_
