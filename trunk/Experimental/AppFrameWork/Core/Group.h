
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

  // Append a window.
  void                                append ( Window * );

  // Iterators to the contained windows.
  Itr                                 begin();
  ConstItr                            begin() const;

  // Iterators to the contained windows.
  Itr                                 end();
  ConstItr                            end() const;

  // Insert a window.
  void                                insert ( Itr where, Window * );

  // Prepend a window.
  void                                prepend ( Window * );

protected:

  // Use reference counting.
  virtual ~Group();

private:

  // No copying.
  Group ( const Group & );
  Group &operator = ( const Group & );

  Windows _windows;
};


} // namespace Core
} // namespace AFW


#endif //_APP_FRAME_WORK_GROUP_CLASS_H_
