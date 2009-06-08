
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Displays text and icons.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_LOG_WINDOW_CLASS_H_
#define _APP_FRAME_WORK_LOG_WINDOW_CLASS_H_

#include "AppFrameWork/Windows/Window.h"

#include <string>


namespace AFW {
namespace Windows {


class APP_FRAME_WORK_EXPORT LogWindow : public Window
{
public:

  // Typedefs.
  typedef Window BaseClass;

  // Accept the visitor.
  virtual void                        accept ( AFW::Core::BaseVisitor * );

  // If this string is not empty then it is interpreted as the menu in which 
  // a check-button should be added to hide/show this window.
  void                                menu ( const std::string &text );
  std::string                         menu() const;

protected:

  // Constructor
  LogWindow();

  // Use reference counting.
  virtual ~LogWindow();

  void                                _makeRequestedControls();

private:

  // No copying.
  LogWindow ( const LogWindow & );
  LogWindow &operator = ( const LogWindow & );

  std::string _menu;

  AFW_DECLARE_OBJECT ( LogWindow );
};


} // namespace Windows
} // namespace AFW


#endif //_APP_FRAME_WORK_LOG_WINDOW_CLASS_H_
