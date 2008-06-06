
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

#include "AppFrameWork/Windows/Window.h"

#include <vector>


namespace AFW {
namespace Windows {


class APP_FRAME_WORK_EXPORT Group : public Window
{
public:

  // Typedefs.
  typedef Window BaseClass;
  typedef std::vector < Window::ValidRefPtr > Windows;
  typedef Windows::iterator Itr;
  typedef Windows::const_iterator ConstItr;

  // Accept the visitor.
  virtual void                        accept ( AFW::Core::BaseVisitor * );

  // Append a window.
  bool                                append ( Window * );

  // Iterators to the contained windows.
  Itr                                 begin();
  ConstItr                            begin() const;

  // Remove all children.
  void                                clear();

  // Write configuration to disk.
  virtual void                        configWrite() const;

  // Iterators to the contained windows.
  Itr                                 end();
  ConstItr                            end() const;

  // Find the child window(s).
  ConstItr                            find ( Window *w ) const;
  Itr                                 find ( Window *w );

  // Insert a window.
  virtual bool                        insert ( Itr where, Window * );

  // Return the number of children.
  unsigned int                        numChildren() const;

  // Remove window(s).
  void                                remove ( Window * );
  void                                remove ( unsigned int );
  void                                removeAll();

  // Prepend a window.
  void                                prepend ( Window * );

  // Hide/show the child.
  virtual void                        visible ( Window *, bool );

protected:

  // Constructor
  Group();

  // Use reference counting.
  virtual ~Group();

  virtual void                        _traverse ( AFW::Core::BaseVisitor * );

private:

  // No copying.
  Group ( const Group & );
  Group &operator = ( const Group & );

  friend class BaseVisitor; // Calls _traverse()

  Windows _windows;

  AFW_DECLARE_OBJECT ( Group );
};


} // namespace Windows
} // namespace AFW


#endif //_APP_FRAME_WORK_GROUP_CLASS_H_
